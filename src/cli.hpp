/*******************************************************************************
 * Command Line Interface
 * ---------------------
 * Implements a shell-like interface for system interaction:
 * 
 * I. Command Processing Architecture:
 * 1. Command Parser
 *    - Token separation
 *    - Argument validation
 *    - Command recognition
 * 
 * 2. Dispatch System
 *    - Command registration
 *    - Handler mapping
 *    - Error propagation
 * 
 * 3. Mode Support
 *    - Interactive shell
 *    - Batch processing
 *    - Test sequence execution
 * 
 * II. System Integration:
 * 1. Component Interaction
 *    - Memory management
 *    - Device operations
 *    - Logging services
 * 
 * 2. Resource Management
 *    - Command history
 *    - Error recovery
 *    - State persistence
 * 
 * III. User Interface Design:
 * 1. Input Processing
 *    - Command completion
 *    - Error reporting
 *    - Help system
 * 
 * 2. Output Formatting
 *    - Status display
 *    - Error messages
 *    - Statistics reporting
 * 
 * Command Structure:
 * command [arg1] [arg2] ... [argN]
 * 
 * Error Handling:
 * - Invalid syntax: User feedback
 * - System errors: Exception capture
 * - Resource limits: Graceful degradation
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

/**
 * Command Line Interface (CLI) Class
 * ================================
 * Implements an interactive shell for system control and monitoring,
 * demonstrating command processing and system integration patterns.
 * 
 * Operating System Concepts Demonstrated:
 * 1. Command Processing
 *    - Token parsing
 *    - Argument validation
 *    - Command dispatch
 * 
 * 2. System Integration
 *    - Component interaction
 *    - Resource management
 *    - Error propagation
 * 
 * 3. User Interface
 *    - Interactive mode
 *    - Batch processing
 *    - Error reporting
 * 
 * Architecture Overview:
 * [User Input] → [Parser] → [Command Registry] → [System Components]
 *     ↑                                                  |
 *     └──────────────── [Output/Feedback] ←─────────────┘
 */
class CLI {
private:
    /**
     * System Component References
     * -------------------------
     * Core system services integrated through dependency injection:
     * - memory_pool:    Memory management operations
     * - device_driver:  I/O request processing
     * - logger:         System event recording
     */
    MemoryPool& memory_pool;      // Memory subsystem interface
    DeviceDriver& device_driver;   // I/O subsystem interface
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