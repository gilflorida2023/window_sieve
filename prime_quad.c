#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdbool.h> // For bool type

typedef struct {
    uint64_t p;
    uint64_t next;
} Prime;

int prime_unread(FILE *fp) {
    int rc = fseek(fp, -sizeof(Prime), SEEK_CUR);
    if (rc == -1) {
        perror("fseek encountered error");
        return -1;
    }
    return 0; // Return 0 on success, -1 on error
}

int main() {
    FILE *fp = fopen("primes.bin", "rb");
    if (fp == NULL) {
        perror("Error opening primes.bin");
        return 1;
    }

    Prime primes[4];
    int state = 0;
    bool quad_found = false; // Use bool for clarity
    int primes_read = 0;

    while (fread(&primes[0], sizeof(Prime), 1, fp) == 1) {
        if (prime_unread(fp) != 0) {
            fclose(fp);
            return 1;
        }

        state = 0;
        quad_found = false;
        primes_read = 0;

        for (int i = 0; i < 4; i++) {
            if (fread(&primes[i], sizeof(Prime), 1, fp) != 1) {
                if (feof(fp)) break; // End of file is OK
                perror("Error reading primes");
                fclose(fp);
                return 1;
            }
            primes_read++;

            switch (state) {
                case 0: if (primes[i].p % 10 == 1) state++; break;
                case 1: if (primes[i].p % 10 == 3) state++; break;
                case 2: if (primes[i].p % 10 == 7) state++; break;
                case 3: if (primes[i].p % 10 == 9) {
                    state++;
                    quad_found = true;
                } break;
            }
        }

        if (quad_found) {
            printf("Quad found: ");
            for (int i = 0; i < 4; i++) {
                printf("%" PRIu64 " ", primes[i].p);
            }
            printf("\n");
        } 
        else {
#if 0       
            printf("Not a quad: ");
            for (int i = 0; i < primes_read; i++) { // Simplified condition
                printf("%" PRIu64 " ", primes[i].p);
            }
            printf("\n");
#endif
            for (int i = primes_read - 1; i > 0; i--) {
                if (prime_unread(fp) != 0) {
                    fclose(fp);
                    return 1;
                }
            }
        }
    }

    fclose(fp);
    return 0;
}
