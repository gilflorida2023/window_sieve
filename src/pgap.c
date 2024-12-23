#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/sysinfo.h>
#include <window_sieve.h>
#include <hardware_info.h>
#include <trial_division.h>
#include <time.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include <math.h>
double wave_function(ulonglong n) {
    double N = (double)n;
    double k = (N * M_PI) / N;
    double w = N * M_PI * N;
    return N * sin(k - w);
}
/*
    convert the prime bin file into a csv and return the number of records processed.
*/
size_t prime_bin2csv(char *inputname ,char * outputname,uchar verbose_flag,uchar fast_flag,uchar next_flag,uchar check_flag,uchar pgap_flag,uchar wave_flag) {
    FILE * input, * output;
    size_t count=0;
    Prime p;
    input = prime_open(inputname);
    output = csv_creat(outputname) ;
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
        if (wave_flag){
            double wave = wave_function(p.p) ;
            fprintf(output,",%lf", wave);
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

int main(int argc, char *argv[]) {
    //size_t prime_bin2csv(char *inputname ,char * outputname,uchar verbose_flag,uchar fast_flag,uchar next_flag,uchar check_flag,uchar pgap_flag,uchar wave_flag) {
    uint count = prime_bin2csv(primesbin, primescsv,1,1,0,0,1,0);
    PRINTF("Converted %u primes.\n", count);
    
    return EXIT_SUCCESS;
}
