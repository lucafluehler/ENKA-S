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
             std::format_string<Args...> fmt,
             Args&&... args,
             const std::source_location& loc = std::source_location::current()) {
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
    void trace(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::TRACE, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void critical(std::format_string<Args...> fmt, Args&&... args) {
        log(LogLevel::CRITICAL, fmt, std::forward<Args>(args)...);
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
