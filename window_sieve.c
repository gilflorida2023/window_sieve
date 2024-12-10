/*
performs a prime sieve which will utilize no data structure except
a binary file and an array of enum bools as the current window. Visits a
range of natural numbers utilizing a sliding window. it accumulates primes in the bin file.
usually upperlimit is 1000000, so it should find 78498 primes.
Currently utilizes unsigned int. 
TODO: need to switch to unsigned long long.
uint max val:                            4,294,967,295
unsigned long long max val: 18,446,744,073,709,551,615

TODO: commamdline options, -w 100000  --window_size 100000, -u 1000000  --upper_limit 1000000, 
                           -f filename   --filename filename,-v --verbose

TODO: option to start from an existing file of primes. option to validate a file 
and detemine its range.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
typedef unsigned int uint;
typedef unsigned long long ulonglong;
#define primescsv "primes.csv"
#define primesbin "primes.bin"
#define map2buffer(val) ((val) - current_window)

#if 0
#define OverflowCheck(val) if ((val) >buffer_size) { perror("arithemetic overflow 1"); \
               fflush(fp); fclose(fp); free(is_prime); uint count = prime_bin2csv(primesbin, \
               primescsv); printf("records written %u\n",count); exit(EXIT_FAILURE); }
#endif
typedef struct Prime {
    uint p;
    uint nextval;
} Prime;

enum Bool {
    false = 0,
    true = 1
};

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
size_t prime_bin2csv(char *inputname ,char * outputname) {
    FILE * input, * output;
    size_t count=0;
    Prime p;
    input = prime_open(inputname);
    output = csv_creat(outputname) ;

    // for each record of input
    while (prime_read(input,&p)==1) {
        count ++;
        fprintf(output, "%u,%u\n", p.p, p.nextval);
        if (count%10000 == 0){
            usleep(250000); 
        }
    }
    fclose(input);
    fflush(output);
    fclose(output);
    return count;
}
/*
    Ssieve identifies prime numbers and write them to a file. 
    Current change: move to binary file and the update the write the csv file at the end.
*/
void sieve(const uint buffer_size, const uint upper_limit) {
    Prime cp; //current prime 
    uint current_window = 0;
    enum Bool *is_prime = (enum Bool *)malloc(buffer_size * sizeof(enum Bool));
    if (is_prime == NULL) {
    // Handle allocation failure
        perror("Memory allocation failed ");
        exit(EXIT_FAILURE);
    }

    FILE * fp = prime_open(primesbin);
    for(;current_window<upper_limit;current_window+=buffer_size) {
        //usleep(500000); // Sleeps for 0.5 seconds
        fprintf(stderr, "current_window: %u\n",current_window);
        memset(is_prime, true, buffer_size * sizeof(enum Bool));
        // read each prime from primes.bin
        // mark prime's composites which occurs in input file
        // update prime's nextval in primes.bin before proceeding
        while (prime_read(fp,&cp) == 1) {
            for (;cp.nextval<current_window+buffer_size;cp.nextval +=cp.p){
                uint val = map2buffer(cp.nextval);
                //OverflowCheck(val)
                if (cp.nextval%1000000 == 0){
                    usleep(150000); 
                }
                is_prime[val]=false;
            }
            prime_unread(fp);
            prime_write(fp, &cp) ;
        }
        // discover new primes
        // skip 0 and 1, since by definition, they are not prime.
        for (cp.p = (current_window == 0 ) ? 2 : current_window; cp.p < current_window + buffer_size; cp.p ++) {
            uint val = map2buffer(cp.p);
            //OverflowCheck(val)
            if (is_prime[val]) {
                cp.nextval = cp.p + cp.p;
    
                // Mark multiples of p as not prime
                while (cp.nextval < current_window + buffer_size) {
                    uint val = map2buffer(cp.nextval);
                    //OverflowCheck(val)
                    if (cp.nextval%100000 == 0){
                        usleep(150000); 
                    }
                    is_prime[val] = false ;
                    cp.nextval += cp.p;
                }
                prime_write(fp, &cp) ;
            }
        }
        rewind(fp);
        //usleep(500000); // Sleeps for 0.5 seconds
    }
    fflush(fp); // Flush the output file.
    fclose(fp); // close primes.bin
    free(is_prime);
}

void files_remove(void) {
    int result = remove(primesbin);
    if (result == 0) {
        printf("primes.bin deleted successfully\n");
    }
    result = remove(primescsv);
    if (result == 0) {
        printf("primes.csv deleted successfully\n");
    } 
}

// Default values for command line options
#define DEFAULT_WINDOW_SIZE 100000
#define DEFAULT_UPPER_LIMIT 1000000

// Global variables for command line options
static uint window_size = DEFAULT_WINDOW_SIZE;
static uint upper_limit = DEFAULT_UPPER_LIMIT;
static int verbose_flag = 0;

void print_usage(const char *program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -w, --window_size <size>   Set window size (default: %d)\n", DEFAULT_WINDOW_SIZE);
    printf("  -u, --upper_limit <limit>  Set upper limit (default: %d)\n", DEFAULT_UPPER_LIMIT);
    printf("  -v, --verbose             Enable verbose output\n");
    printf("  -h, --help                Display this help message\n");
}

int main(int argc, char *argv[]) {
    int c;
    int option_index = 0;
    
    static struct option long_options[] = {
        {"window_size", required_argument, 0, 'w'},
        {"upper_limit", required_argument, 0, 'u'},
        {"verbose",     no_argument,       &verbose_flag, 1},
        {"help",        no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "w:u:vh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'w':
                window_size = atoi(optarg);
                if (window_size <= 0) {
                    fprintf(stderr, "Error: Window size must be positive\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'u':
                upper_limit = atoi(optarg);
                if (upper_limit <= 0) {
                    fprintf(stderr, "Error: Upper limit must be positive\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
                // getopt_long already printed an error message
                return EXIT_FAILURE;
            default:
                break;
        }
    }

    if (verbose_flag) {
        printf("Window size: %u\n", window_size);
        printf("Upper limit: %u\n", upper_limit);
    }

    files_remove();
    sieve(window_size, upper_limit);
    uint count = prime_bin2csv(primesbin, primescsv);
    printf("Found %u primes\n", count);
    
    return EXIT_SUCCESS;
}
