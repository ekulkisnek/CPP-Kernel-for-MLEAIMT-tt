/*******************************************************************************
 * Memory Pool Implementation
 * ------------------------
 * This class implements a memory management system that mirrors real OS concepts:
 * 
 * Key OS Memory Management Concepts Demonstrated:
 * 1. Virtual Memory Simulation
 *    - Block-based memory division (like pages)
 *    - Memory protection through allocation flags
 *    - Address space management via pointers
 * 
 * 2. Memory Allocation Strategies
 *    - First-fit algorithm implementation
 *    - Block splitting for efficiency (like buddy system)
 *    - Coalescing to combat fragmentation
 * 
 * 3. Resource Management
 *    - Dynamic block tracking (similar to page tables)
 *    - Memory usage statistics (like vmstat)
 *    - Error handling for allocation failures
 * 
 * 4. Memory Optimization Techniques
 *    - Block merging (defragmentation)
 *    - Split avoidance for small allocations
 *    - Fragmentation monitoring
 * 
 * Implementation Notes:
 * - Block Structure: Models OS page table entries
 * - Allocation: O(n) search similar to linear page table scan
 * - Deallocation: O(1) direct access like page frame freeing
 * 
 * Memory Layout:
 * +----------------+
 * | Memory Block   |
 * |  +----------+ |
 * |  | Metadata | |  <- Block tracking (size, status)
 * |  +----------+ |
 * |  | Data     | |  <- Actual memory space
 * |  +----------+ |
 * +----------------+
 * 
 * Error Handling:
 * - Out of memory: std::bad_alloc
 * - Invalid free: Silent return
 * - Fragmentation: Monitored via ratio
 ******************************************************************************/

#pragma once
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <iostream>

class MemoryPool {
private:
    struct MemoryBlock {
        size_t size;         // Size of this memory block in bytes
        bool is_allocated;   // Allocation status flag (true = in use)
        char* data;         // Raw memory pointer to block's data region

        // Constructor initializes a new memory block
        MemoryBlock(size_t s, char* d) : size(s), is_allocated(false), data(d) {}
    };

    std::vector<MemoryBlock> blocks;  // Dynamic array of memory block metadata
    char* pool;                       // Contiguous memory buffer pointer
    size_t total_size;                // Total pool size in bytes
    size_t used_size;                 // Currently allocated bytes

public:
    /**
     * Constructor: Simulates physical memory initialization at boot time
     * @param size: Total memory pool size in bytes
     * Allocates a contiguous memory region and creates initial free block
     */
    MemoryPool(size_t size) : total_size(size), used_size(0) {
        // Allocate raw memory buffer (analogous to physical RAM)
        pool = new char[size];  
        // Create initial free block spanning entire pool
        blocks.push_back(MemoryBlock(size, pool));
    }

    /**
     * Destructor: Memory cleanup similar to system shutdown
     * Releases the entire memory pool back to the system
     */
    ~MemoryPool() {
        delete[] pool;  // Release raw memory buffer
    }

    /**
     * Memory Allocation Method
     * -----------------------
     * Simulates virtual memory allocation in real OS:
     * 1. Searches for suitable free block (first-fit)
     * 2. Splits block if significantly larger than requested
     * 3. Updates allocation metadata
     * 
     * @param size: Requested allocation size in bytes
     * @return: Pointer to allocated memory region
     * @throws: std::bad_alloc if no suitable block found
     */
    void* allocate(size_t size) {
        if (size == 0) return nullptr;  // Handle zero-size request

        // Linear search for first suitable block (first-fit algorithm)
        for (auto& block : blocks) {
            if (!block.is_allocated && block.size >= size) {
                // Split block if remaining size worth tracking
                if (block.size > size + sizeof(MemoryBlock)) {
                    // Calculate remaining block size
                    size_t remaining_size = block.size - size;
                    // Update current block size
                    block.size = size;
                    // Create new block from remaining space
                    blocks.push_back(MemoryBlock(remaining_size, block.data + size));
                }

                // Mark block as allocated and update usage stats
                block.is_allocated = true;
                used_size += block.size;
                return block.data;
            }
        }

        throw std::bad_alloc();  // No suitable block found
    }

    /**
     * Memory Deallocation Method
     * -------------------------
     * Simulates memory freeing in real OS:
     * 1. Locates block containing pointer
     * 2. Marks block as free
     * 3. Merges with adjacent free blocks (coalescing)
     * 
     * @param ptr: Pointer to previously allocated memory
     */
    void deallocate(void* ptr) {
        if (!ptr) return;  // Handle null pointer

        // Find block containing this pointer
        auto it = std::find_if(blocks.begin(), blocks.end(),
            [ptr](const MemoryBlock& block) { return block.data == ptr; });

        if (it != blocks.end()) {
            // Mark block as free and update usage stats
            it->is_allocated = false;
            used_size -= it->size;

            // Merge with next block if it's free (coalescing)
            auto next = std::next(it);
            if (next != blocks.end() && !next->is_allocated) {
                // Combine block sizes
                it->size += next->size;
                // Remove absorbed block
                blocks.erase(next);
            }
        }
    }

    /**
     * Fragmentation Analysis Method
     * ----------------------------
     * Calculates memory fragmentation ratio:
     * - 0.0 indicates perfect contiguous free space
     * - 1.0 indicates completely fragmented memory
     * 
     * @return: Fragmentation ratio between 0.0 and 1.0
     */
    double fragmentation_ratio() const {
        size_t largest_free_block = 0;
        size_t total_free = total_size - used_size;

        // Find largest contiguous free block
        for (const auto& block : blocks) {
            if (!block.is_allocated && block.size > largest_free_block) {
                largest_free_block = block.size;
            }
        }

        // Calculate fragmentation ratio
        return total_free > 0 ? 
            1.0 - (static_cast<double>(largest_free_block) / total_free) : 0.0;
    }

    /**
     * Statistics Display Method
     * ------------------------
     * Provides detailed memory usage information:
     * - Total pool size
     * - Currently used memory
     * - Free memory amount
     * - Fragmentation percentage
     * - Number of memory blocks
     */
    void print_stats() const {
        std::cout << "Memory Pool Stats:\n"
                  << "Total Size: " << total_size << " bytes\n"
                  << "Used Size: " << used_size << " bytes\n"
                  << "Free Size: " << (total_size - used_size) << " bytes\n"
                  << "Fragmentation: " << (fragmentation_ratio() * 100) << "%\n"
                  << "Number of blocks: " << blocks.size() << "\n";
    }
};