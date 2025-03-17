#include "memory_pool.hpp"
#include "device_driver.hpp"
#include "logger.hpp"
#include "cli.hpp"
#include <iostream>
#include <vector>
#include <string>

void run_test_sequence(CLI& cli) {
    std::vector<std::string> test_commands = {
        "allocate 1024",     // Allocate 1KB
        "submit read 512",   // Submit a read operation
        "stats",             // View system statistics
        "allocate 2048",     // Allocate 2KB
        "submit write 1024", // Submit a write operation
        "stats",             // View updated statistics
        "exit"
    };

    cli.run_test_sequence(test_commands);
}

int main(int argc, char* argv[]) {
    try {
        // Initialize components
        constexpr size_t POOL_SIZE = 1024 * 1024; // 1MB memory pool
        MemoryPool memory_pool(POOL_SIZE);
        DeviceDriver device_driver;
        Logger& logger = Logger::get_instance();

        // Set up logging
        logger.set_min_level(Logger::Level::INFO);
        logger.info("Kernel simulation starting");

        // Start device driver processing
        device_driver.start_processing();
        logger.info("Device driver initialized");

        // Check if we're in test mode
        bool test_mode = (argc > 1 && std::string(argv[1]) == "--test");

        // Create CLI with test mode flag
        CLI cli(memory_pool, device_driver, test_mode);

        if (test_mode) {
            logger.info("Running in test mode");
            run_test_sequence(cli);
        } else {
            cli.run();
        }

        // Cleanup
        device_driver.stop_processing();
        logger.info("Kernel simulation shutting down");

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}