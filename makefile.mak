CC = gcc
RELEASE = -O3 -s -Wall -Werror 
DEBUG = -g3 -O0 -Wall -Werror
CFLAGS = $(RELEASE)

all: window_sieve

window_sieve: window_sieve.c
	$(CC) $(CFLAGS) -o $@ $<

run: window_sieve
	time ./window_sieve -f -v ; cat primes.csv

clean:
	rm -f window_sieve primes.*

.PHONY: all run clean configure

build: window_sieve
	$(MAKE) window_sieve
