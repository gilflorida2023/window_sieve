#include <stdio.h>
#ifndef __HARDWARE_INFO__
#define __HARDWARE_INFO__
#define LINE_BUFFER_SIZE 666
void print_cpu_info(FILE *cpufile);
int get_ram_info(void);
typedef struct loadaverage{
  double one;
  double five;
  double fifteen;
} loadaverage;
loadaverage read_load_average(const char *filename);
int hardware_info(void);
char* format_bytes(unsigned long long bytes) ;
#endif
