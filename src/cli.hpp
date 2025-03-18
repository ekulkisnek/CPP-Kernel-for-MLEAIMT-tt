
/*******************************************************************************
 * Command Line Interface
 * ---------------------
 * Demonstrates user interface concepts:
 * 
 * 1. Command parsing and dispatching
 * 2. Interactive vs batch operation
 * 3. System component integration
 * 4. Error handling and user feedback
 ******************************************************************************/

#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional>
#include <map>
#include "memory_pool.hpp"
#include "device_driver.hpp"
#include "logger.hpp"

class CLI {
private:
    MemoryPool& memory_pool;              // Reference to memory system
    DeviceDriver& device_driver;          // Reference to I/O system
    Logger& logger;                       // Reference to logging system
    bool running;                         // Main loop control
    bool test_mode;                       // Operation mode flag
    std::vector<std::string> test_outputs;// Test output collection

    /**
     * Command Registry
     * ---------------
     * Maps command names to:
     * - Help string
     * - Handler function
     */
    std::map<std::string, std::pair<std::string, std::function<void(const std::vector<std::string>&)>>> commands;

    /**
     * Command Parsing
     * --------------
     * Splits command string into tokens
     */
    std::vector<std::string> split_command(const std::string& input) {
        std::vector<std::string> tokens;
        std::stringstream ss(input);
        std::string token;

        while (ss >> token) {
            tokens.push_back(token);
        }

        return tokens;
    }

public:
    /**
     * Constructor: System Integration
     * -----------------------------
     * Initializes CLI and registers all available commands
     */
    CLI(MemoryPool& mp, DeviceDriver& dd, bool is_test = false)
        : memory_pool(mp), device_driver(dd), 
          logger(Logger::get_instance()), running(true), test_mode(is_test) {

        // Register available commands
        commands["help"] = {"Show available commands", 
            [this](const std::vector<std::string>&) { show_help(); }};

        commands["allocate"] = {"Allocate memory: allocate <size>",
            [this](const std::vector<std::string>& args) { handle_allocate(args); }};

        commands["submit"] = {"Submit device request: submit <operation> <size>",
            [this](const std::vector<std::string>& args) { handle_submit(args); }};

        commands["stats"] = {"Show system statistics",
            [this](const std::vector<std::string>&) { show_stats(); }};

        commands["exit"] = {"Exit the program",
            [this](const std::vector<std::string>&) { running = false; }};
    }

    /**
     * Command Execution
     * ----------------
     * Processes a single command with error handling
     */
    void execute_command(const std::string& input) {
        auto tokens = split_command(input);
        if (tokens.empty()) return;

        auto cmd = tokens[0];
        tokens.erase(tokens.begin());

        auto it = commands.find(cmd);
        if (it != commands.end()) {
            try {
                it->second.second(tokens);
            } catch (const std::exception& e) {
                logger.error("Command failed: " + std::string(e.what()));
            }
        } else {
            logger.warning("Unknown command: " + cmd);
        }
    }

    /**
     * Main Operation Loop
     * ------------------
     * Handles both interactive and test modes
     */
    void run() {
        logger.info("Starting CLI interface");
        show_help();

        while (running) {
            if (!test_mode) {
                std::cout << "> ";
                std::string input;
                std::getline(std::cin, input);
                execute_command(input);
            } else {
                break;  // Exit immediately in test mode
            }
        }
    }

    /**
     * Test Mode Operation
     * ------------------
     * Executes a sequence of commands for testing
     */
    void run_test_sequence(const std::vector<std::string>& commands) {
        for (const auto& cmd : commands) {
            logger.info("Test executing: " + cmd);
            execute_command(cmd);
        }
    }

private:
    /**
     * Command Handlers
     * ---------------
     * Individual command implementations
     */
    void show_help() {
        std::cout << "Available commands:\n";
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd.first << " - " << cmd.second.first << "\n";
        }
    }

    void handle_allocate(const std::vector<std::string>& args) {
        if (args.empty()) {
            logger.error("Size argument required for allocate");
            return;
        }

        try {
            size_t size = std::stoull(args[0]);
            void* ptr = memory_pool.allocate(size);
            logger.info("Allocated " + std::to_string(size) + " bytes at " + 
                       std::to_string(reinterpret_cast<uintptr_t>(ptr)));
        } catch (const std::exception& e) {
            logger.error("Allocation failed: " + std::string(e.what()));
        }
    }

    void handle_submit(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            logger.error("Operation and size arguments required for submit");
            return;
        }

        try {
            size_t size = std::stoull(args[1]);
            if (device_driver.submit_request(args[0], size)) {
                logger.info("Submitted device request: " + args[0] + 
                          " with size " + std::to_string(size));
            } else {
                logger.warning("Device queue full");
            }
        } catch (const std::exception& e) {
            logger.error("Submit failed: " + std::string(e.what()));
        }
    }

    void show_stats() {
        memory_pool.print_stats();
        device_driver.print_stats();
    }
};
