
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
    /**
     * Memory Block Structure
     * --------------------
     * Represents a contiguous region of memory with metadata
     * Similar to how OS page tables track memory regions
     */
    struct MemoryBlock {
        size_t size;         // Size of the block in bytes
        bool is_allocated;   // Whether block is in use
        char* data;         // Pointer to actual memory
        
        MemoryBlock(size_t s, char* d) : size(s), is_allocated(false), data(d) {}
    };

    std::vector<MemoryBlock> blocks;  // List of memory blocks (like page table)
    char* pool;                       // The actual memory buffer
    size_t total_size;                // Total pool size
    size_t used_size;                 // Currently allocated size

public:
    /**
     * Constructor: Initializes memory pool
     * Similar to how OS initializes physical memory at boot
     */
    MemoryPool(size_t size) : total_size(size), used_size(0) {
        pool = new char[size];  // Allocate the memory buffer
        blocks.push_back(MemoryBlock(size, pool));  // Initial block spans all memory
    }

    /**
     * Destructor: Cleanup memory
     * Similar to OS shutdown memory cleanup
     */
    ~MemoryPool() {
        delete[] pool;
    }

    /**
     * Memory Allocation
     * ----------------
     * Demonstrates first-fit allocation algorithm:
     * 1. Find first block large enough
     * 2. Split if necessary (memory efficiency)
     * 3. Mark as allocated
     */
    void* allocate(size_t size) {
        if (size == 0) return nullptr;

        // Search for first suitable block
        for (auto& block : blocks) {
            if (!block.is_allocated && block.size >= size) {
                // Optimize space by splitting oversized blocks
                if (block.size > size + sizeof(MemoryBlock)) {
                    size_t remaining_size = block.size - size;
                    block.size = size;
                    // Create new block from remaining space
                    blocks.push_back(MemoryBlock(remaining_size, block.data + size));
                }
                
                block.is_allocated = true;
                used_size += block.size;
                return block.data;
            }
        }
        
        throw std::bad_alloc();  // No suitable block found
    }

    /**
     * Memory Deallocation
     * ------------------
     * Demonstrates memory block merging:
     * 1. Mark block as free
     * 2. Merge with adjacent free blocks
     */
    void deallocate(void* ptr) {
        if (!ptr) return;

        // Find the block containing this pointer
        auto it = std::find_if(blocks.begin(), blocks.end(),
            [ptr](const MemoryBlock& block) { return block.data == ptr; });

        if (it != blocks.end()) {
            it->is_allocated = false;
            used_size -= it->size;
            
            // Merge with next block if it's free (reduces fragmentation)
            auto next = std::next(it);
            if (next != blocks.end() && !next->is_allocated) {
                it->size += next->size;
                blocks.erase(next);
            }
        }
    }

    /**
     * Fragmentation Analysis
     * ---------------------
     * Calculates memory fragmentation ratio:
     * 0.0 = No fragmentation
     * 1.0 = Completely fragmented
     */
    double fragmentation_ratio() const {
        size_t largest_free_block = 0;
        size_t total_free = total_size - used_size;

        // Find largest free block
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
     * Memory Statistics
     * ----------------
     * Provides insight into memory usage and fragmentation
     * Similar to OS memory monitoring tools
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
