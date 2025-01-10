/*
performs a prime sieve which will utilize no data structure except
a binary file and an array of enum bools as the current window. Visits a
range of natural numbers utilizing a sliding window. it accumulates primes in the bin file.
usually upperlimit is 1000000, so it should find 78498 primes.
Currently utilizes unsigned long long. 

~/projects/clang/window_sieve$ ./window_sieve -h
Usage: ./window_sieve [options]
Options:
  -w, --window_size <size>   Set window size (default: 100000)
  -u, --upper_limit <limit>  Set upper limit (default: 1000000)
  -v, --verbose             Enable verbose output
  -h, --help                Display this help message

TODO: option to start from an existing file of primes. option to validate a file 
and detemine its range.
*/

#define _GNU_SOURCE
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/sysinfo.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <window_sieve.h>
#include <hardware_info.h>
#include <trial_division.h>

#define PRINTF timestamp_printf
#define map2buffer(val) ((unsigned int)((val) - current_window))

#define true 1
#define false 0

// Default values for command line options
#define DEFAULT_WINDOW_SIZE 100000
#define DEFAULT_UPPER_LIMIT 1000000

// Global variables for command line options
static size_t window_size = DEFAULT_WINDOW_SIZE;
static ulonglong upper_limit = DEFAULT_UPPER_LIMIT;
static int fast_flag = 0;
static int verbose_flag = 0;
static int check_flag = 0;
static int pgap_flag = 0;
static int next_flag = 0;

FILE * TS_LOG = NULL;
/*
  Accepts same parameters as printf, but it puts a date time in front of the message.
  its utilized with the following preprocessor macro.replacing PRINTF with timestamp_printf
  #define PRINTF timestamp_printf
*/
void timestamp_printf(const char *format, ...) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get the current time with microsecond precision
    time_t now = tv.tv_sec;  // Get seconds
    struct tm *tm = localtime(&now); // Convert to local time

    // Buffer to hold the formatted timestamp
    char timestamp[50];
    
    // Format the timestamp with subseconds
    strftime(timestamp, sizeof(timestamp), "%FT%T", tm);
    
    // Append subseconds and timezone offset
    snprintf(timestamp + strlen(timestamp), sizeof(timestamp) - strlen(timestamp), ".%06ld%+03ld:00", tv.tv_usec, tm->tm_gmtoff / 3600);

    //printf("%s: ", timestamp);
    fprintf(stdout,"%s: ", timestamp);
    if (TS_LOG !=NULL) {
        fprintf(TS_LOG,"%s: ", timestamp);
    }
/*
    va_list args;
    va_start(args, format);
    //vprintf(format, args);
    vfprintf(stdout,format,args);
    if (TS_LOG !=NULL) {
        vfprintf(TS_LOG,format,args);
    }

    va_end(args);
    */
   va_list args;
va_list args_copy;
va_start(args, format);
va_copy(args_copy, args);

vfprintf(stdout, format, args);

if (TS_LOG != NULL) {
    vfprintf(TS_LOG, format, args_copy);
}

va_end(args_copy);
va_end(args);
}

/*
    open the specified bin file for read and write. does not truncate the file. 
*/
FILE * prime_open(char * filename) {
    FILE * fp = fopen(filename, "rb+"); //open existing
    if (fp == NULL) {
        fp = fopen(filename, "wb+"); //create if necessary
        if (fp == NULL) {
            perror("Error opening binary file");
            exit(EXIT_FAILURE);
        }
    }
    return fp;
}

/*
   Read Prime struct from input fp.
   parameters:
   FILE *fp - file pointer
   Prime * p - prime struct
   returns bytes read 
*/
size_t prime_read(FILE * fp, Prime *p) {
    return fread(p, sizeof(Prime), 1, fp);
}

/*
   writes the prime struct to the binaryfile referenced by fp.
   parameters:
   FILE *fp - file pointer
   Prime * p - prime struct
   returns bytes read 
*/
size_t prime_write(FILE *fp, Prime * p) {
    // Write the modified integer back to the file
    size_t s = fwrite(p, sizeof(Prime), 1, fp);
    if ( s == 0 ) {
        perror("Error writing binary file");
        exit(EXIT_FAILURE);
    }
    return s;
}

/*
  As each prime is read from the input binary file, on the second
  pass its next value must be updated for the next buffer. 
  Must read Prime
  Prime p;
  while ((prime_read(fp, &p) ==1) {
    for (;map2buffer[p->nextval]<current_window;p->nextval +=p->p){is_prime[map2buffer]=false}
    prime_unread(fp);
    prime_write(fp, &p) ;
  }  
*/
int prime_unread(FILE * fp) {
    int rc = fseek(fp,-sizeof(Prime),SEEK_CUR);
    if ( rc == -1 ) {
        perror("fseek encountered error");
        exit(EXIT_FAILURE);
    }
    return rc;
}

/*
    create the csv file, trunc if it exists.
*/
FILE * csv_creat(char * filename) {
    FILE *fp = fopen(filename,"w");
    // Check if the file was opened successfully
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

/*
    convert the prime bin file into a csv and return the number of records processed.
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

/*
    convert the prime bin file into a csv and return the number of records processed.
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

/*
    Sieve identifies prime numbers and writes the prime along with its next value  to a binary file. 
    Current change: move to binary file and the update the write the csv file at the end.
*/
void sieve(const size_t buffer_size, const ulonglong upper_limit) {
    Prime cp; //current prime 
    ulonglong current_window = 0ULL;
    unsigned long long result=0ULL;
    char * message = "Shut 'er down, Clancy, she's pumping mud!";
    uchar * is_prime = (uchar *)malloc(buffer_size * sizeof(uchar));
    if (verbose_flag) {
        PRINTF("MALLOCED SIZE: %ld\n",buffer_size*sizeof(uchar));
    }
    if (is_prime == NULL) {
    // Handle allocation failure
        perror("Memory allocation failed ");
        exit(EXIT_FAILURE);
    }
    FILE * fp = prime_open(primesbin);
    for(;current_window<upper_limit;current_window+=buffer_size) {
        if (verbose_flag) { 
            PRINTF("current_window: %llu\n",current_window);
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
    PRINTF("Window size: %u, %s\n", window_size, format_bytes((unsigned long long)window_size));
    //PRINTF("Window size: %ld\n", window_size);
    PRINTF("Upper limit: %llu\n", upper_limit);
    sieve(window_size, upper_limit);
    size_t count = prime_bin2csv(primesbin, primescsv, verbose_flag, fast_flag, next_flag, check_flag, pgap_flag) ;
    PRINTF("converted %u primes\n", count);
    fflush(TS_LOG);
    fclose(TS_LOG);
    return EXIT_SUCCESS;
}
#endif
