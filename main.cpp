#include "metrics-collector/MetricsCollector.h"
#include "metrics/Metrics.h"

#include <iostream>
#include <atomic>
#include <vector>
#include <thread>
#include <random>

void workerFunc(CounterMetric& http_requests, SensorMetric& cpu, std::atomic<bool>& stop_flag) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> req_dist(10, 50);
    std::uniform_real_distribution<> cpu_dist(0.5, 2.0);

    while (!stop_flag) {
        http_requests.increment(req_dist(gen));
        cpu.set(cpu_dist(gen));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    using namespace std::chrono_literals;

    MetricsCollector collector;

    auto& cpu_metric = collector.setMetric<SensorMetric>("CPU");
    auto& http_metric = collector.setMetric<CounterMetric>("HTTP requests RPS");

    try {
        collector.start("metrics.log", 1s);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Starting metric collection for 5 seconds" << std::endl;

    std::atomic<bool> stop_workers(false);
    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back(workerFunc, std::ref(http_metric), std::ref(cpu_metric), std::ref(stop_workers));
    }

    std::this_thread::sleep_for(5s);

    stop_workers = true;
    for (auto& worker : workers) {
        worker.join();
    }

    collector.stop();

    std::cout << "Stopped metric collection. Check 'metrics.log' file" << std::endl;

    return 0;
}

