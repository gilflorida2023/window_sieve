
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <stdbool.h>

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
    return 0;
}

int main() {
    const char *filename = "primes.bin";
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening primes.bin");
        return 1;
    }

    Prime primes[4];
    int state = 0;
    bool quad_found = false;
    int primes_read = 0;
    uint64_t sequence_number = 0; // Initialize sequence number

    while (true) { // Outer loop now controlled by feof
        state = 0;
        quad_found = false;
        primes_read = 0;

        for (int i = 0; i < 4; i++) {
            if (fread(&primes[i], sizeof(Prime), 1, fp) != 1) {
                if (feof(fp)) {
                    goto end_of_file; // Use goto for clean exit at EOF
                }
                perror("Error reading primes");
                fclose(fp);
                return 1;
            }
            sequence_number++; // Increment sequence number for EACH prime
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
                printf("(%" PRIu64 ", %" PRIu64 ") ", sequence_number - 3 + i, primes[i].p); // Print sequence number and prime
            }
            printf("\n");
        } else {

            printf("Not a quad: ");
            for (int i = 0; i < primes_read; i++) {
                printf("(%" PRIu64 ", %" PRIu64 ") ", sequence_number - primes_read + 1 + i, primes[i].p); // Print sequence number and prime
            }
            printf("\n");
            for (int i = primes_read - 1; i > 0; i--) {
                if (prime_unread(fp) != 0) {
                    fclose(fp);
                    return 1;
                }
            }
        }
    }

end_of_file: // Label for goto

    if (ferror(fp)) {
        perror("Error during file reading");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}
