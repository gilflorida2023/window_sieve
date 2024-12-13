bin_PROGRAMS = window_sieve
window_sieve_SOURCES = window_sieve.c

clean-local:
	@rm -f primes.* window_sieve window_sieve.o
	

window_sieve_CFLAGS = -Wunused -O0 -g3

run: window_sieve
	time ./window_sieve -f -v ; tail primes.csv

.PHONY: run
