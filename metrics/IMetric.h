#pragma once

#include <string>
#include <iostream>

class IMetric {
public:
    virtual ~IMetric() = default;

    virtual const std::string& getName() const = 0;
    virtual void writeValue(std::ostream& out) const = 0;
    virtual void reset() = 0;
};
