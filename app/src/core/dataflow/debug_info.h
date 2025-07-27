#pragma once

#include <atomic>
#include <cstddef>
#include <limits>

static constexpr size_t QUEUE_NOT_PRESENT = std::numeric_limits<size_t>::max();

struct LiveDebugInfo {
    // Simulation statistics
    std::atomic<int> current_step = 0;
    std::atomic<double> time = 0.0;
    double duration = 0.0;

    // Memory pool statistics
    std::atomic<size_t> system_data_pool_size = 0;
    size_t system_data_pool_capacity = 0;

    std::atomic<size_t> diagnostics_data_pool_size = 0;
    size_t diagnostics_data_pool_capacity = 0;

    std::atomic<size_t> system_snapshot_pool_size = 0;
    size_t system_snapshot_pool_capacity = 0;

    std::atomic<size_t> diagnostics_snapshot_pool_size = 0;
    size_t diagnostics_snapshot_pool_capacity = 0;

    // Consumer queue statistics
    std::atomic<size_t> chart_queue_size = 0;
    size_t chart_queue_capacity = 0;

    std::atomic<size_t> system_storage_queue_size = QUEUE_NOT_PRESENT;
    size_t system_storage_queue_capacity = QUEUE_NOT_PRESENT;

    std::atomic<size_t> diagnostics_storage_queue_size = QUEUE_NOT_PRESENT;
    size_t diagnostics_storage_queue_capacity = QUEUE_NOT_PRESENT;
};
