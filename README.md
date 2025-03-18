
# Kernel Simulation System

## Overview
This project implements a simplified kernel simulation system that demonstrates core operating system concepts including memory management, device I/O handling, and command-line interface interactions. The system is written in modern C++ and showcases various design patterns and system programming concepts.

## Core Components

### 1. Memory Pool (`memory_pool.hpp`)
The memory pool component implements a custom memory allocator that manages a fixed-size block of memory. Key features include:
- Dynamic memory block allocation and deallocation
- Memory fragmentation tracking
- Block splitting and merging
- Real-time memory statistics

#### Memory Statistics
- Total memory size tracking
- Used/free memory monitoring
- Fragmentation ratio calculation
- Block count management

### 2. Device Driver (`device_driver.hpp`)
Simulates hardware device interactions with the following features:
- Asynchronous request processing
- Request queue management
- Thread-safe operations
- Status monitoring (READY, BUSY, ERROR)
- Configurable queue size (default: 100 requests)

### 3. Logger (`logger.hpp`)
Implements a thread-safe singleton logger with:
- Multiple logging levels (DEBUG, INFO, WARNING, ERROR)
- Timestamp-based logging
- Console output support
- Level-based filtering

### 4. Command Line Interface (`cli.hpp`)
Provides an interactive interface with the following commands:
- `help`: Display available commands
- `allocate <size>`: Allocate memory of specified size
- `submit <operation> <size>`: Submit device request
- `stats`: Display system statistics
- `exit`: Terminate the program

## Building and Running

### Prerequisites
- C++17 compatible compiler
- pthread support
- Standard C++ libraries

### Compilation
The project uses g++ for compilation. To build:
```bash
g++ -std=c++17 -pthread src/main.cpp -o main
```

### Running
Execute the compiled binary:
```bash
./main
```

For test mode:
```bash
./main --test
```

## Architecture

### Memory Management
The memory pool uses a block-based allocation strategy:
1. First-fit allocation algorithm
2. Dynamic block splitting
3. Adjacent block merging during deallocation
4. Fragmentation management

### Device I/O
Device operations are handled asynchronously:
1. Request submission to queue
2. Background thread processing
3. Status monitoring
4. Simulated processing delays based on request size

### Logging System
Thread-safe logging implementation:
1. Singleton pattern
2. Mutex-based synchronization
3. Hierarchical logging levels
4. Timestamp-based entries

## Usage Examples

### Basic Memory Operations
```
> allocate 1024
[INFO] Allocated 1024 bytes at <address>
```

### Device Operations
```
> submit read 512
[INFO] Submitted device request: read with size 512
```

### System Statistics
```
> stats
Memory Pool Stats:
Total Size: 1048576 bytes
Used Size: 1024 bytes
Free Size: 1047552 bytes
Fragmentation: 0%
Number of blocks: 2

Device Driver Stats:
Status: READY
Queue Size: 0/100
```

## Error Handling
The system implements comprehensive error handling:
1. Memory allocation failures
2. Invalid command formats
3. Queue overflow conditions
4. Device operation errors

## Testing
The system includes a test mode that can be activated with the `--test` flag. This mode:
1. Runs a predefined sequence of commands
2. Validates system behavior
3. Provides automated testing capabilities

## Technical Details

### Memory Pool Implementation
- Block-based memory management
- O(n) allocation time complexity
- O(1) deallocation time complexity
- Fragmentation monitoring and reporting

### Device Driver Specifications
- Thread-safe queue implementation
- Configurable queue size
- Simulated processing delays
- Status monitoring system

### Logging System Design
- Singleton pattern implementation
- Thread-safe logging operations
- Timestamp-based entry format
- Multiple logging levels

## Performance Considerations
1. Memory allocation efficiency
2. Device queue throughput
3. Thread synchronization overhead
4. Logging system performance

## Limitations
1. Fixed memory pool size
2. Single device driver instance
3. Simplified device operations
4. Basic command set

## Future Enhancements
1. Multiple memory pool support
2. Enhanced device operation types
3. Network device simulation
4. Extended command set
5. Configuration file support

## Contributing
To contribute:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## License
This project is available under the MIT License.

