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
#include <prime_file.h>
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
