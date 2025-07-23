#pragma once

#include <cmath>
#include <vector>

/**
 * @brief Downsamples a series of data points using the Largest-Triangle-Three-Buckets (LTTB)
 * algorithm.
 *
 * This algorithm preserves the visual characteristics of the original data by selecting points
 * that form the largest triangles with the previously selected point. It's more effective than
 * simple sampling for creating visually representative charts.
 *
 * @tparam PointT The type of the points in the data series. Must provide .x() and .y() methods.
 * @param data The input vector of data points, sorted by the x-coordinate.
 * @param threshold The desired number of points in the downsampled output.
 * @return A vector containing the downsampled data points.
 */
template <typename PointT>
std::vector<PointT> largestTriangleThreeBuckets(const std::vector<PointT>& data, size_t threshold) {
    if (threshold >= data.size() || threshold <= 2) {
        return data;  // Not enough data or no downsampling needed
    }

    std::vector<PointT> sampled;
    sampled.reserve(threshold);

    const double bucket_size = static_cast<double>(data.size() - 2) / (threshold - 2);

    // Always add the first point
    sampled.push_back(data.front());

    size_t a = 0;  // Index of the last selected point

    for (size_t i = 0; i < threshold - 2; ++i) {
        // Calculate the range for the next bucket
        const size_t bucket_start = static_cast<size_t>(std::floor((i + 1) * bucket_size)) + 1;
        const size_t bucket_end =
            std::min(static_cast<size_t>(std::floor((i + 2) * bucket_size)) + 1, data.size());

        if (bucket_start >= bucket_end) {
            // This can happen with floating point inaccuracies, handle gracefully
            continue;
        }

        // Calculate the average point for the current bucket
        double avg_x = 0;
        double avg_y = 0;
        const size_t avg_range_start = static_cast<size_t>(std::floor(i * bucket_size)) + 1;
        const size_t avg_range_end = bucket_start;

        for (size_t j = avg_range_start; j < avg_range_end; ++j) {
            avg_x += data[j].x();
            avg_y += data[j].y();
        }
        const size_t avg_count = avg_range_end - avg_range_start;
        avg_x /= avg_count;
        avg_y /= avg_count;

        // Find the point in the next bucket that forms the largest triangle
        double max_area = -1.0;
        size_t max_area_point_index = 0;

        const PointT& point_a = data[a];

        for (size_t j = bucket_start; j < bucket_end; ++j) {
            const PointT& point_c = data[j];
            // Calculate triangle area using cross-product. 0.5 is omitted for performance as we
            // only need relative area.
            const double area = std::abs((point_a.x() - avg_x) * (point_c.y() - point_a.y()) -
                                         (point_a.x() - point_c.x()) * (avg_y - point_a.y()));

            if (area > max_area) {
                max_area = area;
                max_area_point_index = j;
            }
        }

        sampled.push_back(data[max_area_point_index]);
        a = max_area_point_index;  // Update the last selected point
    }

    // Always add the last point
    sampled.push_back(data.back());

    return sampled;
}
