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
        size_t size;
        bool is_allocated;
        char* data;
        
        MemoryBlock(size_t s, char* d) : size(s), is_allocated(false), data(d) {}
    };

    std::vector<MemoryBlock> blocks;
    char* pool;
    size_t total_size;
    size_t used_size;

public:
    MemoryPool(size_t size) : total_size(size), used_size(0) {
        pool = new char[size];
        blocks.push_back(MemoryBlock(size, pool));
    }

    ~MemoryPool() {
        delete[] pool;
    }

    void* allocate(size_t size) {
        if (size == 0) return nullptr;

        // Find first fit block
        for (auto& block : blocks) {
            if (!block.is_allocated && block.size >= size) {
                // Split block if it's significantly larger
                if (block.size > size + sizeof(MemoryBlock)) {
                    size_t remaining_size = block.size - size;
                    block.size = size;
                    blocks.push_back(MemoryBlock(remaining_size, block.data + size));
                }
                
                block.is_allocated = true;
                used_size += block.size;
                return block.data;
            }
        }
        
        throw std::bad_alloc();
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

        auto it = std::find_if(blocks.begin(), blocks.end(),
            [ptr](const MemoryBlock& block) { return block.data == ptr; });

        if (it != blocks.end()) {
            it->is_allocated = false;
            used_size -= it->size;
            
            // Merge with next block if free
            auto next = std::next(it);
            if (next != blocks.end() && !next->is_allocated) {
                it->size += next->size;
                blocks.erase(next);
            }
        }
    }

    double fragmentation_ratio() const {
        size_t largest_free_block = 0;
        size_t total_free = total_size - used_size;

        for (const auto& block : blocks) {
            if (!block.is_allocated && block.size > largest_free_block) {
                largest_free_block = block.size;
            }
        }

        return total_free > 0 ? 
            1.0 - (static_cast<double>(largest_free_block) / total_free) : 0.0;
    }

    void print_stats() const {
        std::cout << "Memory Pool Stats:\n"
                  << "Total Size: " << total_size << " bytes\n"
                  << "Used Size: " << used_size << " bytes\n"
                  << "Free Size: " << (total_size - used_size) << " bytes\n"
                  << "Fragmentation: " << (fragmentation_ratio() * 100) << "%\n"
                  << "Number of blocks: " << blocks.size() << "\n";
    }
};
