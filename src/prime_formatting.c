#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <prime_formatting.h>
char nl_buffer[NL_BUFFER_SIZE];
FILE * TS_LOG = NULL;
/*
  function: char* numeric_literal(unsigned long long n, char* buffer, size_t buffer_size) {
    Converts inout number into a string as shown:
        Original: 1000000, Formatted: 1_000_000
        Original: 1234567890, Formatted: 1_234_567_890
        Original: 123456, Formatted: 123_456
        Original: 1000, Formatted: 1_000
        Original: 1, Formatted: 1
  RETURNS:
    char* pointer to buffer containing formatted string.

*/
char* numeric_literal(unsigned long long n, char* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size < 2) {
        return NULL;  // Invalid input
    }
#define BUFFER_SIZE 128
    char temp[BUFFER_SIZE];
    int len, i, j, count;

    // Convert number to string
    snprintf(temp, sizeof(temp), "%llu", n);
    len = strlen(temp);

    // Start from the end of the number
    i = len - 1;
    j = buffer_size - 1;
    count = 0;

    // Null-terminate the buffer
    buffer[j] = '\0';
    j--;

    while (i >= 0 && j >= 0) {
        buffer[j] = temp[i];
        count++;
        i--;
        j--;

        // Add underscore every 3 digits, but not at the beginning
        if (count % 3 == 0 && i >= 0 && j >= 0) {
            buffer[j] = '_';
            j--;
        }
    }

    // If we ran out of buffer space, return NULL
    if (i >= 0) {
        return NULL;
    }

    // Return pointer to the start of the formatted number in the buffer
    return &buffer[j + 1];
}

/**
 * @brief Prints a formatted message with a timestamp to stdout and optionally to a log file.
 *
 * This function generates a timestamp with microsecond precision in ISO 8601 format
 * and prepends it to the formatted message. The output is written to stdout and
 * optionally to a log file if TS_LOG is defined.
 *
 * @param format A printf-style format string.
 * @param ... Variable arguments to be formatted according to the format string.
 *
 * @note The timestamp format is: YYYY-MM-DDTHH:MM:SS.uuuuuu+HH:MM
 *       where uuuuuu represents microseconds and +HH:MM is the timezone offset.
 *
 * @note This function uses the global variable TS_LOG, which should be a FILE pointer
 *       to an open log file. If TS_LOG is NULL, no log file output occurs.
 *
 * @warning This function is not thread-safe due to the use of localtime().
 *
 * @warning The function assumes that the system's timezone is correctly set for
 *          accurate timezone offset calculation.
 *
 * Example usage:
 *     FILE* logfile = fopen("app.log", "a");
 *     TS_LOG = logfile;
 *     timestamp_printf("Program started with %d arguments\n", argc);
 *     // Output to stdout and app.log: 2025-01-12T17:05:23.123456-06:00: Program started with 3 arguments
 *     fclose(logfile);
 */
void timestamp_printf(const char *format, ...) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get the current time with microsecond precision
    time_t now = tv.tv_sec;  // Get seconds
    struct tm *tm = localtime(&now); // Convert to local time

    char timestamp[50];
    
    strftime(timestamp, sizeof(timestamp), "%FT%T", tm);
    
    snprintf(timestamp + strlen(timestamp), sizeof(timestamp) - strlen(timestamp), ".%06ld%+03ld:00", tv.tv_usec, tm->tm_gmtoff / 3600);

    fprintf(stdout,"%s: ", timestamp);
    if (TS_LOG !=NULL) {
        fprintf(TS_LOG,"%s: ", timestamp);
    }
   va_list args;
   va_list args_copy;
   va_start(args, format);
   va_copy(args_copy, args);

   vfprintf(stdout, format, args);

   if (TS_LOG != NULL) {
       vfprintf(TS_LOG, format, args_copy);
       fflush(TS_LOG);
   }

   va_end(args_copy);
   va_end(args);
}

/**
 * @brief Converts a byte value to a human-readable string with appropriate unit.
 *
 * This function takes a byte value and converts it to a human-readable string
 * with the most appropriate unit (B, KB, MB, GB, etc.). The result is rounded
 * to two decimal places.
 *
 * @param bytes The number of bytes to format (unsigned long long).
 * @return A pointer to a static char array containing the formatted string.
 *         The returned string is statically allocated and should not be freed.
 *         Subsequent calls to this function will overwrite the previous result.
 *
 * @note This function is not thread-safe due to the use of a static buffer.
 *
 * @warning The returned pointer is valid only until the next call to this function.
 *
 * Example usage:
 *     unsigned long long bytes = 1500000;
 *     char* result = format_bytes_to_human_readable(bytes);
 *     printf("Formatted size: %s\n", result);
 *     // Output: Formatted size: 1.50 MB
 */
char* format_bytes_to_human_readable(unsigned long long bytes) {
        static char buffer[100];
    const char* units[] = {"B", //bytes
                           "KB", //kilo byte
                           "MB", //mega byte
                           "GB", // giga byte
                           "TB", //tera byte
                           "PB", // peta byte
                           "EB", // hexa byte
                           "ZB", // zetta byte
                           "YB", // yotta byte
                           "BB"}; // bronto byte
    int i = 0;
    double size = bytes;

    while (size >= 1000 && i < 9) {
        size /= 1000;
        i++;
    }

    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[i]);
    return buffer;
}

#ifdef PRIME_FORMATTING_MAIN
int main() {
    unsigned long long numbers[] = {1000000, 
                                 1234567890, 
                                  234567890, 
                                   34567890, 
                                    4567890, 
                                     567890, 
                                      67890, 
                                       7890, 
                                        890, 
                                         90, 
                                          0,
                                          1};
    int count = sizeof(numbers) / sizeof(numbers[0]);

    for (int i = 0; i < count; i++) {
        char *formatted = NUMERIC_LITERAL(numbers[i]);
        printf("Original: %llu, Formatted: %s\n", numbers[i], formatted);
    }

    return 0;
}
#endif
