
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
   - Persistent storage for discovered primes
   - Each record contains:
     - `p`: The prime number
     - `nextval`: Next multiple of this prime to be marked as composite

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
   - Mark all its multiples as composite within current window
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
### Debug build
	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Debug


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
	
	
