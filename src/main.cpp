
/*******************************************************************************
 * Main Application Entry Point
 * ---------------------------
 * This file demonstrates the initialization and orchestration of core kernel 
 * simulation components including memory management, device I/O, and CLI.
 * 
 * Key Concepts Demonstrated:
 * 1. Component initialization and lifecycle management
 * 2. Exception handling for system-level operations
 * 3. Command-line argument processing
 * 4. Test mode vs interactive mode operation
 ******************************************************************************/

#include "memory_pool.hpp"
#include "device_driver.hpp"
#include "logger.hpp"
#include "cli.hpp"
#include <iostream>
#include <vector>
#include <string>

/**
 * Executes a predefined sequence of test commands
 * This function demonstrates automated testing of system components
 * through a series of representative operations
 */
void run_test_sequence(CLI& cli) {
    // Define a sequence of commands that exercise different system features
    std::vector<std::string> test_commands = {
        "allocate 1024",     // Test memory allocation with 1KB
        "submit read 512",   // Test device I/O with a read operation
        "stats",             // Verify system state through statistics
        "allocate 2048",     // Test another memory allocation
        "submit write 1024", // Test device I/O with a write operation
        "stats",             // Final state verification
        "exit"              // Clean shutdown
    };

    cli.run_test_sequence(test_commands);
}

/**
 * Main Entry Point
 * ---------------
 * Demonstrates the complete lifecycle of our kernel simulation:
 * 1. Component initialization
 * 2. System configuration
 * 3. Operation (either test or interactive)
 * 4. Graceful shutdown
 */
int main(int argc, char* argv[]) {
    try {
        // System Initialization Phase
        // --------------------------
        
        // Create a 1MB memory pool to demonstrate memory management
        constexpr size_t POOL_SIZE = 1024 * 1024; // 1MB memory pool
        MemoryPool memory_pool(POOL_SIZE);
        
        // Initialize device driver for I/O operations simulation
        DeviceDriver device_driver;
        
        // Set up logging system for system monitoring
        Logger& logger = Logger::get_instance();
        logger.set_min_level(Logger::Level::INFO);
        logger.info("Kernel simulation starting");

        // Start asynchronous device request processing
        device_driver.start_processing();
        logger.info("Device driver initialized");

        // Determine operation mode (test vs interactive)
        bool test_mode = (argc > 1 && std::string(argv[1]) == "--test");

        // Create command interface with appropriate mode
        CLI cli(memory_pool, device_driver, test_mode);

        // Operation Phase
        // --------------
        if (test_mode) {
            logger.info("Running in test mode");
            run_test_sequence(cli);
        } else {
            cli.run(); // Interactive mode
        }

        // Cleanup Phase
        // ------------
        device_driver.stop_processing();
        logger.info("Kernel simulation shutting down");

    } catch (const std::exception& e) {
        // Error handling for system-level failures
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
