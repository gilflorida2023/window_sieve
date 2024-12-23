CC = gcc 
RELEASE = -O3 -s -Wall -I include
DEBUG = -g3 -O0 -Wall -Werror
CFLAGS = $(RELEASE)

SRCDIR = src
SOURCES = $(SRCDIR)/window_sieve.c $(SRCDIR)/hardware_info.c $(SRCDIR)/trial_division.c

# Default install directory
INSTALL_DIR = /usr/local/bin

all: window_sieve

window_sieve: $(SOURCES)
	$(CC) $(CFLAGS) -DWINDOW_SIEVE_MAIN -o $@ $^

run: window_sieve
	time ./window_sieve -f -v -c -w 10000000000 -u 100000000000; tail primes.csv

clean:
	rm -f window_sieve *.o primes.*

install: window_sieve
	mkdir -p $(INSTALL_DIR)
	cp window_sieve $(INSTALL_DIR)

.PHONY: all run clean configure install

build: window_sieve
	$(MAKE) window_sieve

