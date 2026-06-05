#include "cpplog/log.hpp"

int main() {
    cpplog::info("server starting on port {}", 8080);
    cpplog::debug("loaded {} config entries", 12);
    cpplog::warn("memory usage at {}%", 87);
    cpplog::error("connection refused: {}", "timeout");

    cpplog::set_level(cpplog::Level::WARN);
    cpplog::info("this won't show (below WARN)");
    cpplog::warn("but this will");

    return 0;
}
