#!/usr/bin/env bash

rm -f *.o ffcl pgap
#gcc  -Iinclude -o pgap src/*.c -lm
gcc  -o ffcl ffcl.c -lm
