
/*******************************************************************************
 * Logger Implementation
 * -------------------
 * Demonstrates key concepts in system logging and thread safety:
 * 
 * 1. Singleton pattern for global access
 * 2. Thread-safe operations
 * 3. Log level filtering
 * 4. Timestamp-based logging
 ******************************************************************************/

#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include <ctime>
#include <sstream>
#include <iostream>

class Logger {
public:
    /**
     * Log Levels
     * ----------
     * Hierarchical logging levels for message filtering:
     * DEBUG   - Detailed information for debugging
     * INFO    - General information about system operation
     * WARNING - Potential issues that aren't errors
     * ERROR   - Serious problems that need attention
     */
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

private:
    static Logger* instance;          // Singleton instance
    static std::mutex mutex;          // Thread synchronization
    
    std::ostream& output;            // Output stream (console/file)
    Level min_level;                 // Minimum level to log

    /**
     * Private Constructor
     * ------------------
     * Prevents direct instantiation (Singleton pattern)
     */
    Logger() : output(std::cout), min_level(Level::INFO) {}

    /**
     * Timestamp Generation
     * -------------------
     * Creates human-readable timestamps for log entries
     */
    static std::string get_timestamp() {
        auto now = std::time(nullptr);
        auto* tm = std::localtime(&now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        return buffer;
    }

    /**
     * Level to String Conversion
     * -------------------------
     * Converts log level to human-readable string
     */
    static const char* level_to_string(Level level) {
        switch (level) {
            case Level::DEBUG: return "DEBUG";
            case Level::INFO: return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

public:
    /**
     * Singleton Access
     * ---------------
     * Global access point to logger instance
     */
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    /**
     * Log Level Configuration
     * ----------------------
     * Sets minimum level for message filtering
     */
    void set_min_level(Level level) {
        min_level = level;
    }

    /**
     * Generic Logging Method
     * ---------------------
     * Thread-safe logging with level filtering
     */
    void log(Level level, const std::string& message) {
        if (level < min_level) return;  // Level filtering

        std::lock_guard<std::mutex> lock(mutex);  // Thread safety
        output << "[" << get_timestamp() << "] "
               << "[" << level_to_string(level) << "] "
               << message << std::endl;
    }

    /**
     * Convenience Methods
     * ------------------
     * Level-specific logging methods
     */
    void debug(const std::string& message) {
        log(Level::DEBUG, message);
    }

    void info(const std::string& message) {
        log(Level::INFO, message);
    }

    void warning(const std::string& message) {
        log(Level::WARNING, message);
    }

    void error(const std::string& message) {
        log(Level::ERROR, message);
    }
};

// Initialize static mutex
std::mutex Logger::mutex;
