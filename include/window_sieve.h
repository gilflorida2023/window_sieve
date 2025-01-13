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
#define primesbin "primes.bin"
#define primescsv "primes.csv"
#define true 1
#define false 0
#define map2buffer(val) ((unsigned int)((val) - current_window))
#define DEFAULT_WINDOW_SIZE 100000
#define DEFAULT_UPPER_LIMIT 1000000

size_t prime_bin2csv(char *inputname ,char * outputname,int verbose_flag,int fast_flag,int next_flag,int check_flag,int pgap_flag) ;
int prime_unread(FILE * fp) ;
FILE * prime_open(char * filename) ;
FILE * csv_creat(char * filename) ;
size_t prime_read(FILE * fp, Prime *p) ;

extern size_t window_size;
extern ulonglong upper_limit;
extern int fast_flag ;
extern int verbose_flag ;
extern int check_flag ;
extern int pgap_flag ;
extern int next_flag ;
#endif
