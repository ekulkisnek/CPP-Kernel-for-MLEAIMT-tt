#pragma once
#include <string>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

class DeviceDriver {
public:
    enum class Status {
        READY,
        BUSY,
        ERROR
    };

    struct DeviceRequest {
        std::string operation;
        size_t data_size;
        std::chrono::steady_clock::time_point timestamp;

        DeviceRequest(const std::string& op, size_t size)
            : operation(op), data_size(size),
              timestamp(std::chrono::steady_clock::now()) {}
    };

private:
    Status status;
    std::queue<DeviceRequest> request_queue;
    mutable std::mutex mutex;  // Made mutable to allow locking in const members
    std::condition_variable cv;
    bool processing;

    static constexpr size_t MAX_QUEUE_SIZE = 100;

public:
    DeviceDriver() : status(Status::READY), processing(false) {}

    bool submit_request(const std::string& operation, size_t data_size) {
        std::lock_guard<std::mutex> lock(mutex);

        if (request_queue.size() >= MAX_QUEUE_SIZE) {
            return false;
        }

        request_queue.emplace(operation, data_size);
        cv.notify_one();
        return true;
    }

    void start_processing() {
        processing = true;
        std::thread([this]() {
            while (processing) {
                std::unique_lock<std::mutex> lock(mutex);

                if (request_queue.empty()) {
                    status = Status::READY;
                    cv.wait(lock);
                    continue;
                }

                status = Status::BUSY;
                auto request = request_queue.front();
                request_queue.pop();
                lock.unlock();

                // Simulate processing time based on data size
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(request.data_size / 1024)
                );
            }
        }).detach();
    }

    void stop_processing() {
        processing = false;
        cv.notify_all();
    }

    Status get_status() const {
        return status;
    }

    size_t queue_size() const {
        std::lock_guard<std::mutex> lock(mutex);  // Now works with mutable mutex
        return request_queue.size();
    }

    void print_stats() const {
        std::lock_guard<std::mutex> lock(mutex);  // Now works with mutable mutex
        std::cout << "Device Driver Stats:\n"
                  << "Status: " << static_cast<int>(status) << "\n"
                  << "Queue Size: " << request_queue.size() << "/"
                  << MAX_QUEUE_SIZE << "\n";
    }
};