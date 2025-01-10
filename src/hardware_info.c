
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

#define LINE_BUFFER_SIZE 666
typedef struct loadaverage{
  double one;
  double five;
  double fifteen;
} loadaverage;

void print_cpu_info(FILE *cpufile);
int get_ram_info(void);
loadaverage read_load_average(const char *filename);

int hardware_info() {
    FILE * cpufile;
    
    // Open /proc/cpuinfo
    if ((cpufile = fopen("/proc/cpuinfo", "r")) == NULL) {
        perror("Error opening /proc/cpuinfo");
        return 1;
    }

    printf("HARDWARE INFO\n");
    print_cpu_info(cpufile);
    fclose(cpufile);

    // Sysinfo (RAM)
    if (get_ram_info() != 0) {
        return 1;
    }

    // Load Average
     loadaverage la = read_load_average("/proc/loadavg");

    printf("Load Average: 1-minute: %.2f, 5-minute: %.2f, 15-minute: %.2f\n", la.one, la.five, la.fifteen);

    return 0;
}

void print_cpu_info(FILE *cpufile) {
    char line[LINE_BUFFER_SIZE];
    int core_count = 0;

    while (fgets(line, LINE_BUFFER_SIZE, cpufile)) {
        if (strstr(line, "model name") != NULL ) {
            if (core_count ==0 ) {printf("Model Name: %s", strchr(line, ':') + 2);}
            core_count++;
        } 
        if (strstr(line, "Model") != NULL ) {
            printf("%s", line);
        } 
    }

    // If "cpu cores" is not found, assume single core for each model name line
    printf("Number of Cores: %d\n", core_count);
}


char* format_bytes(unsigned long long bytes) {
    static char buffer[100];
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double size = bytes;

    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }

    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[i]);
    return buffer;
}



int get_ram_info() {
    struct sysinfo info;

    if (sysinfo(&info) == 0) {
        unsigned long long totalram = info.totalram;
        unsigned long long freeram = info.freeram;
        unsigned long long usedram = totalram - freeram;

        printf("Total RAM: %s\n", format_bytes(totalram));
        printf("Free RAM: %s\n", format_bytes(freeram));
        printf("Used RAM: %s\n", format_bytes(usedram));
    } else {
        perror("sysinfo");
        return 1;
    }

    return 0;
}
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
