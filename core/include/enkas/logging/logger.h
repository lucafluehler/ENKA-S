#pragma once

#include <format>
#include <memory>
#include <mutex>
#include <source_location>
#include <string_view>

namespace enkas::logging {

enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, CRITICAL, NONE };

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void log(LogLevel level, std::string_view message) = 0;
};

class Logger {
public:
    void configure(LogLevel level, std::shared_ptr<LogSink> sink) {
        std::lock_guard<std::mutex> lock(mutex_);
        level_ = level;
        sink_ = std::move(sink);
    }

    template <typename... Args>
    void log(LogLevel level,
             const std::source_location& loc,
             const std::format_string<Args...>& fmt,
             Args&&... args) {
        LogLevel current_level;
        std::shared_ptr<LogSink> current_sink;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            current_level = level_;
            current_sink = sink_;
        }

        if (level < current_level || !current_sink) {
            return;
        }

        auto formatted_message =
            std::format("[{}:{}] {}",
                        loc.file_name(),
                        loc.line(),
                        std::vformat(fmt.get(), std::make_format_args(args...)));

        current_sink->log(level, formatted_message);
    }

    template <typename... Args>
    void trace(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::TRACE, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::DEBUG, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::INFO, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warning(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::WARNING, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::ERROR, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(const std::format_string<Args...>& fmt, Args&&... args) {
        log(LogLevel::CRITICAL, std::source_location::current(), fmt, std::forward<Args>(args)...);
    }

private:
    std::mutex mutex_;
    LogLevel level_ = LogLevel::NONE;
    std::shared_ptr<LogSink> sink_;
};

inline Logger& getLogger() {
    static Logger logger;
    return logger;
}

}  // namespace enkas::logging
