
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>

// Structure to hold program options
typedef struct {
    char *input_name;
    char *output_name;
    bool prime_gap;
    bool factors;
    bool next;
} ProgramOptions;

// Function prototypes
void print_usage(const char *program_name);
bool validate_input_file(const char *filename);
char *generate_output_filename(const char *input_name);

int main(int argc, char *argv[]) {
    ProgramOptions options = {
        .input_name = NULL,
        .output_name = NULL,
        .prime_gap = false,
        .factors = false,
        .next = false
    };

    static struct option long_options[] = {
        {"input_name", required_argument, 0, 'i'},
        {"output_name", required_argument, 0, 'o'},
        {"prime_gap", no_argument, 0, 'p'},
        {"factors", no_argument, 0, 'f'},
        {"next", no_argument, 0, 'n'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int opt;

    // Parse command line options
    while ((opt = getopt_long(argc, argv, "i:o:pfn", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i':
                options.input_name = optarg;
                break;
            case 'o':
                options.output_name = optarg;
                break;
            case 'p':
                options.prime_gap = true;
                break;
            case 'f':
                options.factors = true;
                break;
            case 'n':
                options.next = true;
                break;
            case '?':
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Validate input file parameter
    if (options.input_name == NULL) {
        fprintf(stderr, "Error: Input file is required\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Validate input file exists and has .bin extension
    if (!validate_input_file(options.input_name)) {
        fprintf(stderr, "Error: Invalid input file '%s'. File must exist and have .bin extension\n", 
                options.input_name);
        return EXIT_FAILURE;
    }

    // Generate output filename if not provided
    if (options.output_name == NULL) {
        options.output_name = generate_output_filename(options.input_name);
        if (options.output_name == NULL) {
            fprintf(stderr, "Error: Failed to generate output filename\n");
            return EXIT_FAILURE;
        }
    }

    // Open input file in read-only binary mode
    FILE *input_file = fopen(options.input_name, "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
        free(options.output_name);
        return EXIT_FAILURE;
    }

    // Open output file
    FILE *output_file = fopen(options.output_name, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", strerror(errno));
        fclose(input_file);
        free(options.output_name);
        return EXIT_FAILURE;
    }

    // Print configuration (for debugging)
    printf("Configuration:\n");
    printf("Input file: %s\n", options.input_name);
    printf("Output file: %s\n", options.output_name);
    printf("Prime gap enabled: %s\n", options.prime_gap ? "yes" : "no");
    printf("Factors validation enabled: %s\n", options.factors ? "yes" : "no");
    printf("Next value debugging enabled: %s\n", options.next ? "yes" : "no");

    // Clean up
    fclose(input_file);
    fclose(output_file);
    if (options.output_name != options.input_name) { // Only free if we allocated it
        free(options.output_name);
    }

    return EXIT_SUCCESS;
}

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i, --input_name FILE    Input binary file (required, must be .bin)\n");
    fprintf(stderr, "  -o, --output_name FILE   Output CSV file (optional, defaults to input_name.csv)\n");
    fprintf(stderr, "  -p, --prime_gap          Calculate prime gaps\n");
    fprintf(stderr, "  -f, --factors            Validate prime factors\n");
    fprintf(stderr, "  -n, --next               Enable next value debugging\n");
}

bool validate_input_file(const char *filename) {
    if (filename == NULL) return false;
    
    // Check file exists
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return false;
    fclose(file);

    // Check extension
    const char *ext = strrchr(filename, '.');
    if (ext == NULL) return false;
    return strcmp(ext, ".bin") == 0;
}

char *generate_output_filename(const char *input_name) {
    char *output_name = strdup(input_name);
    if (output_name == NULL) return NULL;

    char *ext = strrchr(output_name, '.');
    if (ext != NULL) {
        strcpy(ext, ".csv");
    } else {
        // If no extension, append .csv
        char *new_name = realloc(output_name, strlen(output_name) + 5); // +5 for .csv\0
        if (new_name == NULL) {
            free(output_name);
            return NULL;
        }
        output_name = new_name;
        strcat(output_name, ".csv");
    }

    return output_name;
}
