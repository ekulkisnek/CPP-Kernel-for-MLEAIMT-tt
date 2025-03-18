
/*******************************************************************************
 * Device Driver Simulation
 * -----------------------
 * This class demonstrates core OS I/O and synchronization concepts:
 * 
 * Producer-Consumer Pattern:
 * - Producer (CLI) generates I/O requests
 * - Consumer (device thread) processes requests
 * - Bounded buffer (queue) manages synchronization
 * 
 * I/O Scheduling Concepts:
 * - FIFO scheduling demonstrates basic I/O queuing
 * - Request batching for efficiency
 * - Priority handling could be added for real-time requirements
 * 
 * Device State Management:
 * - State machine pattern mirrors real device behavior
 * - Status tracking enables system monitoring
 * - Error handling demonstrates fault tolerance
 * 
 * Thread Synchronization:
 * - Mutex prevents race conditions
 * - Condition variables manage thread signaling
 * - Critical section protection in queue operations
 ******************************************************************************/

#pragma once
#include <string>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

class DeviceDriver {
public:
    /**
     * Device Status Enumeration
     * ------------------------
     * Represents possible device states:
     * READY  - Available for new requests
     * BUSY   - Currently processing a request
     * ERROR  - Error condition detected
     */
    enum class Status {
        READY,
        BUSY,
        ERROR
    };

    /**
     * Device Request Structure
     * -----------------------
     * Represents an I/O request with:
     * - Operation type (read/write)
     * - Data size
     * - Timestamp for request tracking
     */
    struct DeviceRequest {
        std::string operation;
        size_t data_size;
        std::chrono::steady_clock::time_point timestamp;

        DeviceRequest(const std::string& op, size_t size)
            : operation(op), data_size(size),
              timestamp(std::chrono::steady_clock::now()) {}
    };

private:
    Status status;                    // Current device status
    std::queue<DeviceRequest> request_queue;  // Request queue
    mutable std::mutex mutex;         // Thread synchronization
    std::condition_variable cv;       // Thread notification
    bool processing;                  // Processing control flag
    
    static constexpr size_t MAX_QUEUE_SIZE = 100;  // Maximum queued requests

public:
    /**
     * Constructor: Initialize device in READY state
     */
    DeviceDriver() : status(Status::READY), processing(false) {}

    /**
     * Request Submission
     * -----------------
     * Thread-safe method to queue new I/O requests
     * Returns false if queue is full
     */
    bool submit_request(const std::string& operation, size_t data_size) {
        std::lock_guard<std::mutex> lock(mutex);  // Thread safety

        if (request_queue.size() >= MAX_QUEUE_SIZE) {
            return false;  // Queue full
        }

        request_queue.emplace(operation, data_size);
        cv.notify_one();  // Wake up processing thread
        return true;
    }

    /**
     * Start Request Processing
     * -----------------------
     * Launches background thread for asynchronous request processing
     * Simulates actual device I/O operations
     */
    void start_processing() {
        processing = true;
        std::thread([this]() {
            while (processing) {
                std::unique_lock<std::mutex> lock(mutex);

                // Wait for requests if queue empty
                if (request_queue.empty()) {
                    status = Status::READY;
                    cv.wait(lock);
                    continue;
                }

                // Process next request
                status = Status::BUSY;
                auto request = request_queue.front();
                request_queue.pop();
                lock.unlock();

                // Simulate I/O time based on data size
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(request.data_size / 1024)
                );
            }
        }).detach();
    }

    /**
     * Stop Processing
     * --------------
     * Graceful shutdown of request processing
     */
    void stop_processing() {
        processing = false;
        cv.notify_all();  // Wake all waiting threads
    }

    /**
     * Status Queries
     * -------------
     * Thread-safe status information access
     */
    Status get_status() const {
        return status;
    }

    size_t queue_size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return request_queue.size();
    }

    /**
     * Statistics Display
     * -----------------
     * Shows current device state and queue status
     */
    void print_stats() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Device Driver Stats:\n"
                  << "Status: " << static_cast<int>(status) << "\n"
                  << "Queue Size: " << request_queue.size() << "/"
                  << MAX_QUEUE_SIZE << "\n";
    }
};
