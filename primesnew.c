/**
 * @file primes3.c
 * @brief Segmented Sieve of Eratosthenes implementation for large prime number generation
 * 
 * This program efficiently calculates all prime numbers up to a given upper limit (1,000,000 by default)
 * using a segmented sieve approach. It handles 64-bit integers safely with overflow checks and stores results
 * in a binary file format. The implementation processes numbers in segments to maintain memory efficiency.
 * 
 * Key features:
 * - Segmented sieve algorithm for memory optimization
 * - 64-bit unsigned integer support for large number ranges
 * - Overflow/underflow protection in all arithmetic operations
 * - Binary file storage of primes with their next multiples for efficient sieving
 * - Multi-segment processing with temporary file management
 * 
 * Constants:
 * - SEGMENT_SIZE: Numbers processed per segment (100,000)
 * - UPPER_LIMIT: Maximum number to check for primality (1,000,000)
 * - PRIME_FILE: Output binary file storing PrimeEntry structures
 * - TMP_FILE: Temporary file used during segment processing
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

const unsigned long long SEGMENT_SIZE = 100000ULL; ///< Numbers processed per sieve segment
const unsigned long long UPPER_LIMIT = 1000000ULL; ///< Maximum number to check for primality
#define PRIME_FILE "primes.bin"      ///< Output file for storing prime entries
#define TMP_FILE "tmp_primes.bin"    ///< Temporary file used during processing

/**
 * @brief Structure storing prime numbers and their next multiple
 * 
 * Used to persist sieve state between segments. For each prime found,
 * we store the next multiple that will appear in future segments.
 */
typedef struct {
    unsigned long long prime;         ///< Prime number value
    unsigned long long next_multiple; ///< Next multiple of this prime to check
} PrimeEntry;

/**
 * @brief Process a single segment of the number range
 * 
 * Handles both initial segment (using standard sieve) and subsequent segments
 * (using persisted prime data). Marks non-primes in current segment and updates
 * prime multiples for future segments.
 * 
 * @param segment Segment number to process (0-based index)
 * 
 * @note For segment 0:
 * - Initializes sieve array
 * - Runs base Sieve of Eratosthenes
 * - Stores initial primes in PRIME_FILE
 * 
 * For other segments:
 * - Reads existing primes from PRIME_FILE
 * - Marks multiples in current segment
 * - Updates next multiples in TMP_FILE
 * - Atomically replaces old prime file with updated data
 */
void process_segment(unsigned long long segment) {
    const unsigned long long low = segment * SEGMENT_SIZE;
    const unsigned long long high = low + SEGMENT_SIZE - 1;
    bool is_prime[SEGMENT_SIZE];
    memset(is_prime, true, sizeof(is_prime));

    if (segment == 0) {
        /* Handle initial segment with standard sieve */
        is_prime[0] = is_prime[1] = false;

        for (unsigned long long i = 2; i*i <= high; ++i) {
            if (is_prime[i]) {
                unsigned long long square;
                if (__builtin_umulll_overflow(i, i, &square)) {
                    fprintf(stderr, "Multiplication overflow: %llu\n", i);
                    exit(EXIT_FAILURE);
                }
                
                for (unsigned long long j = square; j <= high; j += i) {
                    is_prime[j] = false;
                }
            }
        }
    } else {
        /* Process subsequent segments using existing primes */
        FILE *tmp_fp = fopen(TMP_FILE, "wb");
        FILE *prime_fp = fopen(PRIME_FILE, "rb");
        
        PrimeEntry pe;
        while (fread(&pe, sizeof(PrimeEntry), 1, prime_fp) == 1) {
            unsigned long long current = pe.next_multiple;
            
            /* Adjust current multiple to current segment */
            while (current < low) {
                unsigned long long new_current;
                if (__builtin_uaddll_overflow(current, pe.prime, &new_current)) {
                    fprintf(stderr, "Addition overflow: %llu + %llu\n", current, pe.prime);
                    exit(EXIT_FAILURE);
                }
                current = new_current;
            }

            /* Mark composites in current segment */
            pe.next_multiple = current;
            while (pe.next_multiple <= high) {
                unsigned long long index;
                if (__builtin_usubll_overflow(pe.next_multiple, low, &index)) {
                    fprintf(stderr, "Subtraction underflow: %llu - %llu\n", pe.next_multiple, low);
                    exit(EXIT_FAILURE);
                }
                is_prime[index] = false;

                unsigned long long next;
                if (__builtin_uaddll_overflow(pe.next_multiple, pe.prime, &next)) {
                    fprintf(stderr, "Addition overflow: %llu + %llu\n", pe.next_multiple, pe.prime);
                    exit(EXIT_FAILURE);
                }
                pe.next_multiple = next;
            }

            fwrite(&pe, sizeof(PrimeEntry), 1, tmp_fp);
        }

        fclose(prime_fp);
        fclose(tmp_fp);
        remove(PRIME_FILE);
        rename(TMP_FILE, PRIME_FILE);
    }

    /* Write new primes to output file */
    FILE *fp = fopen(PRIME_FILE, segment == 0 ? "wb" : "ab");
    const unsigned long long start = (segment == 0) ? 2 : 0;
    
    for (unsigned long long i = start; i < SEGMENT_SIZE; ++i) {
        const unsigned long long num = low + i;
        if (num > UPPER_LIMIT) break;
        
        if (is_prime[i]) {
            unsigned long long next_multiple;
            if (__builtin_umulll_overflow(num, num, &next_multiple)) {
                fprintf(stderr, "Multiplication overflow: %llu\n", num);
                exit(EXIT_FAILURE);
            }
            
            PrimeEntry pe = {num, next_multiple};
            fwrite(&pe, sizeof(PrimeEntry), 1, fp);
        }
    }
    fclose(fp);
}

/**
 * @brief Print primes from binary file to stdout
 * 
 * Reads PrimeEntry structures from PRIME_FILE and prints prime values
 * up to UPPER_LIMIT. Ensures clean output even if file contains
 * data beyond the specified limit.
 */
void print_primes() {
    FILE *fp = fopen(PRIME_FILE, "rb");
    PrimeEntry pe;
    while (fread(&pe, sizeof(PrimeEntry), 1, fp) == 1) {
        if (pe.prime > UPPER_LIMIT) break;
        printf("%llu\n", pe.prime);
    }
    fclose(fp);
}

/**
 * @brief Main program controller
 * 
 * Orchestrates the segmented sieve process:
 * 1. Removes previous prime file
 * 2. Calculates required number of segments
 * 3. Processes each segment sequentially
 * 4. Prints final results
 * 
 * @return EXIT_SUCCESS upon normal termination
 */
int main() {
    remove(PRIME_FILE);
    const unsigned long long num_segments = (UPPER_LIMIT + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
    
    for (unsigned long long seg = 0; seg < num_segments; ++seg) {
        process_segment(seg);
    }
    
    print_primes();
    return EXIT_SUCCESS;
}
