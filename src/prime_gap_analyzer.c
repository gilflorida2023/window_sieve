/**
 * @file prime_gap_analyzer.c
 * @brief A program to analyze prime factors in a range of numbers.
 *
 * This program takes command-line arguments to specify a starting number
 * and a count of consecutive numbers to analyze for prime factors.
 */

#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
// 78498 is the number of primes found within 1 million
#define MAX_MAINLIST_SIZE 78498

#define MAX_FACTORS 100
/**
 * @brief Check if a value is present in a sorted array using binary search.
 * 
 * @param mainlist The sorted array to search in.
 * @param mainlist_size The size of the array.
 * @param value The value to search for.
 * @return true if the value is found, false otherwise.
 */
bool is_present(const unsigned long long mainlist[], int mainlist_size, unsigned long long value) {
    // Check for invalid input
    if (mainlist == NULL || mainlist_size <= 0) {
        return false;
    }
    int low = 0;
    int high = mainlist_size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2; // Avoid potential overflow
        if (mainlist[mid] == value) {
            return true;
        } else if (mainlist[mid] < value) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return false;
}

/**
 * @brief Find the insertion point for a value in a sorted array using binary search.
 *
 * This function determines where a new value should be inserted in a sorted array
 * to maintain the array's order. If the value already exists, it returns the
 * index of the existing value.
 *
 * @param mainlist The sorted array to search in.
 * @param mainlist_size The size of the array.
 * @param value The value to find an insertion point for.
 * @return The index where the value should be inserted, or -1 if inputs are invalid.
 */
int find_insertion_point(const unsigned long long mainlist[], int mainlist_size, unsigned long long value) {
    if (mainlist == NULL || mainlist_size <= 0) {
        return -1;  // Invalid input
    }
    int low = 0;
    int high = mainlist_size - 1;
    int insertion_point = mainlist_size; // Default: insert at the end

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (mainlist[mid] == value) {
            return mid; // Value already present
        } else if (mainlist[mid] < value) {
            low = mid + 1;
        } else {
            high = mid - 1;
            insertion_point = mid; // Update insertion point
        }
    }
    return insertion_point;
}

/**
 * @brief Merge a small array into a main list while maintaining unique and sorted order.
 *
 * This function inserts elements from the small array into the main list,
 * ensuring that the main list remains sorted and contains only unique elements.
 * If an element from the small array already exists in the main list, it is skipped.
 *
 * @param mainlist The main sorted array to insert into.
 * @param mainlist_size_ptr Pointer to the size of the main list.
 * @param small_array The small array to merge from.
 * @param small_array_size The size of the small array.
 * @return 0 on success, -1 if the main list becomes full.
 */
int merge(unsigned long long mainlist[], int *mainlist_size_ptr, const unsigned long long small_array[], int small_array_size) {
    if (mainlist == NULL || mainlist_size_ptr == NULL || small_array == NULL || 
        *mainlist_size_ptr < 0 || small_array_size <= 0) {
        fprintf(stderr, "Error: Invalid input parameters.\n");
        return -1;
    }
    int mainlist_size = *mainlist_size_ptr;

    for (int i = 0; i < small_array_size; i++) {
        int insertion_point = find_insertion_point(mainlist, mainlist_size, small_array[i]);

        // Check if the value is already present
        if (insertion_point < mainlist_size && mainlist[insertion_point] == small_array[i]) {
            continue; // Skip if already present
        }

        if (mainlist_size >= MAX_MAINLIST_SIZE) {
            fprintf(stderr, "Error: MAINLIST is full.\n");
            return -1;
        }

        // Shift elements to make space
        for (int j = mainlist_size; j > insertion_point; j--) {
            mainlist[j] = mainlist[j - 1];
        }

        // Insert the new element
        mainlist[insertion_point] = small_array[i];
        mainlist_size++;
    }

    *mainlist_size_ptr = mainlist_size;
    return 0;
}

