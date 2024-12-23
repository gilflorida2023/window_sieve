#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <limits.h>
//========================================
//========================================
//========================================
//========================================
//========================================

#include <stdbool.h>

#define MAX_MAINLIST_SIZE 78498
//#define MAX_SMALL_ARRAY_SIZE 100
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

// Function to merge a small array into the MAINLIST while maintaining sorted order and uniqueness
int merge(unsigned long long mainlist[], int *mainlist_size_ptr, unsigned long long small_array[], int small_array_size) {
    int mainlist_size = *mainlist_size_ptr;
    for (int i = 0; i < small_array_size; i++) {
        if (!is_present(mainlist, mainlist_size, small_array[i])) {
            if (mainlist_size >= MAX_MAINLIST_SIZE) {
                fprintf(stderr, "Error: MAINLIST is full.\n");
                return -1; // Indicate an error
            }

            int j = mainlist_size;
            while (j > 0 && mainlist[j - 1] > small_array[i]) {
                mainlist[j] = mainlist[j - 1];
                j--;
            }
            mainlist[j] = small_array[i];
            mainlist_size++;
        }
    }
    *mainlist_size_ptr = mainlist_size;
    return 0; // Indicate success
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


void print_help() {
    printf("Usage: program [OPTIONS]\n");
    printf("Options:\n");
    printf("  -n, --number  <number>       Specify a place to begin loking at factors.\n");
    printf("  -c, --count  <count>        Specify a count numbers to check.\n");
    printf("  -h, --help                  Display this help message.\n");
}
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
aRETURNS : 0 SUCCESS , -1 TOO MANY FACTORS
*/
int find_factors(unsigned long long  n, int *num_factors,unsigned long long factors[]) {
    int i, count = 0;

    if ( n == 1llu) {
        factors[count++] = n;
        *num_factors = remove_duplicates(factors, count) ;
        return 0;
    }
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

/*
    function accepts the prime and count to the next prime.
    prints the factors of each number in the range.
*/
int adjacent_prime_gap(const unsigned long long start,const int count) {
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
            if (num == 1) {
                printf(",SPECIAL\n");
            } else if ( num_factors == 1 && factors[0] == num ){
               printf(",PRIME\n");
           }else {
               printf(",COMPOSITE\n");
            }

            if(merge(mainlist, &mainlist_size, factors, num_factors) != 0){
                fprintf(stderr, "Error: MAINLIST is full.\n");
                return EXIT_FAILURE;
            }
        }
    }
    print_array(mainlist, mainlist_size, "#Gap Primes");
    return 0;
}
#ifndef FIND_FACTOR_LIB
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
                break;

            case 'c': // Handle -c or --count
                //count = atoi(optarg); // Convert argument to integer

                unsigned long value = strtoul(optarg, &endptr, 10);
                if (value >= UINT_MAX || value == 0llu){
                //if (count <= 0) {
                    fprintf(stderr, "Invalid count: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                count = value;
                fprintf(stderr, "count: %s,%u\n", optarg,count);
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
    if ( count == 0 ||number == 0llu ){
       fprintf(stderr, "Must specify both count and number >0.\nUsage: %s [-n|--number <unsigned long long>] [-c|--count <int>]\n", argv[0]);
       return EXIT_FAILURE;
    }
    adjacent_prime_gap(number,count);
    return EXIT_SUCCESS;
}
#endif
