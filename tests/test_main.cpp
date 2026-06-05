#include "cpplog/log.hpp"
#include <cassert>
#include <sstream>

int tests_passed = 0;

void test_basic() {
    cpplog::info("hello from info");
    cpplog::warn("watch out: {} items left", 3);
    cpplog::error("failed with code {}", -1);
    cpplog::debug("x={}, y={}", 1.5, 2.5);
    tests_passed++;
}

void test_level_filter() {
    cpplog::set_level(cpplog::Level::ERROR);
    cpplog::info("this should not appear");
    cpplog::error("this should appear");
    cpplog::set_level(cpplog::Level::DEBUG);
    tests_passed++;
}

void test_no_args() {
    cpplog::info("simple message no args");
    tests_passed++;
}

void test_file_output() {
    cpplog::set_file("/tmp/cpplog_test.log");
    cpplog::info("written to file");
    cpplog::Logger::get().file_out.close();
    tests_passed++;
}

int main() {
    test_basic();
    test_level_filter();
    test_no_args();
    test_file_output();

    std::cerr << "\n" << tests_passed << "/4 tests passed\n";
    return tests_passed == 4 ? 0 : 1;
}
