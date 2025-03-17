#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include <ctime>
#include <sstream>
#include <iostream>

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

private:
    static Logger* instance;
    static std::mutex mutex;
    
    std::ostream& output;
    Level min_level;

    Logger() : output(std::cout), min_level(Level::INFO) {}

    static std::string get_timestamp() {
        auto now = std::time(nullptr);
        auto* tm = std::localtime(&now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        return buffer;
    }

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
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }

    void set_min_level(Level level) {
        min_level = level;
    }

    void log(Level level, const std::string& message) {
        if (level < min_level) return;

        std::lock_guard<std::mutex> lock(mutex);
        output << "[" << get_timestamp() << "] "
               << "[" << level_to_string(level) << "] "
               << message << std::endl;
    }

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

std::mutex Logger::mutex;
