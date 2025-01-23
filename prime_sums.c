#include <stdio.h>
#include <string.h>

typedef struct {
    int p1;
    int p2;
    int middle;
    int p3;
    int p4;
} PRIME_QUADRUPLET;

const PRIME_QUADRUPLET prime_quadruplets[]= {
    {   5,    7,    9,  11,  13},
    {  11,   13,   15,  17,  19},
    { 101,  103,  105, 107,  109},
    { 191,  193,  195, 197,  199},
    { 821,  823,  825, 827,  829},
    {1481, 1483, 1485, 1487, 1489},
    {1871, 1873, 1875, 1877, 1879},
    {2081, 2083, 2085, 2087, 2089}
};

const int primes[] = {
                2, 3, 5, 7,
                11, 13, 17, 19,
                23, 29, 31, 37,
                41, 43, 47, 53,
                59, 61, 67, 71,
                73, 79, 83, 89,
                -1
            };

const int degrees_increment = 360 / 24;                
const char * titles[] = {
    "#sequence", "prime_sum", "degrees"
};

int main (){
    printf("%s, %s, %s\n", titles[0], titles[1],titles[2]);
    int prime_sum = 0;
    int prime_index = 0;
    int degrees = 0;
    int sequence = 0;
    while(primes[prime_index] >0) {
        prime_sum += primes[prime_index++];
        degrees += degrees_increment;
        printf("%*d, %*d, %*d\n", (int)strlen(titles[0]),++sequence, (int)strlen(titles[1]),prime_sum, (int)strlen(titles[2]),degrees);
    }
}
