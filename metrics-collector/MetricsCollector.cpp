#include "MetricsCollector.h"

#include <iostream>
#include <stdexcept>
#include <iomanip>

MetricsCollector::MetricsCollector() {}

MetricsCollector::~MetricsCollector() {
    if (is_running_) stop();
}

void MetricsCollector::start(const std::string& filepath, std::chrono::milliseconds interval) {
    if (is_running_) {
        std::cerr << "MetricsCollector is already running" << std::endl;
        return;
    }

    file_.open(filepath, std::ios::out | std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open metrics file: " + filepath);
    }

    write_interval_ = interval;
    is_running_ = true;
    writer_thread_ = std::thread(&MetricsCollector::writerLoop, this);
}

void MetricsCollector::stop() {
    is_running_ = false;
    if (writer_thread_.joinable()) {
        writer_thread_.join();
    }
    if (file_.is_open()) {
        file_.close();
    }
}

std::string MetricsCollector::getCurrentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time_t_now = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setw(3) << std::setfill('0') << ms.count();

    return ss.str();
}

void MetricsCollector::writerLoop() {
    while (is_running_) {
        std::this_thread::sleep_for(write_interval_);

        std::stringstream ss;
        ss << getCurrentTimestamp();

        {
            std::lock_guard<std::mutex> lock(metrics_mutex_);
            for (const auto& metric : metrics_) {
                ss << " \"" << metric->getName() << "\" ";
                metric->writeValue(ss);
            }

            for (const auto& metric : metrics_) {
                metric->reset();
            }
        }

        if (file_.good()) {
            file_ << ss.str() << std::endl;
        }
    }
}