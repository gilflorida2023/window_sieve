#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <window_sieve.h>
#include <prime_formatting.h>
#include <hardware_info.h>

/**
 * @brief Collects and prints various hardware information about the system.
 *
 * This function gathers and displays the following information:
 * 1. CPU details (using print_cpu_info function)
 * 2. RAM information (using get_ram_info function)
 * 3. System load averages (1, 5, and 15 minutes)
 *
 * @return int Returns 0 on success, 1 if an error occurs during execution.
 *
 * @note This function assumes a Linux-like environment with access to:
 *       - /proc/cpuinfo for CPU information
 *       - /proc/loadavg for load average information
 *       - Appropriate system calls or files for RAM information
 *
 * @note The function uses a macro or function called PRINTF for output.
 *       Ensure this is defined or replaced with an appropriate output function.
 *
 * @warning This function may fail if it doesn't have permission to read
 *          the necessary system files or if the system structure differs
 *          from what's expected.
 *
 * Dependencies:
 * - print_cpu_info(FILE *): Function to print CPU information
 * - get_ram_info(): Function to get and print RAM information
 * - read_load_average(const char *): Function to read load average
 * - loadaverage struct: For storing load average information
 *
 * Example usage:
 *     if (hardware_info() == 0) {
 *         printf("Hardware information gathered successfully.\n");
 *     } else {
 *         printf("Error occurred while gathering hardware information.\n");
 *     }
 */
int hardware_info() {
    FILE * cpufile;
    
    // Open /proc/cpuinfo
    if ((cpufile = fopen("/proc/cpuinfo", "r")) == NULL) {
        perror("Error opening /proc/cpuinfo");
        return 1;
    }

    PRINTF("HARDWARE INFO\n");
    print_cpu_info(cpufile);
    fclose(cpufile);

    // Sysinfo (RAM)
    if (get_ram_info() != 0) {
        return 1;
    }

    // Load Average
     loadaverage la = read_load_average("/proc/loadavg");

    PRINTF("Load Average: 1-minute: %.2f, 5-minute: %.2f, 15-minute: %.2f\n", la.one, la.five, la.fifteen);

    return 0;
}

/**
 * @brief Prints CPU and computer model information from a given file.
 *
 * This function reads a file (typically /proc/cpuinfo on Linux systems) line by line,
 * extracts and prints the CPU model, computer model, and estimates the number of CPU cores.
 *
 * @param cpufile A FILE pointer to an open file containing CPU information.
 *                This file should be opened before calling the function and
 *                closed after the function returns.
 *
 * @note The function assumes a specific format for the input file:
 *       - CPU model is identified by the "model name" string
 *       - Computer model is identified by the "Model" string
 *       - The number of cores is estimated by counting "model name" occurrences
 *
 * @note The function uses a macro or function called PRINTF for output.
 *       Ensure this is defined or replaced with an appropriate output function.
 *
 * @warning This function may not accurately count cores on all systems,
 *          especially those with hyperthreading or complex CPU architectures.
 *
 * Example usage:
 *     FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
 *     if (cpuinfo != NULL) {
 *         print_cpu_info(cpuinfo);
 *         fclose(cpuinfo);
 *     }
 */
void print_cpu_info(FILE *cpufile) {
    char line[LINE_BUFFER_SIZE];
    int core_count = 0;

    while (fgets(line, LINE_BUFFER_SIZE, cpufile)) {
        if (strstr(line, "model name") != NULL ) {
            if (core_count ==0 ) {
                PRINTF("CPU Model: %s", strchr(line, ':') + 2);}
                core_count++;
        } 
        if (strstr(line, "Model") != NULL ) {
            PRINTF("Computer Model: %s", strchr(line,':') + 2);
        } 
    }

    // If "cpu cores" is not found, assume single core for each model name line
    PRINTF("Number of Cores: %d\n", core_count);
}

/**
 * @brief Retrieves and prints system RAM information.
 *
 * This function uses the sysinfo system call to gather information about
 * the system's RAM. It then calculates and prints the following:
 * - Total RAM
 * - Free RAM
 * - Used RAM (calculated as Total RAM - Free RAM)
 *
 * All RAM values are formatted into human-readable strings (e.g., "4.5 GB").
 *
 * @return int Returns 0 on success, 1 if an error occurs during execution.
 *
 * @note This function uses the sysinfo() system call, which is specific to
 *       Linux systems. It may not work on other operating systems.
 *
 * @note The function uses a macro or function called PRINTF for output.
 *       Ensure this is defined or replaced with an appropriate output function.
 *
 * @warning This function may fail if it doesn't have permission to call sysinfo().
 *
 * Dependencies:
 * - <sys/sysinfo.h>: For the sysinfo() function and struct sysinfo
 * - format_bytes_to_human_readable(): Function to convert bytes to human-readable format
 *
 * Example usage:
 *     if (get_ram_info() == 0) {
 *         printf("RAM information retrieved successfully.\n");
 *     } else {
 *         printf("Error occurred while retrieving RAM information.\n");
 *     }
 */
int get_ram_info() {
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        unsigned long long totalram = info.totalram;
        unsigned long long freeram = info.freeram;
        unsigned long long usedram = totalram - freeram;

        PRINTF("Total RAM: %s\n", format_bytes_to_human_readable(totalram));
        PRINTF("Free RAM: %s\n", format_bytes_to_human_readable(freeram));
        PRINTF("Used RAM: %s\n", format_bytes_to_human_readable(usedram));
    } else {
        perror("sysinfo");
        return 1;
    }

    return 0;
}

/**
 * @brief Reads system load average from a specified file.
 *
 * This function opens and reads a file (typically /proc/loadavg on Linux systems)
 * to extract the 1-minute, 5-minute, and 15-minute load averages.
 *
 * @param filename The path to the file containing load average information.
 *
 * @return A loadaverage struct containing the following fields:
 *         - one: 1-minute load average
 *         - five: 5-minute load average
 *         - fifteen: 15-minute load average
 *
 * @note The loadaverage struct is defined as:
 *       typedef struct loadaverage {
 *         double one;
 *         double five;
 *         double fifteen;
 *       } loadaverage;
 *
 * @warning This function will terminate the program if:
 *          - The specified file cannot be opened
 *          - The file does not contain three valid floating-point numbers
 *
 * @note Error messages are printed to stderr using perror() before termination.
 *
 * Example usage:
 *     const char *filename = "/proc/loadavg";
 *     loadaverage la = read_load_average(filename);
 *     printf("Load averages: %.2f (1m), %.2f (5m), %.2f (15m)\n",
 *            la.one, la.five, la.fifteen);
 */
loadaverage read_load_average(const char *filename) {
    FILE *file = fopen(filename, "r");
    loadaverage la;

    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    double load[3];
    int nread = fscanf(file, "%lf %lf %lf", &load[0], &load[1], &load[2]);
    la.one = load[0];
    la.five = load[1];
    la.fifteen = load[2];
    fclose(file);

    // Check for successful read
    if (nread != 3) {
        perror("fscanf");
        exit(EXIT_FAILURE);
    }

    return la;
}

#ifdef skip
int main() {
    if (hardware_info() != 0) {
        printf("Failed to retrieve hardware information.\n");
        return 1;
    }
    return 0;
}
#endif
