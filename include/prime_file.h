#include <window_sieve.h>
#include <string.h>
#include <stdio.h>
#ifndef __PRIME_FILE__
#define __PRIME_FILE__
FILE * prime_open(char * filename);
size_t prime_read(FILE * fp, Prime *p);
size_t prime_write(FILE *fp, Prime * p);
int prime_unread(FILE * fp);
FILE * csv_creat(char * filename) ;
#endif