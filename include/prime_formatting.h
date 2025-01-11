#ifndef __PRIME_FORMATTING__
#define __PRIME_FORMATTING__
#include <stdio.h>
#define NL_BUFFER_SIZE 100
#ifdef MAIN_MODULE
char nl_buffer[NL_BUFFER_SIZE];
#else
extern char nl_buffer[NL_BUFFER_SIZE];
#endif
// NUMERIC_LITERAL(n) is not thread-safe. but this code is single-threaded.
#define NUMERIC_LITERAL(n) (numeric_literal((n), nl_buffer, NL_BUFFER_SIZE))
char* numeric_literal(unsigned long long n, char* buffer, size_t buffer_size);
#endif
