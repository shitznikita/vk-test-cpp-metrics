#pragma once

#include "../metrics/IMetric.h"

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <atomic>
#include <thread>

class MetricsCollector {
public:
    MetricsCollector();
    ~MetricsCollector();

    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;

    template <typename T, typename... Args>
    T& setMetric(Args&&... args);

    void start(const std::string& filepath, std::chrono::milliseconds interval);

    void stop();

private:
    void writerLoop();
    std::string getCurrentTimestamp();

    std::vector<std::unique_ptr<IMetric>> metrics_;
    std::mutex metrics_mutex_;

    std::ofstream file_;
    std::chrono::milliseconds write_interval_;
    std::atomic<bool> is_running_ = false;
    std::thread writer_thread_;
};

template <typename T, typename... Args>
T& MetricsCollector::setMetric(Args&&... args) {
    auto metric = std::make_unique<T>(std::forward<Args>(args)...);
    T* raw_ptr = metric.get();

    std::lock_guard<std::mutex> lock(metrics_mutex_);
    metrics_.push_back(std::move(metric));

    return *raw_ptr;
}