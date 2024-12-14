CC = gcc 
RELEASE = -O3 -s -Wall -I include  # Add -I include to specify the include directory
#-Werror 
DEBUG = -g3 -O0 -Wall -Werror
CFLAGS = $(RELEASE)

# Specify source files in the src directory
SRCDIR = src
SOURCES = $(SRCDIR)/window_sieve.c $(SRCDIR)/hardware_info.c $(SRCDIR)/trial_division.c

all: window_sieve

window_sieve: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

run: window_sieve
	time ./window_sieve -f -v ; tail primes.csv

clean:
	rm -f window_sieve window_sieve.o hardware_info.o primes.*

.PHONY: all run clean configure

build: window_sieve
	$(MAKE) window_sieve

