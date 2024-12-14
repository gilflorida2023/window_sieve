#ifndef __WINDOW_SIEVE__
#define __WINDOW_SIEVE__
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long long ulonglong;
typedef struct Prime {
    ulonglong p;
    ulonglong nextval;
} Prime;
#define primesbin "primes.bin"
#define primescsv "primes.csv"
size_t prime_bin2csv(char *inputname ,char * outputname,uchar verbose_flag,uchar fast_flag,uchar check_flag) ;
FILE * prime_open(char * filename) ;
FILE * csv_creat(char * filename) ;
size_t prime_read(FILE * fp, Prime *p) ;
#endif
