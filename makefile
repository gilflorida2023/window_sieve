CC = gcc
CFLAGS = -ftrapv -Wunused -O0 -g3

all: window_sieve

sieve: window_sieve.c
	$(CC) $(CFLAGS) -o $@ $<

run: window_sieve
	time ./window_sieve

clean:
	rm -f window_sieve

.PHONY: all run clean
