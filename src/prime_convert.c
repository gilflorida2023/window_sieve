#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 256

struct ProgramOptions {
    char input_name[MAX_FILENAME_LENGTH];
    char output_name[MAX_FILENAME_LENGTH];
    int prime_gap;
    int factors;
    int next;
};

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -i, --input_name FILENAME   Input binary file name\n");
    printf("  -o, --output_name FILENAME  Output CSV file name (default: input_name.csv)\n");
    printf("  -p, --prime_gap             Calculate prime gap\n");
    printf("  -f, --factors               Validate primes by factorization\n");
    printf("  -n, --next                  Write next value for debugging\n");
    printf("  -h, --help                  Display this help message\n");
}

int main(int argc, char *argv[]) {
    struct ProgramOptions options = {0};
    int c;
    int option_index = 0;

    static struct option long_options[] = {
        {"input_name", required_argument, 0, 'i'},
        {"output_name", required_argument, 0, 'o'},
        {"prime_gap", no_argument, 0, 'p'},
        {"factors", no_argument, 0, 'f'},
        {"next", no_argument, 0, 'n'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((c = getopt_long(argc, argv, "i:o:pfnh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                strncpy(options.input_name, optarg, MAX_FILENAME_LENGTH - 1);
                options.input_name[MAX_FILENAME_LENGTH - 1] = '\0';
                break;
            case 'o':
                strncpy(options.output_name, optarg, MAX_FILENAME_LENGTH - 1);
                options.output_name[MAX_FILENAME_LENGTH - 1] = '\0';
                break;
            case 'p':
                options.prime_gap = 1;
                break;
            case 'f':
                options.factors = 1;
                break;
            case 'n':
                options.next = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case '?':
                fprintf(stderr, "Unknown option or missing argument.\n");
                print_usage(argv[0]);
                exit(1);
            default:
                abort();
        }
    }

    // Check if input file is provided and has .bin extension
    if (options.input_name[0] == '\0' || strstr(options.input_name, ".bin") == NULL) {
        fprintf(stderr, "Error: Input file must be provided with .bin extension.\n");
        print_usage(argv[0]);
        exit(1);
    }
#define CSV_EXTENSION ".csv"
if (options.output_name[0] == '\0') {
    char *dot = strrchr(options.input_name, '.');
    if (dot != NULL) {
        size_t name_length = dot - options.input_name;
        strncpy(options.output_name, options.input_name, name_length);
        options.output_name[name_length] = '\0';
    } else {
        strncpy(options.output_name, options.input_name, MAX_FILENAME_LENGTH - 1);
        options.output_name[MAX_FILENAME_LENGTH - 1] = '\0';
    }
    strncat(options.output_name, CSV_EXTENSION, MAX_FILENAME_LENGTH - strlen(options.output_name) - 1);
}

    // Print the parsed options (for demonstration purposes)
    printf("Input file: %s\n", options.input_name);
    printf("Output file: %s\n", options.output_name);
    printf("Prime gap: %s\n", options.prime_gap ? "Enabled" : "Disabled");
    printf("Factors: %s\n", options.factors ? "Enabled" : "Disabled");
    printf("Next: %s\n", options.next ? "Enabled" : "Disabled");

    // Your main program logic goes here

    return 0;
}
