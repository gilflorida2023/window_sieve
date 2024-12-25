#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <limits.h>
//========================================
//========================================
//==SORTED=AND=UNIQUE=LIST================
//========================================
//========================================

#include <stdbool.h>

// 78498 is the number of primes found within 1 million, excluding 1
#define MAX_MAINLIST_SIZE 78498

#define MAX_FACTORS 100
// Function to check if a value is already present in the MAINLIST using binary search
bool is_present(unsigned long long mainlist[], int mainlist_size, unsigned long long value) {
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

// Function to find the insertion point using binary search.
// Returns the index where the value should be inserted.
// If the value is already present, it returns the index of the existing value.
int find_insertion_point(unsigned long long mainlist[], int mainlist_size, unsigned long long value) {
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

int merge(unsigned long long mainlist[], int *mainlist_size_ptr, unsigned long long small_array[], int small_array_size) {
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

/*
    PARAMETERS:
        arr -array of integers to dump.
       size - number of integers to print.
       message - description of date
*/
void print_array(unsigned long long arr[], int size, const char* name) {
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
//========================================
//========================================
//========================================
//========================================
//========================================


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

/*
find prime factors of n.
updates the factor array with the list of prime factors.
updates num_factors with the count added to the array.
RETURNS : 0 SUCCESS , -1 TOO MANY FACTORS
*/
int find_factors(unsigned long long  n, int *num_factors,unsigned long long factors[]) {
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

#define IC_SPECIAL "SPECIAL" // 0 and 1
#define IC_PRIME "PRIME" // numbers divisible by one and itself
#define IC_COMPOSITE "COMPOSITE" // the rest.
char * integer_classification(const unsigned long long num, const int num_factors,const unsigned long long * factors) {
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
//========================================
//========================================
//========================================
//========================================
//========================================

/*
    function accepts the prime and count to the next prime.
    prints the factors of each number in the range.
*/
int consecutive_prime_gap_report(const unsigned long long start,const int count) {
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
void print_help() {
    printf("Usage: program [OPTIONS]\n");
    printf("Options:\n");
    printf("  -n, --number  <number>       Specify a place to begin loking at factors.\n");
    printf("  -c, --count  <count>        Specify a count numbers to check.\n");
    printf("  -h, --help                  Display this help message.\n");
}

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
                number = strtoull(optarg, &endptr, 10); // Convert argument to unsigned long long
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
