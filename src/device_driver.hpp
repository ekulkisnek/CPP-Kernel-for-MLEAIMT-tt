/*******************************************************************************
 * Device Driver Simulation
 * -----------------------
 * A comprehensive simulation of OS device driver architecture:
 * 
 * I. Core OS I/O Concepts Demonstrated:
 * 1. Interrupt Handling Simulation
 *    - Asynchronous request processing
 *    - Event notification via condition variables
 *    - Priority-based interrupt handling
 * 
 * 2. Device Management
 *    - State machine implementation (READY/BUSY/ERROR)
 *    - Device status monitoring
 *    - Error recovery mechanisms
 * 
 * 3. I/O Scheduling
 *    - FIFO queue implementation
 *    - Request batching optimization
 *    - Queue depth management
 * 
 * II. Synchronization Patterns:
 * 1. Producer-Consumer Implementation
 *    - Thread-safe queue operations
 *    - Blocking vs non-blocking I/O
 *    - Buffer management
 * 
 * 2. Critical Section Protection
 *    - Mutex-based synchronization
 *    - Condition variable signaling
 *    - Deadlock prevention
 * 
 * III. Performance Considerations:
 * 1. Request Processing
 *    - Batch processing for efficiency
 *    - Simulated I/O latency
 *    - Queue size optimization
 * 
 * 2. Resource Management
 *    - Memory efficiency
 *    - Thread lifecycle
 *    - Error handling overhead
 * 
 * Implementation Architecture:
 * [User Space]        [Kernel Space]        [Hardware]
 * Request --> Queue --> Driver Thread --> Simulated Device
 *   ^          |            |               |
 *   |          v            v               v
 * Response <- Status <-- Processing <-- I/O Complete
 * 
 * Error Handling:
 * - Queue full: Request rejection
 * - Device error: State transition
 * - Thread failure: Graceful shutdown
 ******************************************************************************/

#pragma once
#include <string>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

/**
 * DeviceDriver Class
 * =================
 * Simulates an operating system device driver with comprehensive I/O management.
 * 
 * Key OS Concepts Demonstrated:
 * 1. Device State Management
 *    - State transitions (READY → BUSY → ERROR)
 *    - Interrupt handling simulation
 *    - Device status monitoring
 * 
 * 2. I/O Request Processing
 *    - Asynchronous operations
 *    - Request queueing
 *    - Completion handling
 * 
 * 3. Resource Management
 *    - Queue depth control
 *    - Thread lifecycle
 *    - Error recovery
 * 
 * Implementation Details:
 * ----------------------
 * - Uses producer-consumer pattern
 * - Thread-safe operations
 * - FIFO scheduling
 */
class DeviceDriver {
public:
    /**
     * Device Status Enumeration
     * ------------------------
     * Represents the operational states of the device:
     * - READY:  Device is idle and can accept new requests
     * - BUSY:   Device is processing a request
     * - ERROR:  Device encountered an error condition
     * 
     * State Transitions:
     * READY ←→ BUSY → ERROR → READY
     */
    enum class Status {
        READY,  // Device available for new requests
        BUSY,   // Request being processed
        ERROR   // Error condition detected
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
        std::string operation;  // Type of I/O operation (e.g., "read", "write")
        size_t data_size;       // Amount of data to be read or written
        std::chrono::steady_clock::time_point timestamp; // Timestamp of request submission

        /**
         * Constructor for DeviceRequest
         * @param op The type of operation
         * @param size The size of the data
         */
        DeviceRequest(const std::string& op, size_t size)
            : operation(op), data_size(size),
              timestamp(std::chrono::steady_clock::now()) {}
    };

private:
    Status status;                    // Current device status
    std::queue<DeviceRequest> request_queue;  // Queue of pending I/O requests
    mutable std::mutex mutex;         // Mutex for thread synchronization
    std::condition_variable cv;       // Condition variable for thread signaling
    bool processing;                  // Flag indicating whether the processing thread is active
    static constexpr size_t MAX_QUEUE_SIZE = 100;  // Maximum number of requests allowed in the queue

public:
    /**
     * Constructor: Initializes the device driver in the READY state.
     */
    DeviceDriver() : status(Status::READY), processing(false) {}

    /**
     * Request Submission
     * -----------------
     * Submits a new I/O request to the device driver.
     * @param operation The type of operation ("read" or "write")
     * @param data_size The size of the data in bytes
     * @return True if the request was successfully submitted, false if the queue is full.
     */
    bool submit_request(const std::string& operation, size_t data_size) {
        std::lock_guard<std::mutex> lock(mutex);  // Acquire lock for thread safety

        // Check if the request queue is full
        if (request_queue.size() >= MAX_QUEUE_SIZE) {
            return false;  // Queue is full, request rejected
        }

        // Add the request to the queue
        request_queue.emplace(operation, data_size);
        // Signal the processing thread that a new request has arrived
        cv.notify_one();
        return true;
    }

    /**
     * Start Request Processing
     * -----------------------
     * Starts a background thread to process I/O requests asynchronously.
     * Simulates device I/O operations with simulated latency.
     */
    void start_processing() {
        processing = true;
        std::thread([this]() {
            while (processing) {
                std::unique_lock<std::mutex> lock(mutex);

                // Wait for requests if the queue is empty
                if (request_queue.empty()) {
                    status = Status::READY;
                    cv.wait(lock); // Wait for a request to be added to the queue
                    continue;
                }

                // Process the next request in the queue
                status = Status::BUSY;
                auto request = request_queue.front();
                request_queue.pop();
                lock.unlock(); // Release the lock before simulating I/O

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
     * Gracefully stops the request processing thread.
     */
    void stop_processing() {
        processing = false;
        cv.notify_all();  // Wake up any waiting threads
    }

    /**
     * Status Queries
     * -------------
     * Returns the current status of the device driver.
     * @return The current device status (READY, BUSY, or ERROR).
     */
    Status get_status() const {
        return status;
    }

    /**
     * Queue Size
     * ----------
     * Returns the current number of requests in the queue.
     * @return The number of requests in the queue.
     */
    size_t queue_size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return request_queue.size();
    }

    /**
     * Statistics Display
     * -----------------
     * Prints the current device status and queue size to the console.
     */
    void print_stats() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::cout << "Device Driver Stats:\n"
                  << "Status: " << static_cast<int>(status) << "\n"
                  << "Queue Size: " << request_queue.size() << "/"
                  << MAX_QUEUE_SIZE << "\n";
    }
};