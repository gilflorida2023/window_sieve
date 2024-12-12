bin_PROGRAMS = window_sieve
window_sieve_SOURCES = window_sieve.c

clean-local:
	@rm config.status configure config.log
	@rm Makefile
	@rm -r autom4te.cache/
	@rm alocal.m4
	@rm compile install-sh missing Makefile.in
	

window_sieve_CFLAGS = -ftrapv -Wunused -O0 -g3

run: window_sieve
	time ./window_sieve -f -v ; cat primes.csv

.PHONY: run
