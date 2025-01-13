/**
 * Implements the Sieve of Eratosthenes algorithm to find prime numbers up to a specified limit.
 *
 * This function uses a segmented sieve approach to identify prime numbers and writes them
 * along with their next multiple to a binary file. It processes the numbers in windows of
 * size 'buffer_size' up to 'upper_limit'.
 */

#define _GNU_SOURCE
#define MAIN_MODULE
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
 * @brief Writes the header row for a CSV file containing prime number information.
 *
 * This function generates and writes a header row to the specified output file.
 * The header includes columns based on the flags provided, allowing for a 
 * customizable CSV structure.
 *
 * @param output FILE pointer to the output CSV file.
 * @param verbose_flag Unused in this function, but kept for consistency with other functions.
 * @param fast_flag Unused in this function, but kept for consistency with other functions.
 * @param next_flag If non-zero, includes "NEXT_VALUE" column in the header.
 * @param check_flag If non-zero, includes "INT_CATEGORY" column in the header.
 * @param pgap_flag If non-zero, includes "CONSECUTIVE_PRIME_GAP" column in the header.
 *
 * @note The function always includes a "PRIME" column, regardless of flag settings.
 * @note verbose_flag and fast_flag are unused but included for potential future use
 *       or consistency with other function signatures.
 *
 * @warning This function assumes that the output FILE pointer is valid and writable.
 *          No checks are performed on the validity of the output stream.
 *
 * Header structure:
 * - Always includes: #PRIME
 * - If next_flag: ,NEXT_VALUE
 * - If check_flag: ,INT_CATEGORY
 * - If pgap_flag: ,CONSECUTIVE_PRIME_GAP
 *
 * Example usage:
 *     FILE *csv_file = fopen("primes.csv", "w");
 *     if (csv_file) {
 *         prime_bin2csv_header(csv_file, 0, 0, 1, 1, 1);
 *         fclose(csv_file);
 *     }
 */
void prime_bin2csv_header(FILE * output,uchar verbose_flag,uchar fast_flag,uchar next_flag,uchar check_flag,uchar pgap_flag) {
    fprintf(output,"%s", "#PRIME");
    if (next_flag){
        fprintf(output,",%s", "NEXT_VALUE");
    }
    if (check_flag){
        fprintf(output,",%s", "INT_CATEGORY");
    } 
    if (pgap_flag){
        fprintf(output,",%s", "CONSECUTIVE_PRIME_GAP");
    }
    fprintf(output,"\n");
}

/**
 * @brief Converts a binary file of prime numbers to a CSV format with optional additional information.
 *
 * This function reads prime numbers from a binary file and writes them to a CSV file.
 * It can include additional information such as the next prime, primality check results,
 * and prime gaps based on the provided flags.
 *
 * @param inputname The name of the input binary file containing prime numbers.
 * @param outputname The name of the output CSV file to be created.
 * @param verbose_flag If non-zero, prints progress information.
 * @param fast_flag If zero, introduces a delay every 10000 primes processed.
 * @param next_flag If non-zero, includes the next prime value in the CSV.
 * @param check_flag If non-zero, includes primality check results in the CSV.
 * @param pgap_flag If non-zero, includes prime gaps in the CSV.
 *
 * @return size_t The number of prime numbers processed.
 *
 * @note The function uses custom file handling functions like prime_open() and csv_creat().
 * @note The Prime struct is assumed to contain at least 'p' and 'nextval' members.
 * @note The function may introduce delays if fast_flag is not set, potentially for rate limiting.
 *
 * @warning This function will terminate if it cannot open the input or output files.
 *
 * Dependencies:
 * - prime_open(): Function to open the binary prime file.
 * - csv_creat(): Function to create the CSV file.
 * - prime_bin2csv_header(): Function to write the CSV header.
 * - prime_read(): Function to read a prime from the binary file.
 * - check_prime(): Function to perform primality check.
 * - prime_unread(): Function to undo a read operation on the prime file.
 * - PRINTF: Macro or function for verbose output.
 *
 * Example usage:
 *     size_t count = prime_bin2csv("primes.bin", "primes.csv", 1, 0, 1, 1, 1);
 *     printf("Processed %zu prime numbers.\n", count);
 */
size_t prime_bin2csv(char *inputname ,char * outputname,int verbose_flag,int fast_flag,int next_flag,int check_flag,int pgap_flag) {
    FILE * input, * output;
    size_t count=0;
    Prime p;
    input = prime_open(inputname);
    output = csv_creat(outputname) ;
    prime_bin2csv_header(output, verbose_flag, fast_flag, next_flag, check_flag, pgap_flag) ;
    if (verbose_flag) {
        PRINTF("creating %s from %s\n",outputname,inputname);
    }
    // for each record of input
    while (prime_read(input,&p)==1) {
        count ++;
        fprintf(output,"%llu", p.p);
       if (next_flag){
            fprintf(output,",%llu", p.nextval);
        }
        if (check_flag){
            char * primecode = check_prime(p.p);
            fprintf(output,",%s", primecode);
        } 
        if (pgap_flag){
            Prime next;
            if (prime_read(input,&next)==1){
                fprintf(output,",%llu", next.p-p.p);
                prime_unread(input) ;
            } else {
                fprintf(output,",");
            }
        }
        fprintf(output,"\n");
        if (!fast_flag && count%10000 == 0){
            usleep(250000); 
        }
    }
    fclose(input);
    fflush(output);
    fclose(output);
    return count;
}

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

#ifdef WINDOW_SIEVE_MAIN
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
            
                if (errno == ERANGE) {
                    fprintf(stderr,"Overflow occurred\n");
                    return EXIT_FAILURE;
                }
            
                if (*endptr != '\0') {
                    fprintf(stderr,"Invalid input: not a number\n");
                    return EXIT_FAILURE;
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
                if (upper_limit < window_size) {
#if defined(__LP64__)
                    char * winsize_formatstring = "Error: Upper limit %llu should be greater than or equal to window_size %llu.\n";
#else
                    char * winsize_formatstring = "Error: Upper limit %llu should be greater than or equal to window_size %u.\n";
#endif
                    fprintf(stderr, winsize_formatstring,upper_limit,window_size);
                    return EXIT_FAILURE;
                }
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
    sieve(window_size, upper_limit);
    size_t count = prime_bin2csv(primesbin, primescsv, verbose_flag, fast_flag, next_flag, check_flag, pgap_flag) ;
    PRINTF("converted %u primes\n", count);
    fflush(TS_LOG);
    fclose(TS_LOG);
    return EXIT_SUCCESS;
}
#endif
