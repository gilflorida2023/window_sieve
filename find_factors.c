#include <stdio.h>
#include <math.h>
#define MAX_FACTORS 100
int factors[MAX_FACTORS];
int remove_duplicates(int arr[], int n) {
    if (n == 0 || n == 1)
        return n;

    int j = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] != arr[j]) {
            j++;
            arr[j] = arr[i];
        }
    }

    return j + 1;
}

int find_factors(int n, int *num_factors) {
    int i, count = 0;

    // Check divisibility by 2
    if (n % 2 == 0) {
        factors[count++] = 2;
        while (n % 2 == 0) {
            n /= 2;
        }
    }

    // Check divisibility by odd numbers up to the square root of n
    for (i = 3; i * i <= n; i += 2) {
        while (n % i == 0) {
            if (count >= MAX_FACTORS) {
                printf("Error: Too many factors.\n");
                return -1;
            }
            factors[count++] = i;
            n /= i;
        }
    }

    // If n is greater than 2, it's a prime factor
    if (n > 2) {
        if (count >= MAX_FACTORS) {
            printf("Error: Too many factors.\n");
            return -1;
        }
        factors[count++] = n;
    }

    //*num_factors = count;
    *num_factors = remove_duplicates(factors, count) ;
    return 0;
}

int main() {
    int num;
    const int start = 2;
    const int end = 1000000;

    for(int num=start;num <= end; num++) {
        int  num_factors;
        if (find_factors(num, &num_factors) == 0) {
            printf("%d,{", num);
            int first=1;
            for (int i = 0; i < num_factors; i++) {
                if (!first) {
                    printf(" ");
                }
                printf("%d", factors[i]);
                first = 0;
            }
            printf("}");
            if (num == 1) {
                printf(",SPECIAL\n");
            } else if ( num_factors == 1 && factors[0] == num ){
               printf(",PRIME\n");
           }else {
               printf(",COMPOSITE\n");
            }
        }
    }
    return 0;
}
