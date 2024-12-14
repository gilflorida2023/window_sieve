#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <window_sieve.h>
#include <trial_division.h>

unsigned long long isqrt(unsigned long long n) {
    unsigned long long root = 0;
    unsigned long long bit = 1ULL << 31; // Start with the highest power of 4 <= n

    while (bit > n)
        bit >>= 2;

    while (bit != 0) {
        if (n >= root + bit) {
            n -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return root;
}

ulonglong find_factors_and_check_prime(unsigned long long n) {
    if (n < 2) {
        return 0;
    }

    unsigned long long i;
    int is_prime = 1; // Assume n is prime until we find a factor

    // Check for factors from 2 to sqrt(n)
    for (i = 2; i <= isqrt(n); i++) {
        if (n % i == 0) { // i is a factor
            return i;
            if (i != 1 && i != n) {
                is_prime = 0; // Found a factor other than 1 and n
            }
            if (i != n / i) { // Check the corresponding factor
                return n/i;
                if (n / i != 1 && n / i != n) {
                    is_prime = 0; // Found a factor other than 1 and n
                }
            }
        }
    }
    // Determine if the number is prime
    return (is_prime) ?  n: 0ull;
}

char * check_prime(unsigned long long n) {
    char * return_code=NULL;
        ulonglong factor = find_factors_and_check_prime(n) ;
        if (n == 0 || n == 1){return_code = td_special;}
        else return_code = (factor==n&& n!=0)?td_prime:td_composite;
    return return_code;
}
#ifdef SEQUENTIAL
int main(void) {
    for (ulonglong i = 0llu;i<1000000llu;i++) {
        char * cp = check_prime(i) ;
        printf("%llu,%s\n",i, cp);
    }   
    return 0;
}
#endif
#ifdef TD_TEST
int main(void) {
    size_t count = prime_bin2csv(primesbin,primescsv,0,0,0); 
    printf("%lu records written to csv %s from binary file %s\n",count,primescsv,primesbin)  ;
    return 0;
}
#endif
