/**
 * Implements the Sieve of Eratosthenes algorithm to find prime numbers up to a specified limit.
 *
 * This function uses a segmented sieve approach to identify prime numbers and writes them
 * along with their next multiple to a binary file. It processes the numbers in windows of
 * size 'buffer_size' up to 'upper_limit'.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <sys/resource.h>
#include <trial_division.h>
#include <hardware_info.h>
#include <prime_file.h>
#include <prime_formatting.h>
#include <stdlib.h>
#include <window_sieve.h>

/**
 * @brief Implements the Sieve of Eratosthenes algorithm to find prime numbers up to a specified limit.
 *
 * This function uses a segmented sieve approach to identify prime numbers and writes them
 * along with their next multiple to a binary file. It processes the numbers in windows of
 * size 'buffer_size' up to 'upper_limit'.
 *
 * @param buffer_size The size of each segment (window) to process at a time.
 * @param upper_limit The upper bound for prime number generation.
 *
 * @note This function writes prime numbers to a binary file specified by the global variable 'primesbin'.
 * @note It uses a segmented approach to handle large ranges efficiently.
 * @note The function includes overflow checks when compiled with GCC.
 *
 * Global variables used:
 * - verbose_flag: If set, prints detailed progress information.
 * - fast_flag: If not set, introduces delays during processing for rate limiting.
 * - next_flag: Set to 1 if the function terminates early due to overflow.
 * - primesbin: The name of the binary file to store prime numbers.
 *
 * @warning This function may terminate early if arithmetic overflow is detected.
 * @warning Ensure sufficient memory is available for the buffer allocation.
 *
 * Dependencies:
 * - prime_open(): Function to open the binary prime file.
 * - prime_read(), prime_write(), prime_unread(): Functions for binary file I/O.
 * - format_bytes_to_human_readable(): Function to format byte sizes.
 * - PRINTF: Macro or function for verbose output.
 * - NUMERIC_LITERAL: Macro for numeric literal representation.
 *
 * Example usage:
 *     size_t buffer_size = 1000000;
 *     unsigned long long upper_limit = 1000000000ULL;
 *     sieve(buffer_size, upper_limit);
 */
void sieve(const size_t buffer_size, const ulonglong upper_limit) {
    Prime cp; //current prime 
    ulonglong current_window = 0ULL;
    unsigned long long result=0ULL;
    char * message = "Shut 'er down, Clancy, she's pumping mud!";
    uchar * is_prime = (uchar *)malloc(buffer_size * sizeof(uchar));
    if (verbose_flag) {
        PRINTF("MALLOCED SIZE: %ld %s \n",buffer_size*sizeof(uchar), format_bytes_to_human_readable((unsigned long long)buffer_size*sizeof(uchar)));
    }
    if (is_prime == NULL) {
    // Handle allocation failure
        perror("Memory allocation failed ");
        exit(EXIT_FAILURE);
    }
    FILE * fp = prime_open(primesbin);
    for(;current_window<upper_limit;current_window+=buffer_size) {
        if (verbose_flag) { 
            PRINTF("current_window: %llu, %s, %s\n",current_window,format_bytes_to_human_readable(current_window),NUMERIC_LITERAL(current_window));
        }
        memset(is_prime, true, buffer_size * sizeof(uchar));
        // read each prime from primes.bin
        // mark prime's composites which occurs in input file
        // update prime's nextval in primes.bin before proceeding
        while (prime_read(fp,&cp) == 1) {
            uchar entered_loop = false;
            for (;cp.nextval<current_window+buffer_size;cp.nextval +=cp.p){
                entered_loop = true;
                uint val ;
#ifdef __GNUC__
                if (__builtin_usubll_overflow(cp.nextval, current_window, &result)) {
                    PRINTF("%s\nSubtraction overflow detected! -- perform recovery\n",message);
                    free(is_prime);
                    fclose(fp);
                    next_flag = 1; // write next val to the csv
                    // work we have done will be dumped to csv.
                    return;
                }
                val = (int)result;
#else      
                val= map2buffer(cp.nextval);
#endif
                if (!fast_flag && cp.nextval%1000000 == 0){
                    usleep(150000); 
                }
                is_prime[val]=false;
            }
            if (entered_loop) {
                prime_unread(fp);
                prime_write(fp, &cp) ;
            }
        }

        // discover new primes
        // skip 0 and 1, since by definition, they are not prime.
        for (cp.p = (current_window == 0 ) ? 2 : current_window; cp.p < current_window + buffer_size; cp.p ++) {
            //uint val = map2buffer(cp.p);
            uint val ;
#ifdef __GNUC__
            if (__builtin_usubll_overflow(cp.p, current_window, &result)) {
                PRINTF("%s\nSubtraction overflow detected! -- perform recovery\n",message);
                free(is_prime);
                fclose(fp);
                next_flag = 1; // write next val to the csv
                // work we have done will be dumped to csv.
                return;
            }
            val = (int)result;
#else      
            val= map2buffer(cp.p);
#endif
            //OverflowCheck(val)
            if (is_prime[val]) {
                cp.nextval = cp.p + cp.p;
                // Mark multiples of p as not prime
                while (cp.nextval < current_window + buffer_size) {
                    uint val ;
#ifdef __GNUC__
                    if (__builtin_usubll_overflow(cp.nextval, current_window, &result)) {
                        PRINTF("%s\nSubtraction overflow detected! -- perform recovery\n",message);
                        free(is_prime);
                        fclose(fp);
                        next_flag = 1; // write next val to the csv
                        // work we have done will be dumped to csv.
                        return;
                }
                val = (int)result;
#else      
                val= map2buffer(cp.nextval);
#endif
                    if (!fast_flag && cp.nextval%100000 == 0){
                        usleep(150000); 
                    }
                    is_prime[val] = false ;
                    cp.nextval += cp.p;
                }
                prime_write(fp, &cp) ;
            }
        }
        rewind(fp);
    }
    fflush(fp); // Flush the output file.
    fclose(fp); // close primes.bin
    free(is_prime);
}

