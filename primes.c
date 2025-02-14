#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

const unsigned long long SEGMENT_SIZE = 100000ULL;
const unsigned long long UPPER_LIMIT = 1000000ULL;
#define PRIME_FILE "primes.bin"
#define TMP_FILE "tmp_primes.bin"

typedef struct {
    unsigned long long prime;
    unsigned long long next_multiple;
} PrimeEntry;

// Global flags for signal handling
volatile sig_atomic_t interrupted = 0;
volatile sig_atomic_t pipe_broken = 0;

void handle_sigint(int sig) {
    interrupted = 1;
    const char msg[] = "\nReceived interrupt. Finishing current segment...\n";
    write(STDERR_FILENO, msg, sizeof(msg)-1); // Safe in signal handler
}

void handle_sigpipe(int sig) {
    pipe_broken = 1;
    const char msg[] = "\nPipe closed. Finishing current segment...\n";
    write(STDERR_FILENO, msg, sizeof(msg)-1);
}

void process_segment(unsigned long long segment) {
    const unsigned long long low = segment * SEGMENT_SIZE;
    const unsigned long long high = low + SEGMENT_SIZE - 1;
    bool is_prime[SEGMENT_SIZE];
    memset(is_prime, true, sizeof(is_prime));

    if (segment == 0) {
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
        FILE *tmp_fp = fopen(TMP_FILE, "wb");
        FILE *prime_fp = fopen(PRIME_FILE, "rb");
        
        PrimeEntry pe;
        while (fread(&pe, sizeof(PrimeEntry), 1, prime_fp) == 1) {
            unsigned long long current = pe.next_multiple;
            
            while (current < low) {
                unsigned long long new_current;
                if (__builtin_uaddll_overflow(current, pe.prime, &new_current)) {
                    fprintf(stderr, "Addition overflow: %llu + %llu\n", current, pe.prime);
                    exit(EXIT_FAILURE);
                }
                current = new_current;
            }

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

void print_primes() {
    FILE *fp = fopen(PRIME_FILE, "rb");
    PrimeEntry pe;
    while (fread(&pe, sizeof(PrimeEntry), 1, fp) == 1) {
        if (pe.prime > UPPER_LIMIT || pipe_broken || interrupted) break;
        printf("%llu\n", pe.prime);
        fflush(stdout); // Ensure output gets through the pipe
    }
    fclose(fp);
}

int main() {
    struct sigaction sa_int, sa_pipe;
    
    // Configure SIGINT handler
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    // Configure SIGPIPE handler
    sa_pipe.sa_handler = handle_sigpipe;
    sigemptyset(&sa_pipe.sa_mask);
    sa_pipe.sa_flags = SA_RESTART;
    sigaction(SIGPIPE, &sa_pipe, NULL);

    remove(PRIME_FILE);
    const unsigned long long num_segments = (UPPER_LIMIT + SEGMENT_SIZE - 1) / SEGMENT_SIZE;
    
    for (unsigned long long seg = 0; seg < num_segments; ++seg) {
        process_segment(seg);
        if (interrupted || pipe_broken) {
            fprintf(stderr, "Early exit after completing segment %llu\n", seg);
            break;
        }
    }
    
    print_primes();
    return 0;
}
