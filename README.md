<div align="center">

# cpplog

minimal header-only logging for C++17

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![header-only](https://img.shields.io/badge/header--only-yes-brightgreen?style=flat-square)
![license](https://img.shields.io/badge/license-MIT-blue?style=flat-square)
![CI](https://github.com/ligumas/cpplog/actions/workflows/ci.yml/badge.svg)

</div>

drop one header into your project and get colored, leveled, formatted logging.

```cpp
#include "cpplog/log.hpp"

cpplog::info("server started on port {}", 8080);
cpplog::warn("memory at {}%", 87);
cpplog::error("connection failed: {}", "timeout");
cpplog::debug("loaded {} entries", count);
```

output in terminal (color-coded by level):

```
[INFO ] 14:02:11 server started on port 8080
[WARN ] 14:02:11 memory at 87%
[ERROR] 14:02:11 connection failed: timeout
[DEBUG] 14:02:11 loaded 42 entries
```

## install

copy `include/cpplog/log.hpp` into your project.

or with CMake FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(cpplog
    GIT_REPOSITORY https://github.com/ligumas/cpplog.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cpplog)
target_link_libraries(your_target cpplog)
```

## usage

```cpp
#include "cpplog/log.hpp"

cpplog::debug("x = {}", x);
cpplog::info("ready");
cpplog::warn("high load: {}%", load);
cpplog::error("failed: {}", msg);

cpplog::set_level(cpplog::Level::WARN);  // filter below WARN
cpplog::set_color(false);                // disable ANSI (e.g. piped output)
cpplog::set_file("app.log");             // also write to file
```

use the `CPPLOG_*` macros when you want file and line in the output:

```cpp
CPPLOG_INFO("loading {} samples", n);
CPPLOG_WARN("slow response: {}ms", ms);
CPPLOG_ERROR("disk full");
```

```
[INFO ] 14:02:11 main.cpp:12 loading 500 samples
[WARN ] 14:02:11 server.cpp:87 slow response: 340ms
[ERROR] 14:02:11 disk.cpp:203 disk full
```

## features

- single header, zero dependencies
- C++17, Linux/Windows/macOS
- `{}` placeholder formatting
- log levels: DEBUG INFO WARN ERROR
- colored terminal output (ANSI)
- optional file output
- thread-safe
- `CPPLOG_*` macros for file:line source location

## build / test

```bash
cmake -B build && cmake --build build
./build/tests
```

**License:** MIT
