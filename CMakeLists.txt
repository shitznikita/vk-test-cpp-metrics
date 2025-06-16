cmake_minimum_required(VERSION 3.14)
project(MetricsLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(metrics_collector
    metrics-collector/MetricsCollector.cpp
)

target_include_directories(metrics_collector PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/metrics"
    "${CMAKE_CURRENT_SOURCE_DIR}/metrics-collector"
)

add_executable(example main.cpp)

target_link_libraries(example PRIVATE metrics_collector)

if (UNIX AND NOT APPLE)
    target_link_libraries(example PRIVATE pthread)
endif()