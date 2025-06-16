#pragma once

#include "IMetric.h"

#include <atomic>
#include <string>

// Метрика-счётчик для значений типа int
class CounterMetric : public IMetric {
public:
    explicit CounterMetric(const std::string& name)
        : name_(name)
        , value_(0)
    {}

    const std::string& getName() const override {
        return name_;
    }

    void writeValue(std::ostream& out) const override {
        out << value_.load();
    }

    void reset() override {
        value_ = 0;
    }

    void increment(int val = 1) {
        value_ += val;
    }

private:
    std::string name_;
    std::atomic<int> value_;
};

// Метрика-датчик для вещественных значений
class SensorMetric : public IMetric {
public:
    explicit SensorMetric(const std::string& name) 
        : name_(name)
        , value_(0.0) 
    {}

    const std::string& getName() const override {
        return name_;
    }

    void writeValue(std::ostream& out) const override {
        out << value_.load();
    }

    void reset() override {
        value_ = 0.0;
    }

    void set(double val) {
        value_ = val;
    }

private:
    std::string name_;
    std::atomic<double> value_;
};
