#include <stddef.h>
#include <stdio.h>

#ifndef __WINDOW_SIEVE__
#define __WINDOW_SIEVE__
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long long ulonglong;
typedef struct Prime {
    ulonglong p;
    ulonglong nextval;
} Prime;
#define PRINTF timestamp_printf
void timestamp_printf(const char *format, ...) ;
#define primesbin "primes.bin"
#define primescsv "primes.csv"

size_t prime_bin2csv(char *inputname ,char * outputname,int verbose_flag,int fast_flag,int next_flag,int check_flag,int pgap_flag) ;
int prime_unread(FILE * fp) ;
FILE * prime_open(char * filename) ;
FILE * csv_creat(char * filename) ;
size_t prime_read(FILE * fp, Prime *p) ;
#endif
