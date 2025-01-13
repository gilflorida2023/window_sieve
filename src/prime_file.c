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

/**
 * @brief Opens a binary file for reading and writing prime numbers.
 *
 * This function attempts to open an existing file in read-write binary mode.
 * If the file doesn't exist, it creates a new file in read-write binary mode.
 *
 * @param filename A null-terminated string containing the name of the file to open.
 *
 * @return FILE* A pointer to the opened file stream.
 *
 * @note The function first tries to open the file in "rb+" mode (read and write, binary).
 *       If that fails, it attempts to create the file in "wb+" mode (create for reading and writing, binary).
 *
 * @warning This function will terminate the program if it fails to open or create the file.
 *          It uses perror() to print an error message before exiting.
 *
 * Error handling:
 * - If the file cannot be opened or created, the function prints an error message to stderr
 *   and terminates the program with EXIT_FAILURE.
 *
 * Example usage:
 *     char *filename = "primes.bin";
 *     FILE *prime_file = prime_open(filename);
 *     if (prime_file != NULL) {
 *         // Use the file...
 *         fclose(prime_file);
 *     }
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

/**
 * @brief Reads a single Prime structure from a binary file.
 *
 * This function reads one Prime structure from the current position in the specified file stream.
 * It's designed to work with files opened in binary read mode.
 *
 * @param fp Pointer to the FILE stream from which to read the Prime structure.
 *           This stream should be opened in binary read mode.
 * @param p Pointer to a Prime structure where the read data will be stored.
 *
 * @return size_t The number of Prime structures successfully read (1 if successful, 0 if end-of-file or error).
 *
 * @note The function uses fread() to perform the binary read operation.
 * @note The size of the read operation is determined by sizeof(Prime).
 * @note Unlike some other functions in this library, prime_read does not exit on failure.
 *       It's up to the caller to check the return value and handle any errors.
 *
 * Error handling:
 * - If fread() returns 0, it could mean either end-of-file was reached or an error occurred.
 *   The caller should use feof() and ferror() to distinguish between these cases if needed.
 *
 * Assumptions:
 * - The Prime structure is assumed to be properly defined and accessible.
 * - The file pointer fp is assumed to be valid and opened in the appropriate mode.
 *
 * Example usage:
 *     FILE *fp = fopen("primes.bin", "rb");
 *     Prime prime;
 *     if (fp != NULL) {
 *         while (prime_read(fp, &prime) == 1) {
 *             printf("Prime: %llu, Next Value: %llu\n", prime.p, prime.nextval);
 *         }
 *         fclose(fp);
 *     }
 */
size_t prime_read(FILE * fp, Prime *p) {
    return fread(p, sizeof(Prime), 1, fp);
}

/**
 * @brief Writes a Prime structure to a binary file.
 *
 * This function writes a single Prime structure to the specified file stream
 * in binary format. It's designed to work with files opened in binary mode.
 *
 * @param fp Pointer to the FILE stream where the Prime structure will be written.
 *           This stream should be opened in binary write mode.
 * @param p Pointer to the Prime structure to be written to the file.
 *
 * @return size_t The number of Prime structures successfully written (should be 1 if successful).
 *
 * @note The function uses fwrite() to perform the binary write operation.
 * @note The size of the write operation is determined by sizeof(Prime).
 *
 * @warning This function will terminate the program if the write operation fails.
 *          It uses perror() to print an error message before exiting.
 *
 * Error handling:
 * - If fwrite() returns 0 (indicating a write failure), the function prints an error message
 *   to stderr and terminates the program with EXIT_FAILURE.
 *
 * Assumptions:
 * - The Prime structure is assumed to be properly defined and accessible.
 * - The file pointer fp is assumed to be valid and opened in the appropriate mode.
 *
 * Example usage:
 *     FILE *fp = fopen("primes.bin", "wb");
 *     Prime prime = {.p = 17, .nextval = 34};
 *     if (fp != NULL) {
 *         size_t written = prime_write(fp, &prime);
 *         if (written == 1) {
 *             printf("Prime successfully written.\n");
 *         }
 *         fclose(fp);
 *     }
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

/**
 * @brief Moves the file pointer back by one Prime structure in a binary file.
 *
 * This function is crucial in the prime number generation process, particularly
 * when updating the 'nextval' field of Prime structures. It allows the program
 * to rewind the file pointer to the start of the last read Prime structure,
 * enabling an immediate update of that structure.
 *
 * @param fp Pointer to the FILE stream of the binary prime number file.
 *
 * @return int Returns 0 on success, -1 on failure (though the function will exit on failure).
 *
 * @note This function uses fseek() to move the file pointer backwards by sizeof(Prime) bytes.
 * @note It's typically used in conjunction with prime_read() and prime_write() in a sequence like:
 *       1. Read a Prime structure (prime_read)
 *       2. Update the Prime structure (e.g., modify nextval)
 *       3. Move back to the start of that structure (prime_unread)
 *       4. Write the updated structure back to the file (prime_write)
 *
 * @warning This function will terminate the program if fseek() fails.
 *          It uses perror() to print an error message before exiting.
 *
 * Error handling:
 * - If fseek() returns -1 (indicating failure), the function prints an error message
 *   to stderr and terminates the program with EXIT_FAILURE.
 *
 * Usage context:
 * This function is typically used in a loop that processes prime numbers:
 * Prime p;
 * while (prime_read(fp, &p) == 1) {
 *     // Process and update p (e.g., update p.nextval)
 *     prime_unread(fp);
 *     prime_write(fp, &p);
 * }
 *
 * Example usage:
 *     FILE *fp = fopen("primes.bin", "rb+");
 *     if (fp != NULL) {
 *         Prime p;
 *         if (prime_read(fp, &p) == 1) {
 *             p.nextval += p.p;  // Update nextval
 *             prime_unread(fp);  // Move back to start of this Prime
 *             prime_write(fp, &p);  // Write updated Prime
 *         }
 *         fclose(fp);
 *     }
 */
int prime_unread(FILE * fp) {
    int rc = fseek(fp,-sizeof(Prime),SEEK_CUR);
    if ( rc == -1 ) {
        perror("fseek encountered error");
        exit(EXIT_FAILURE);
    }
    return rc;
}

/**
 * @brief Creates a new CSV file for writing.
 *
 * This function creates a new file with the specified name and opens it in write mode.
 * If a file with the same name already exists, it will be truncated (i.e., its contents will be erased).
 *
 * @param filename A null-terminated string containing the name of the file to be created.
 *
 * @return FILE* A pointer to the newly created file stream.
 *
 * @note The function uses the "w" mode with fopen(), which creates a new empty file for writing.
 *       If the file already exists, its contents are discarded.
 *
 * @warning This function will terminate the program if it fails to create or open the file.
 *          It uses perror() to print an error message before exiting.
 *
 * Error handling:
 * - If the file cannot be created or opened, the function prints an error message to stderr
 *   and terminates the program with EXIT_FAILURE.
 *
 * Example usage:
 *     char *filename = "output.csv";
 *     FILE *csv_file = csv_creat(filename);
 *     if (csv_file != NULL) {
 *         fprintf(csv_file, "Column1,Column2,Column3\n");
 *         fprintf(csv_file, "Data1,Data2,Data3\n");
 *         fclose(csv_file);
 *     }
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
