CC = gcc
RELEASE = -O3 -s -Wall -I . 
#-Werror 
DEBUG = -g3 -O0 -Wall -Werror
CFLAGS = $(RELEASE)

all: window_sieve

window_sieve: window_sieve.c hardware_info.c
	$(CC) $(CFLAGS) -o $@ $^

run: window_sieve
	time ./window_sieve -f -v ; tail primes.csv

clean:
	rm -f window_sieve window_sieve.o primes.*

.PHONY: all run clean configure

build: window_sieve
	$(MAKE) window_sieve
