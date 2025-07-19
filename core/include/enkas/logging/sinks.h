#pragma once

#include <enkas/logging/logger.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace enkas::logging {

/**
 * @brief A thread-safe sink that writes log messages to the console.
 * Writes WARNING and above to std::cerr, and lower levels to std::cout.
 */
class ConsoleSink : public LogSink {
public:
    void log(LogLevel level, std::string_view message) override {
        // Lock to prevent garbled output from multiple threads writing at once.
        std::lock_guard<std::mutex> lock(mutex_);
        if (level >= LogLevel::WARNING) {
            std::cerr << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }

private:
    std::mutex mutex_;
};

/**
 * @brief A sink that does nothing. Useful for completely disabling logging.
 */
class NullSink : public LogSink {
public:
    void log(LogLevel, std::string_view) override {
        // Intentionally empty.
    }
};

/**
 * @brief A sink that forwards log messages to multiple other sinks.
 * This class is thread-safe.
 */
class MultiSink : public LogSink {
public:
    void addSink(std::shared_ptr<LogSink> sink) {
        if (sink) {
            std::lock_guard<std::mutex> lock(mutex_);
            sinks_.push_back(std::move(sink));
        }
    }

    void log(LogLevel level, std::string_view message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& sink : sinks_) {
            sink->log(level, message);
        }
    }

private:
    std::mutex mutex_;
    std::vector<std::shared_ptr<LogSink>> sinks_;
};

}  // namespace enkas::logging
