#pragma once

#include <chrono>
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

inline std::string_view get_filename(std::string_view full_path) {
    auto const pos = full_path.find_last_of("/\\");
    if (pos != std::string_view::npos) {
        return full_path.substr(pos + 1);
    }
    return full_path;
}

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
            std::format("[{:%Y-%m-%d %H:%M:%S}] [{}:{}] {}",
                        std::chrono::system_clock::now(),
                        get_filename(loc.file_name()),
                        loc.line(),
                        std::vformat(fmt.get(), std::make_format_args(args...)));

        current_sink->log(level, formatted_message);
    }

    template <typename... Args>
    void trace(const std::source_location& loc,
               const std::format_string<Args...>& fmt,
               Args&&... args) {
        log(LogLevel::TRACE, loc, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(const std::source_location& loc,
               const std::format_string<Args...>& fmt,
               Args&&... args) {
        log(LogLevel::DEBUG, loc, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(const std::source_location& loc,
              const std::format_string<Args...>& fmt,
              Args&&... args) {
        log(LogLevel::INFO, loc, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warning(const std::source_location& loc,
                 const std::format_string<Args...>& fmt,
                 Args&&... args) {
        log(LogLevel::WARNING, loc, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(const std::source_location& loc,
               const std::format_string<Args...>& fmt,
               Args&&... args) {
        log(LogLevel::ERROR, loc, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(const std::source_location& loc,
                  const std::format_string<Args...>& fmt,
                  Args&&... args) {
        log(LogLevel::CRITICAL, loc, fmt, std::forward<Args>(args)...);
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
