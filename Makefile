CC = gcc 
RELEASE = -O3 -s -Wall -I include
DEBUG = -g3 -O0 -Wall -Werror -I include
CFLAGS = $(RELEASE)

SRCDIR = src
INCDIR = include
SIEVE_SOURCES = $(SRCDIR)/window_sieve.c $(SRCDIR)/hardware_info.c $(SRCDIR)/trial_division.c
SIEVE_HEADERS = $(INCDIR)/window_sieve.h $(INCDIR)/hardware_info.h $(INCDIR)/trial_division.h

# Default install directory
INSTALL_DIR = /usr/local/bin

all: window_sieve

window_sieve: $(SIEVE_SOURCES) $(SIEVE_HEADERS)
	$(CC) $(CFLAGS) -DWINDOW_SIEVE_MAIN -o $@ $^

run: window_sieve
	time ./window_sieve -f -v -w 100000 -u 1000000; tail primes.csv

clean:
	rm -f window_sieve *.o primes.*

install: window_sieve
	mkdir -p $(INSTALL_DIR)
	cp window_sieve $(INSTALL_DIR)

.PHONY: all run clean configure install

build: window_sieve
	$(MAKE) window_sieve

