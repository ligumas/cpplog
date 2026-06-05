#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>

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
        case Level::DEBUG: return "\033[36m";   // cyan
        case Level::INFO:  return "\033[32m";   // green
        case Level::WARN:  return "\033[33m";   // yellow
        case Level::ERROR: return "\033[31m";   // red
        default:           return "\033[0m";
    }
}

inline std::string timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&t));
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

} // namespace detail

class Logger {
public:
    Level min_level = Level::DEBUG;
    bool color       = true;
    bool show_time   = true;
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
        if (level < min_level) return;
        std::string msg = detail::format(fmt, std::forward<Args>(args)...);
        std::string time_str = show_time ? detail::timestamp() : "";

        std::lock_guard<std::mutex> lock(mtx_);

        if (color) {
            std::cerr << detail::level_color(level)
                      << "[" << detail::level_str(level) << "]"
                      << "\033[0m";
        } else {
            std::cerr << "[" << detail::level_str(level) << "]";
        }

        if (show_time) std::cerr << " \033[90m" << time_str << "\033[0m";
        std::cerr << " " << msg << "\n";

        if (file_out.is_open())
            file_out << "[" << detail::level_str(level) << "] " << time_str << " " << msg << "\n";
    }

private:
    std::mutex mtx_;
    Logger() = default;
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
