CC = gcc
RELEASE = -O3 -s -Wall -I include
DEBUG = -g3 -O0 -Wall -Werror -I include
CFLAGS = $(RELEASE)

SRCDIR = src
INCDIR = include

SIEVE_SOURCES = $(SRCDIR)/window_sieve.c $(SRCDIR)/hardware_info.c $(SRCDIR)/trial_division.c
SIEVE_HEADERS = $(INCDIR)/window_sieve.h $(INCDIR)/hardware_info.h $(INCDIR)/trial_division.h

FFCL_SOURCES = $(SRCDIR)/ffcl.c
FFCL_HEADERS = 

# Default install directory
#INSTALL_DIR = /usr/local/bin
INSTALL_DIR = ~/projects/bin

TARGETS = window_sieve ffcl

# Targets for each binary
window_sieve: $(SIEVE_SOURCES) $(SIEVE_HEADERS)
	$(CC) $(CFLAGS) -DWINDOW_SIEVE_MAIN -o $@ $^

ffcl: $(FFCL_SOURCES) $(FFCL_HEADERS)
	$(CC) $(CFLAGS) -DFFCL_MAIN -o $@ $^

# Aggregate target for all binaries
all: $(TARGETS)

run_sieve: window_sieve
	time ./window_sieve -f -v -w 100000 -u 1000000; tail primes.csv

run_ffcl: ffcl
	./ffcl -n 2 -c 100

clean:
	rm -f $(TARGETS) primes.*

install: all # Install both binaries
	mkdir -p $(INSTALL_DIR)
	cp window_sieve $(INSTALL_DIR)
	cp ffcl $(INSTALL_DIR)

.PHONY: all run_sieve run_ffcl clean configure install