/**
 * @brief Print the contents of an unsigned long long array.
 *
 * This function prints the elements of an array along with a descriptive name and the count of elements.
 * If the array is empty, it prints "(empty)" instead of the elements.
 *
 * @param arr The array of unsigned long long integers to print.
 * @param size The number of elements in the array.
 * @param name A descriptive name or message for the array.
 *
 * @note The function assumes that the provided size is non-negative and accurately represents
 *       the number of elements in the array. It does not perform bounds checking.
 *
 * @warning This function does not check for NULL pointers. Ensure that arr and name are valid pointers before calling.
 *
 * Example usage:
 * @code
 * unsigned long long myArray[] = {1, 2, 3, 4, 5};
 * print_array(myArray, 5, "My Integer Array");
 * @endcode
 */
void print_array(const unsigned long long arr[], int size, const char* name) {
    printf("%s: count: %d;", name, size);
    if (size == 0) {
        printf("(empty)\n");
        return;
    }
    for (int i = 0; i < size; i++) {
        printf("%llu ", arr[i]);
    }
    printf("\n");
}

/**
 * @brief Remove duplicates from a sorted array of unsigned long long integers.
 *
 * This function removes duplicate elements from a sorted array in-place,
 * maintaining the original order of unique elements. It assumes the input
 * array is sorted in ascending order.
 *
 * @param arr The sorted array of unsigned long long integers.
 * @param n The number of elements in the array.
 * @return The new count of unique elements in the array.
 *
 * @note This function modifies the original array.
 * @note Time complexity: O(n), where n is the number of elements in the array.
 * @note Space complexity: O(1), as it operates in-place.
 *
 * @warning This function does not check if the input array is sorted.
 *          Behavior is undefined for unsorted input.
 * @warning No bounds checking is performed. Ensure that n accurately
 *          represents the number of elements in arr.
 *
 * Example usage:
 * @code
 * unsigned long long factors[] = {2, 2, 3, 3, 5, 7, 7};
 * int size = 7;
 * int new_size = remove_duplicates(factors, size);
 * // After this call, factors will be {2, 3, 5, 7} and new_size will be 4
 * @endcode
 */
int remove_duplicates(unsigned long long  arr[], int n) {
    if (n == 0 || n == 1)
        return n;

    int j = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] != arr[j]) {
            j++;
            arr[j] = arr[i];
        }
    }

    return j + 1;
}

/**
 * @brief Find the prime factors of a given number.
 *
 * This function calculates the prime factors of a given unsigned long long integer
 * and stores them in an array. It also removes duplicates from the factor list.
 *
 * @param n The number to factorize.
 * @param num_factors Pointer to an integer where the count of unique prime factors will be stored.
 * @param factors Array to store the prime factors.
 *
 * @return 0 on success, -1 if the number of factors exceeds MAX_FACTORS.
 *
 * @note This function assumes the existence of a global constant MAX_FACTORS
 *       which defines the maximum number of factors that can be stored.
 * @note The function uses the remove_duplicates() helper function to ensure
 *       each prime factor is listed only once in the output.
 *
 * Algorithm:
 * 1. Check divisibility by 2 separately for efficiency.
 * 2. Check divisibility by odd numbers up to the square root of n.
 * 3. If any factor remains greater than 2, it's a prime factor itself.
 * 4. Remove duplicates from the factor list.
 *
 * @warning This function modifies the factors array and the value pointed to by num_factors.
 * @warning Ensure that the factors array has at least MAX_FACTORS elements allocated.
 *
 * Example usage:
 * @code
 * unsigned long long n = 60;
 * int num_factors;
 * unsigned long long factors[MAX_FACTORS];
 * int result = find_factors(n, &num_factors, factors);
 * // factors will contain {2, 3, 5}, num_factors will be 3
 * @endcode
 */
