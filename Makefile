CC = gcc
RELEASE = -O3 -s -Wall -Werror -I include
DEBUG   = -g3 -O0 -Wall -Werror -I include
CFLAGS = $(RELEASE)

SRCDIR = src
INCDIR = include
BINDIR = bin

SIEVE_SOURCES = $(SRCDIR)/window_sieve.c $(SRCDIR)/hardware_info.c $(SRCDIR)/trial_division.c $(SRCDIR)/prime_formatting.c $(SRCDIR)/prime_file.c
SIEVE_HEADERS = $(INCDIR)/window_sieve.h $(INCDIR)/hardware_info.h $(INCDIR)/trial_division.h $(INCDIR)/prime_formatting.h $(INCDIR)/prime_file.h

PRIME_GAP_ANALYZER_SOURCES = $(SRCDIR)/prime_gap_analyzer.c
PRIME_GAP_ANALYZER_HEADERS = 

# Default install directory
ifeq ($(shell id -u),0)
INSTALL_DIR = /usr/local/bin
else
INSTALL_DIR = ~/projects/bin
endif
TARGETS = $(BINDIR)/window_sieve $(BINDIR)/prime_gap_analyzer

all: $(TARGETS)

# Ensure bin directory exists
$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/window_sieve: $(SIEVE_SOURCES) $(SIEVE_HEADERS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

test_window_sieve: $(BINDIR)/window_sieve
	$(BINDIR)/window_sieve -f -v -w 100000 -u 1000000 -p

$(BINDIR)/prime_gap_analyzer: $(PRIME_GAP_ANALYZER_SOURCES) $(PRIME_GAP_ANALYZER_HEADERS)  | $(BINDIR)
	$(CC) $(CFLAGS) -DPRIME_GAP_ANALYZER_MAIN -o $@ $^

test_prime_gap_analyzer: $(BINDIR)/prime_gap_analyzer
	$(BINDIR)/prime_gap_analyzer  -n 492113 -c 114

clean:
	rm -f $(TARGETS) primes.bin primes.csv window_sieve.log
	rm -rf $(BINDIR)/

install: all # Install both binaries
	mkdir -p $(INSTALL_DIR)
	cp $(BINDIR)/window_sieve $(INSTALL_DIR)
	cp $(BINDIR)/prime_gap_analyzer $(INSTALL_DIR)

.PHONY: all test_window_sieve test_prime_gap_analyzer clean configure install
