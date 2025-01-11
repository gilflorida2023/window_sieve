#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <prime_formatting.h>
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



//FILE * TS_LOG = NULL;
/*
  Accepts same parameters as printf, but it puts a date time in front of the message.
  its utilized with the following preprocessor macro.replacing PRINTF with timestamp_printf
  #define PRINTF timestamp_printf
  prime_formatting
*/
void timestamp_printf(const char *format, ...) {
    struct timeval tv;
    gettimeofday(&tv, NULL); // Get the current time with microsecond precision
    time_t now = tv.tv_sec;  // Get seconds
    struct tm *tm = localtime(&now); // Convert to local time

    // Buffer to hold the formatted timestamp
    char timestamp[50];
    
    // Format the timestamp with subseconds
    strftime(timestamp, sizeof(timestamp), "%FT%T", tm);
    
    // Append subseconds and timezone offset
    snprintf(timestamp + strlen(timestamp), sizeof(timestamp) - strlen(timestamp), ".%06ld%+03ld:00", tv.tv_usec, tm->tm_gmtoff / 3600);

    //printf("%s: ", timestamp);
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