int find_factors(unsigned long long  n, int *num_factors,\
    unsigned long long factors[]) {
    int i, count = 0;

    // Check divisibility by 2
    if (n % 2 == 0) {
        factors[count++] = 2;
        while (n % 2 == 0) {
            n /= 2;
        }
    }

    // Check divisibility by odd numbers up to the square root of n
    for (i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            if (count >= MAX_FACTORS) {
                printf("Error: Too many factors.\n");
                return -1;
            }
            factors[count++] = i;
            n /= i;
        }
    }

    // If n is greater than 2, it's a prime factor
    if (n > 2) {
        if (count >= MAX_FACTORS) {
            printf("Error: Too many factors.\n");
            return -1;
        }
        factors[count++] = n;
    }

    //*num_factors = count;
    *num_factors = remove_duplicates(factors, count) ;
    return 0;
}
#if 0
#define IC_SPECIAL "SPECIAL" // 0 and 1
#define IC_PRIME "PRIME" // numbers divisible by one and itself
#define IC_COMPOSITE "COMPOSITE" // the rest.
/*
based on the number's prime factors rto determibe if its
prime,composite, or special. only accepts positive unsigned
long long integers.
*/
char * integer_classification(const unsigned long long num, \
    const int num_factors,const unsigned long long * factors) {
    char * return_val = NULL;
    if (num == 1ULL || num == 0ULL ) {
        return_val = IC_SPECIAL;
    } else if ( num_factors == 1 && factors[0] == num ){
               return_val = IC_PRIME;
    }else {
        return_val = IC_COMPOSITE;
    }
    return return_val;
}
#else
/**
 * @file integer_classification.h
 * @brief Defines constants and functions for classifying integers.
 */

#define IC_SPECIAL "SPECIAL"  /**< Classification for numbers 0 and 1 */
#define IC_PRIME "PRIME"      /**< Classification for prime numbers */
#define IC_COMPOSITE "COMPOSITE" /**< Classification for composite numbers */

/**
 * @brief Classifies a positive integer based on its prime factors.
 *
 * This function determines whether a given positive integer is special (0 or 1),
 * prime, or composite based on its prime factorization.
 *
 * @param num The number to classify (must be a positive unsigned long long integer).
 * @param num_factors The number of prime factors.
 * @param factors An array containing the prime factors of the number.
 *
 * @return A string constant indicating the classification:
 *         - IC_SPECIAL for 0 and 1
 *         - IC_PRIME for prime numbers
 *         - IC_COMPOSITE for composite numbers
 *
 * @note The function assumes that the input is valid and the factorization is correct.
 *       It does not perform any validation on the input parameters.
 *
 * @warning This function does not allocate memory for the return value.
 *          The returned pointer points to a static string constant.
 *
 * Example usage:
 * @code
 * unsigned long long number = 17;
 * unsigned long long factors[] = {17};
 * char* classification = integer_classification(number, 1, factors);
 * // classification will be "PRIME"
 * @endcode
 */
char* integer_classification(const unsigned long long num,
                             const int num_factors,
                             const unsigned long long* factors) {
    if (num == 1ULL || num == 0ULL) {
        return IC_SPECIAL;
    } else if (num_factors == 1 && factors[0] == num) {
        return IC_PRIME;
    } else {
        return IC_COMPOSITE;
    }
}
#endif
//========================================
//========================================
//========================================
//========================================
//========================================

/**
 * @brief Generate a report of prime factors for a range of consecutive numbers.
 *
 * This function calculates and prints the prime factors for each number in a range
 * starting from a given prime number. It also classifies each number as prime,
 * composite, or special, and maintains a sorted list of unique factors.
 *
 * @param start The starting prime number (inclusive).
 * @param count The number of consecutive integers to process after the start.
 * @return 0 on success, EXIT_FAILURE if an error occurs.
 *
 * @note The function assumes the existence of helper functions:
 *       - find_factors(): to find prime factors of a number
 *       - integer_classification(): to classify a number based on its factors
 *       - merge(): to merge new factors into the main list
 *       - print_array(): to print the final list of unique factors
 *
 * @warning This function uses global constants MAX_MAINLIST_SIZE and MAX_FACTORS.
 *          Ensure these are properly defined before calling this function.
 *
 * Output format:
 * - Prints a header with the start number and count
 * - For each number in the range:
 *   - Prints the number, its factors, and its classification
 * - Prints a sorted list of all unique factors at the end
 *
 * Example usage:
 * @code
 * int result = consecutive_prime_gap_report(17, 5);
 * // This will process numbers 17, 18, 19, 20, 21, 22
 * @endcode
 */