// Global variables for command line options
 size_t window_size = DEFAULT_WINDOW_SIZE;
 ulonglong upper_limit = DEFAULT_UPPER_LIMIT;
 int fast_flag =0;
 int verbose_flag =0;
 int check_flag =0;
 int pgap_flag =0;
 int next_flag =0;

/**
 * @brief Removes the binary and CSV prime files.
 *
 * This function attempts to delete two files: 'primes.bin' and 'primes.csv'.
 * If the verbose flag is set, it prints messages indicating successful deletions.
 *
 * @details
 * - Attempts to remove 'primes.bin' using the remove() function.
 * - If successful and verbose mode is on, prints a success message for 'primes.bin'.
 * - Then attempts to remove 'primes.csv'.
 * - If successful and verbose mode is on, prints a success message for 'primes.csv'.
 *
 * @note
 * - The function assumes global variables 'primesbin' and 'primescsv' contain the file paths.
 * - It also assumes a global 'verbose_flag' to control output verbosity.
 * - The function does not handle or report errors if file removal fails.
 *
 * @return void
 */
void files_remove(void) {
    int result = remove(primesbin);
    if (result == 0) {
        if (verbose_flag) { 
            PRINTF("primes.bin deleted successfully\n");
        }
    }
    result = remove(primescsv);
    if (result == 0) {
        if (verbose_flag) { 
            PRINTF("primes.csv deleted successfully\n");
        }
    } 
}

/**
 * @brief Prints the usage information for the program.
 *
 * This function displays a formatted help message that includes the program name
 * and all available command-line options with their descriptions.
 *
 * @param program_name The name of the program as invoked from the command line.
 *
 * @details
 * The function prints the following information:
 * - Usage syntax
 * - Available options with their short and long forms
 * - Brief descriptions of each option
 * - Default values for window size and upper limit
 *
 * Options explained:
 * - window_size: Sets the size of the window for the sieve algorithm
 * - upper_limit: Sets the maximum number to check for primality
 * - verbose: Enables detailed output during execution
 * - check: Activates prime number validation using trial division
 * - fast: Disables periodic yielding of processor to the system
 * - next: Includes the next value in the output
 * - pgap: Calculates and includes prime gaps between adjacent primes
 * - help: Displays this usage information
 *
 * @note
 * - Uses printf for output, which writes to stdout.
 * - Assumes DEFAULT_WINDOW_SIZE and DEFAULT_UPPER_LIMIT are defined constants.
 */
void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -w, --window_size <size>   Set window size (default: %d)\n", DEFAULT_WINDOW_SIZE);
    printf("  -u, --upper_limit <limit>  Set upper limit (default: %d)\n", DEFAULT_UPPER_LIMIT);
    printf("  -v, --verbose              Enable verbose output\n");
    printf("  -c, --check                Check validity of prime with trial division.\n");
    printf("  -f, --fast                 Dont periodically yield processor to system.\n");
    printf("  -n, --next                 Include next value.\n");
    printf("  -p, --pgap                 calculate prime gap between adjacent primes.\n");
    printf("  -h, --help                 Display this help message\n");
}

