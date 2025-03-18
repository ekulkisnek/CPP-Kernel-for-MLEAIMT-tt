# Kernel Simulation System

## Overview
This project implements a simplified kernel simulation system that demonstrates core operating system concepts through practical implementations.

## System Architecture
```
Memory Layout:
+----------------+
| Memory Pool    |
|  +----------+  |
|  | Block 1  |  |
|  +----------+  |
|  | Block 2  |  |
|  +----------+  |
|  | ...      |  |
+----------------+

Request Flow:
[CLI] -> [Device Driver Queue] -> [Processing Thread]
          ^                            |
          |                           v
   [Status Monitoring]           [I/O Operation]

Producer-Consumer Pattern:
Producer (CLI)     Consumer (Device Thread)
    |                    |
    v                    v
[Request Queue] <-- [Process Requests]
```

## Core Components

### 1. Memory Pool (`memory_pool.hpp`)
Demonstrates OS memory management concepts:
- **Paging Concept**: Block-based memory division
- **Fragmentation Types**: 
  - Internal: Within allocated blocks
  - External: Between blocks
- **Allocation Strategies**: First-fit implementation
- **Memory Coalescing**: Merging adjacent free blocks

### 2. Device Driver (`device_driver.hpp`)
Implements key I/O concepts:
- **Producer-Consumer Pattern**: CLI produces requests, device thread consumes
- **I/O Scheduling**: FIFO queue implementation
- **Asynchronous I/O**: Non-blocking request processing
- **Device States**: READY, BUSY, ERROR state management

### 3. Logger (`logger.hpp`)
System monitoring features:
- **Event Tracking**: Hierarchical logging levels
- **Thread Safety**: Mutex-based synchronization
- **Diagnostic Support**: Timestamped entries

## Performance Characteristics
- Memory allocation: O(n) search time
- Device queue: O(1) enqueue/dequeue
- Thread synchronization overhead: ~microseconds
- Maximum queue depth: 100 requests

## Common Failure Scenarios
1. Memory Exhaustion
   - Symptoms: bad_alloc exception
   - Handling: Request rejection, fragmentation management

2. Queue Overflow
   - Symptoms: Queue full errors
   - Handling: Back-pressure implementation

3. Device Errors
   - Symptoms: ERROR state
   - Handling: Automatic retry, error logging

## Educational Test Scenarios
Run with `./main --test` to demonstrate:
1. Memory fragmentation patterns
2. Producer-consumer interactions
3. Error handling mechanisms
4. Performance under load

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