int consecutive_prime_gap_report(unsigned long long start,int count) {
    unsigned long long mainlist[MAX_MAINLIST_SIZE];
    int mainlist_size = 0;
    unsigned long long  factors[MAX_FACTORS];
    const unsigned long long  end = start+(unsigned long long)count;
    printf("#=P R I M E  G A P=\n");
    printf("#%llu,%d\n",start,count);
    for(unsigned long long num=start;num <= end; num++) {
        int  num_factors;
        if (find_factors(num, &num_factors,factors) == 0) {
            printf("%llu,{", num);
            int first=1;
            for (int i = 0; i < num_factors; i++) {
                if (!first) {
                    printf(" ");
                }
                printf("%llu", factors[i]);
                first = 0;
            }
            printf("}");
            char * ic = integer_classification(num, num_factors,factors) ;
            printf(",%s\n",ic);

            // add factors from each prime and or composite into main list.
            if(merge(mainlist, &mainlist_size, factors, num_factors) != 0){
                fprintf(stderr, "Error: MAINLIST is full.\n");
                return EXIT_FAILURE;
            }
        }
    }
    // display sorted and unique prime factors.
    print_array(mainlist, mainlist_size, "#Factors ");
    return 0;
}

#ifndef FIND_FACTOR_LIB
/**
 * @brief Prints the help message showing program usage and options.
 */
void print_help() {
    printf("Usage: program [OPTIONS]\n");
    printf("Options:\n");
    printf("  -n, --number  <number>       Specify a place to begin looking at factors.\n");
    printf("  -c, --count  <count>        Specify a count numbers to check.\n");
    printf("  -h, --help                  Display this help message.\n");
}

/**
 * @brief Main function to parse command-line arguments and run the prime factor analysis.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return EXIT_SUCCESS on successful execution, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[]) {

    // Check if no arguments were provided (argc should be 1)
    if (argc == 1) {
        print_help();
        return EXIT_SUCCESS;
    }

    // Variables to store the option values
    unsigned long long number = 0;
    int count = 0;

    // Define the long options
    static struct option long_options[] = {
        {"number", required_argument, 0, 'n'},
        {"count", required_argument, 0, 'c'},
        {"help", no_argument, 0,'h'},
        {0, 0, 0, 0} // Terminator
    };

    int opt;
    int option_index = 0;

    char *endptr;
    while ((opt = getopt_long(argc, argv, "n:c:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'n': // Handle -n or --number
                number = strtoull(optarg, &endptr, 10);
                fprintf(stderr, "number: %s,%llu\n", optarg,number);
                if (number == 0 && optarg[0] != '0') {
                    fprintf(stderr, "Invalid number: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                if (*endptr != '\0') {
                    fprintf(stderr,"Invalid input: not a number\n");
                    return EXIT_FAILURE;
                }
                break;

            case 'c': // Handle -c or --count
                unsigned long value = strtoul(optarg, &endptr, 10);
                if (value >= UINT_MAX || value == 0llu){
                //if (count <= 0) {
                    fprintf(stderr, "Invalid count: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                if (*endptr != '\0') {
                    fprintf(stderr,"Invalid input: not a number\n");
                    return EXIT_FAILURE;
                }
                count = value;
                fprintf(stderr, "count: %s,%d\n", optarg,count);
                break;

           case 'h': // Handle -h option
                print_help();
                return EXIT_SUCCESS;

            case '?': // Unknown option or missing argument
                fprintf(stderr, "Usage: %s [-n|--number <unsigned long long>] [-c|--count <int>]\n", argv[0]);
                return EXIT_FAILURE;

            default:
                abort(); // Should not reach here
        }
    }
    if ( count == 0l || number == 0ull ){
       fprintf(stderr, "Must specify both count and number. count and number must be  >0.\nUsage: %s [-n|--number <unsigned long long>] [-c|--count <int>]\n", argv[0]);
       return EXIT_FAILURE;
    }
    consecutive_prime_gap_report(number,count);
    return EXIT_SUCCESS;
}
#endif