/**
 * @brief Main function for the window sieve prime number generator.
 *
 * This function initializes the program, processes command-line arguments,
 * sets up logging, adjusts process priority, and executes the prime number
 * generation and processing workflow.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Returns EXIT_SUCCESS on successful execution, EXIT_FAILURE otherwise.
 *
 * @details
 * The function performs the following main tasks:
 * 1. Opens a log file for writing.
 * 2. Sets the process priority to slightly lower than normal.
 * 3. Processes command-line options using getopt_long.
 * 4. Validates and sets the window size and upper limit for prime generation.
 * 5. Handles various flags for program behavior (verbose, check, fast, next, pgap).
 * 6. Prints hardware info if in verbose mode.
 * 7. Removes any existing output files.
 * 8. Executes the sieve algorithm for prime number generation.
 * 9. Converts the binary prime file to CSV format with optional processing.
 * 10. Closes the log file and exits.
 *
 * @note
 * - Uses global variables for flags and file handles (e.g., TS_LOG, verbose_flag).
 * - Assumes the existence of several helper functions (e.g., hardware_info, files_remove).
 * - Error handling is implemented for file operations and invalid inputs.
 */
int main(int argc, char *argv[]) {
    int c;
    int option_index = 0;

    TS_LOG = fopen("window_sieve.log", "a");
    if (TS_LOG == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }else {
        PRINTF("================================\n");
        PRINTF("Created log file\n");
    }
    // Set the priority to 1 , ower than normal. normally starts at 0.
    // -20 (highest priority) to 19 (lowest priority)    
    if (setpriority(PRIO_PROCESS, 0, 1) == -1) {
        perror("setpriority");
        exit(EXIT_FAILURE);
    }
    PRINTF("Set nice priority to -1\n");

    
    static struct option long_options[] = {
        {"window_size", required_argument, 0, 'w'},
        {"upper_limit", required_argument, 0, 'u'},
        {"verbose",     no_argument,       &verbose_flag, 1},
        {"check",       no_argument,       &check_flag, 1},
        {"fast",        no_argument,       &fast_flag, 1},
        {"next",        no_argument,       &next_flag, 1},
        {"pgap",        no_argument,       &pgap_flag, 1},
        {"help",        no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    char *endptr;

    while ((c = getopt_long(argc, argv, "w:u:vcfnph", long_options, &option_index)) != -1) {
        switch (c) {
            case 'w':
             errno = 0;  // Reset errno before the call
            
                unsigned long long value = strtoull(optarg, &endptr, 10);
            
                // Check for conversion errors
                if (*endptr != '\0') {
                    printf("Invalid input. Please enter a valid positive integer.\n");
                    return 1;
                }
            
                if (value > SIZE_MAX) {
                    fprintf(stderr,"Value too large for size_t\n");
                    return EXIT_FAILURE;
                }
            
                window_size = (size_t)value;
                if (window_size <5) {
                    fprintf(stderr, "Error: Window size must greater than five.\n");
                    return EXIT_FAILURE;
                } 
                break;

            case 'u':
                upper_limit = strtoull(optarg, &endptr, 10);
                if (*endptr != '\0') {
                    fprintf(stderr,"Invalid input: not a number\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'c':
                check_flag = 1;
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case 'f':
                fast_flag = 1;
                break;
            case 'p':
                pgap_flag = 1;
                break;
            case 'n':
                next_flag = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    if (verbose_flag == true) {
        hardware_info();
    }
    files_remove();
    PRINTF("Window size: %u, %s, %s\n", window_size, format_bytes_to_human_readable((unsigned long long)window_size), NUMERIC_LITERAL((unsigned long long)window_size));
    PRINTF("Upper limit: %llu, %s, %s\n", upper_limit,format_bytes_to_human_readable(upper_limit), NUMERIC_LITERAL(upper_limit));
    if (window_size > upper_limit){
        fprintf(stderr,"Invalid input: window_size must be less than or equal to upper limit.\n");
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }
    sieve(window_size, upper_limit);
    size_t count = prime_bin2csv(primesbin, primescsv, verbose_flag, fast_flag, next_flag, check_flag, pgap_flag) ;
    PRINTF("converted %u primes\n", count);
    fflush(TS_LOG);
    fclose(TS_LOG);
    return EXIT_SUCCESS;
}