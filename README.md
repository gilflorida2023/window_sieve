
[![CMake on a single platform](https://github.com/gilflorida2023/window_sieve/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/gilflorida2023/window_sieve/actions/workflows/cmake-single-platform.yml)
# window_sieve

## Overview
Window Sieve is a memory-efficient implementation of the Sieve of Eratosthenes algorithm for finding prime numbers. Unlike traditional implementations that require memory proportional to the upper limit, this program uses a sliding window approach to find primes up to large numbers while maintaining a constant memory footprint.

## How It Works

### Core Algorithm
1. **Sliding Window Approach**
   - Instead of allocating memory for the entire range (0 to upper_limit), the program works with fixed-size windows
   - Default window size is 100,000 numbers
   - Each window represents a consecutive range of numbers in the full sequence

2. **Two-Phase Processing**
   - **Phase 1 - Marking Composites**: For each known prime, mark its multiples as composite within the current window
   - **Phase 2 - Prime Discovery**: Scan the window to find new primes and store them for future windows

### Data Structures
1. **Binary File (primes.bin)**
   - Persistent storage for discovered primes. the next val points past the current buffer into the next.
   - Each record contains:
     - `p`: The prime number
     - `nextval`: Next multiple of this prime to be marked as composite. multiples are calculated by adding p+p.

2. **Window Buffer**
   - Boolean array representing primality of numbers in current window
   - Size is fixed regardless of the upper limit
   - Reused for each window segment

### Implementation Details

#### Window Processing
```
For each window:
1. Clear the is_prime[] buffer (set all to true)
2. Read each prime (p) from primes.bin
3. Mark composites: For each prime p
   - Start from p.nextval
   - Mark all its multiples p+p as composite within current window
   - Update p.nextval for next window
4. Discover new primes in current window
   - For each unmarked number n
   - Confirm n is prime
   - Add n to primes.bin
   - Mark multiples of n as composite
5. Move to next window
```

#### File Handling
- Uses binary file for efficient storage and retrieval
- Maintains prime numbers and their next composite values
- Converts binary data to CSV format after completion

## Features

### Command Line Options
- `-w, --window_size <size>`: Set window size (default: 100,000)
- `-u, --upper_limit <limit>`: Set upper limit (default: 1,000,000)
- `-v, --verbose`: Enable verbose output
- `-c, --check`: check the primes in the binary file and make sure they are prime.
- `-f, --fast`: Disable processing delays, to increases cpu.
- `-h, --help`: Display help message

### Output Files
1. **primes.bin**: Binary file storing prime numbers and their next composite values
2. **primes.csv**: Human-readable CSV format of discovered primes

## Performance
- Memory Usage: O(window_size) regardless of upper_limit
- Time Complexity: O(n log log n) where n is upper_limit
- For default parameters (upper_limit = 1,000,000):
  - Finds 78,498 prime numbers
  - Uses constant ~100KB memory regardless of input size

## Implementation Notes
- Uses unsigned long long for number representation
- Implements efficient file I/O with buffering
- Includes optional processing delays for lowering cpu
- Error handling for file operations and memory allocation

# Compilation
## Cmake
### Release build
	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release
 	make
  	time ./window_sieve -f -v
   	tail primes.csv
### Debug build
	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug
 	make
  	time ./window_sieve -f -v
   	tail primes.csv
### Install


### EXECUTION
	time window_sieve -f -v -w 10 -u 100;tail primes.csv
	created primes.csv and primes.bin
	Window size: 10
	Upper limit: 100
	primes.bin deleted successfully
	primes.csv deleted successfully
	current_window: 0
	current_window: 10
	current_window: 20
	current_window: 30
	current_window: 40
	current_window: 50
	current_window: 60
	current_window: 70
	current_window: 80
	current_window: 90
	Found 25 primes
	
	real	0m0.004s
	user	0m0.001s
	sys	0m0.003s
	53,106
	59,118
	61,122
	67,134
	71,142
	73,146
	79,158
	83,166
	89,178
	97,194
	
	
## EXECUTIONS
# Execution 1 10 buffers totalling 100.
	$ time ./window_sieve -f -v -w 10 -u 100
	HARDWARE INFO
	Model Name: Intel(R) Core(TM) i7-8650U CPU @ 1.90GHz
	Number of Cores: 8
	Total RAM: 31608 GB
	Free RAM: 25765 GB
	Used RAM: 5843 GB
	Load Average: 1-minute: 0.50, 5-minute: 0.75, 15-minute: 0.97
	primes.bin deleted successfully
	primes.csv deleted successfully
	Window size: 10
	Upper limit: 100
	current_window: 0
	current_window: 10
	current_window: 20
	current_window: 30
	current_window: 40
	current_window: 50
	current_window: 60
	current_window: 70
	current_window: 80
	current_window: 90
	creating primes.csv from primes.bin
	Found 25 primes
	
	real    0m0.010s
	user    0m0.007s
	sys     0m0.003s
# EXECUTION 2 2 buffers totalling 100
	$ time ./window_sieve -f -v -w 50 -u 100
	HARDWARE INFO
	Model Name: Intel(R) Core(TM) i7-8650U CPU @ 1.90GHz
	Number of Cores: 8
	Total RAM: 31608 GB
	Free RAM: 25768 GB
	Used RAM: 5840 GB
	Load Average: 1-minute: 0.39, 5-minute: 0.71, 15-minute: 0.95
	primes.bin deleted successfully
	primes.csv deleted successfully
	Window size: 50
	Upper limit: 100
	current_window: 0
	current_window: 50
	creating primes.csv from primes.bin
	Found 25 primes
	
	real    0m0.013s
	user    0m0.007s
	sys     0m0.006s
# EXECUTION 3 100 byte buffer totalling 100. does 1 iteratoin.
	$ time ./window_sieve -f -v -w 100 -u 100
	HARDWARE INFO
	Model Name: Intel(R) Core(TM) i7-8650U CPU @ 1.90GHz
	Number of Cores: 8
	Total RAM: 31608 GB
	Free RAM: 25758 GB
	Used RAM: 5849 GB
	Load Average: 1-minute: 0.56, 5-minute: 0.73, 15-minute: 0.95
	primes.bin deleted successfully
	primes.csv deleted successfully
	Window size: 100
	Upper limit: 100
	current_window: 0
	creating primes.csv from primes.bin
	Found 25 primes
	
	real    0m0.012s
	user    0m0.001s
	sys     0m0.011s
