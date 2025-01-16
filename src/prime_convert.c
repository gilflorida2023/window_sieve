#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <trial_division.h>
#include <prime_formatting.h>
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



#ifdef PRIME_CONVERT_MAIN

#define MAX_FILENAME_LENGTH 256

struct ProgramOptions {
    char input_name[MAX_FILENAME_LENGTH];
    char output_name[MAX_FILENAME_LENGTH];
    int prime_gap;
    int factors;
    int next;
    int verbose;
};

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -i, --input_name FILENAME   Input binary file name\n");
    printf("  -o, --output_name FILENAME  Output CSV file name (default: input_name.csv)\n");
    printf("  -p, --prime_gap             Calculate prime gap\n");
    printf("  -f, --factors               Validate primes by factorization\n");
    printf("  -n, --next                  Write next value for debugging\n");
    printf("  -v, --verbose               verbose output\n");
    printf("  -h, --help                  Display this help message\n");
}

int main(int argc, char *argv[]) {
    struct ProgramOptions options = {0};
    int c;
    int option_index = 0;

    static struct option long_options[] = {
        {"input_name", required_argument, 0, 'i'},
        {"output_name", required_argument, 0, 'o'},
        {"prime_gap", no_argument, 0, 'p'},
        {"factors", no_argument, 0, 'f'},
        {"next", no_argument, 0, 'n'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "i:o:pfnvh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                strncpy(options.input_name, optarg, MAX_FILENAME_LENGTH - 1);
                options.input_name[MAX_FILENAME_LENGTH - 1] = '\0';
                break;
            case 'o':
                strncpy(options.output_name, optarg, MAX_FILENAME_LENGTH - 1);
                options.output_name[MAX_FILENAME_LENGTH - 1] = '\0';
                break;
            case 'p':
                options.prime_gap = 1;
                break;
            case 'f':
                options.factors = 1;
                break;
            case 'n':
                options.next = 1;
                break;
            case 'v':
                options.verbose = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case '?':
                fprintf(stderr, "Unknown option or missing argument.\n");
                print_usage(argv[0]);
                exit(1);
            default:
                abort();
        }
    }

    // Check if input file is provided and has .bin extension
    if (options.input_name[0] == '\0' || strstr(options.input_name, ".bin") == NULL) {
        fprintf(stderr, "Error: Input file must be provided with .bin extension.\n");
        print_usage(argv[0]);
        exit(1);
    }
    #define CSV_EXTENSION ".csv"
    if (options.output_name[0] == '\0') {
        char *dot = strrchr(options.input_name, '.');
        if (dot != NULL) {
            size_t name_length = dot - options.input_name;
            strncpy(options.output_name, options.input_name, name_length);
            options.output_name[name_length] = '\0';
        } else {
            strncpy(options.output_name, options.input_name, MAX_FILENAME_LENGTH - 1);
            options.output_name[MAX_FILENAME_LENGTH - 1] = '\0';
        }
        strncat(options.output_name, CSV_EXTENSION, MAX_FILENAME_LENGTH - strlen(options.output_name) - 1);
    }

    if (options.verbose) {
        TS_LOG = fopen("window_sieve.log", "a");
        if (TS_LOG == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        } else {
            PRINTF("================================\n");
            PRINTF("Created log file\n");
        }
    }
    size_t records_processed = prime_bin2csv(
    options.input_name, 
    options.output_name, 
    options.verbose, 
    0, 
    options.next, 
    options.factors, 
    options.prime_gap) ;
    PRINTF("%ul primes written\n", records_processed);
    if (options.verbose){
        fflush(TS_LOG);
        fclose(TS_LOG);
    }
    return EXIT_SUCCESS;    

    //return 0;
}
#endif