bin_PROGRAMS = window_sieve
window_sieve_SOURCES = window_sieve.c

clean-local:
	@rm -f config.status configure config.log
	@rm -f Makefile
	@rm -f -r autom4te.cache/
	@rm -f alocal.m4
	@rm -f compile install-sh missing Makefile.in
	@rm -f primes.* window-sieve window-sieve.o
	

window_sieve_CFLAGS = -Wunused -O0 -g3

run: window_sieve
	time ./window_sieve -f -v ; tail primes.csv

.PHONY: run

.PHONY: clean
clean:
	rm -f window_sieve *.o

