#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>

#ifdef _WIN32
  #include <io.h>
  #define CPPLOG_ISATTY() (_isatty(2) != 0)
#else
  #include <unistd.h>
  #define CPPLOG_ISATTY() (isatty(STDERR_FILENO) != 0)
#endif

namespace cpplog {

enum class Level { DEBUG = 0, INFO, WARN, ERROR, OFF };

namespace detail {

inline const char* level_str(Level l) {
    switch (l) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO:  return "INFO ";
        case Level::WARN:  return "WARN ";
        case Level::ERROR: return "ERROR";
        default:           return "     ";
    }
}

inline const char* level_color(Level l) {
    switch (l) {
        case Level::DEBUG: return "\033[36m";
        case Level::INFO:  return "\033[32m";
        case Level::WARN:  return "\033[33m";
        case Level::ERROR: return "\033[31m";
        default:           return "\033[0m";
    }
}

inline std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &t);
#else
    localtime_r(&t, &tm_buf);
#endif
    char buf[20];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm_buf);
    return buf;
}

template<typename T>
void format_into(std::ostringstream& ss, const std::string& fmt, size_t pos, T&& val) {
    size_t open = fmt.find('{', pos);
    if (open == std::string::npos) { ss << fmt.substr(pos); return; }
    ss << fmt.substr(pos, open - pos);
    size_t close = fmt.find('}', open);
    ss << val;
    if (close != std::string::npos) ss << fmt.substr(close + 1);
}

template<typename T, typename... Args>
void format_into(std::ostringstream& ss, const std::string& fmt, size_t pos, T&& val, Args&&... args) {
    size_t open = fmt.find('{', pos);
    if (open == std::string::npos) { ss << fmt.substr(pos); return; }
    ss << fmt.substr(pos, open - pos);
    size_t close = fmt.find('}', open);
    ss << val;
    if (close != std::string::npos)
        format_into(ss, fmt, close + 1, std::forward<Args>(args)...);
}

template<typename... Args>
std::string format(const std::string& fmt, Args&&... args) {
    if constexpr (sizeof...(args) == 0) {
        return fmt;
    } else {
        std::ostringstream ss;
        format_into(ss, fmt, 0, std::forward<Args>(args)...);
        return ss.str();
    }
}

inline const char* basename(const char* path) {
    const char* p = path;
    for (const char* c = path; *c; ++c)
        if (*c == '/' || *c == '\\') p = c + 1;
    return p;
}

} // namespace detail

class Logger {
public:
    std::atomic<Level> min_level{Level::DEBUG};
    std::atomic<bool> color;
    std::atomic<bool> show_time{true};
    std::ofstream file_out;

    static Logger& get() {
        static Logger instance;
        return instance;
    }

    void set_file(const std::string& path) {
        std::lock_guard<std::mutex> lock(mtx_);
        file_out.open(path, std::ios::app);
    }

    template<typename... Args>
    void log(Level level, const std::string& fmt, Args&&... args) {
        if (level < min_level.load()) return;
        std::string msg = detail::format(fmt, std::forward<Args>(args)...);
        std::string time_str = show_time.load() ? detail::timestamp() : "";

        std::lock_guard<std::mutex> lock(mtx_);
        write_header(level, time_str, nullptr, 0);
        std::cerr << " " << msg << "\n";

        if (file_out.is_open()) {
            file_out << "[" << detail::level_str(level) << "] " << time_str << " " << msg << "\n";
            file_out.flush();
        }
    }

    template<typename... Args>
    void log_at(Level level, const char* src_file, int src_line, const std::string& fmt, Args&&... args) {
        if (level < min_level.load()) return;
        std::string msg = detail::format(fmt, std::forward<Args>(args)...);
        std::string time_str = show_time.load() ? detail::timestamp() : "";

        std::lock_guard<std::mutex> lock(mtx_);
        write_header(level, time_str, src_file, src_line);
        std::cerr << " " << msg << "\n";

        if (file_out.is_open()) {
            file_out << "[" << detail::level_str(level) << "] "
                     << time_str << " "
                     << detail::basename(src_file) << ":" << src_line << " "
                     << msg << "\n";
            file_out.flush();
        }
    }

private:
    std::mutex mtx_;
    Logger() : color(CPPLOG_ISATTY()) {}

    void write_header(Level level, const std::string& time_str, const char* src_file, int src_line) {
        if (color.load()) {
            std::cerr << detail::level_color(level)
                      << "[" << detail::level_str(level) << "]"
                      << "\033[0m";
        } else {
            std::cerr << "[" << detail::level_str(level) << "]";
        }

        if (show_time.load()) {
            std::cerr << " ";
            if (color.load()) std::cerr << "\033[90m";
            std::cerr << time_str;
            if (color.load()) std::cerr << "\033[0m";
        }

        if (src_file) {
            std::cerr << " ";
            if (color.load()) std::cerr << "\033[90m";
            std::cerr << detail::basename(src_file) << ":" << src_line;
            if (color.load()) std::cerr << "\033[0m";
        }
    }
};

template<typename... Args> void debug(const std::string& fmt, Args&&... args) { Logger::get().log(Level::DEBUG, fmt, std::forward<Args>(args)...); }
template<typename... Args> void info (const std::string& fmt, Args&&... args) { Logger::get().log(Level::INFO,  fmt, std::forward<Args>(args)...); }
template<typename... Args> void warn (const std::string& fmt, Args&&... args) { Logger::get().log(Level::WARN,  fmt, std::forward<Args>(args)...); }
template<typename... Args> void error(const std::string& fmt, Args&&... args) { Logger::get().log(Level::ERROR, fmt, std::forward<Args>(args)...); }

inline void set_level(Level l)             { Logger::get().min_level = l; }
inline void set_color(bool on)             { Logger::get().color = on; }
inline void set_time(bool on)              { Logger::get().show_time = on; }
inline void set_file(const std::string& p) { Logger::get().set_file(p); }

} // namespace cpplog

#define CPPLOG_DEBUG(fmt, ...) ::cpplog::Logger::get().log_at(::cpplog::Level::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CPPLOG_INFO(fmt, ...)  ::cpplog::Logger::get().log_at(::cpplog::Level::INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CPPLOG_WARN(fmt, ...)  ::cpplog::Logger::get().log_at(::cpplog::Level::WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define CPPLOG_ERROR(fmt, ...) ::cpplog::Logger::get().log_at(::cpplog::Level::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
