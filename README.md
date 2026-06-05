<div align="center">

# cpplog

**Minimal header-only logging for C++17**

![Language](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square)
![Header only](https://img.shields.io/badge/header--only-yes-brightgreen?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-blue?style=flat-square)
![Zero dependencies](https://img.shields.io/badge/dependencies-none-brightgreen?style=flat-square)

</div>

---

Drop one header into your project and get colored, leveled, formatted logging in under a minute.

```cpp
#include "cpplog/log.hpp"

cpplog::info("server started on port {}", 8080);
cpplog::warn("memory at {}%", 87);
cpplog::error("connection failed: {}", "timeout");
cpplog::debug("loaded {} entries", count);
```

Output:

```
[INFO ] 14:02:11 server started on port 8080
[WARN ] 14:02:11 memory at 87%
[ERROR] 14:02:11 connection failed: timeout
[DEBUG] 14:02:11 loaded 42 entries
```

(color-coded by level in terminal)

---

## Install

Copy `include/cpplog/log.hpp` into your project. That's it.

Or with CMake FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(cpplog
    GIT_REPOSITORY https://github.com/ligumas/cpplog.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cpplog)
target_link_libraries(your_target cpplog)
```

---

## Usage

```cpp
#include "cpplog/log.hpp"

// log levels: debug < info < warn < error
cpplog::debug("x = {}", x);
cpplog::info("ready");
cpplog::warn("high load: {}%", load);
cpplog::error("failed: {}", msg);

// filter — only show WARN and above
cpplog::set_level(cpplog::Level::WARN);

// disable color (e.g. for piped output)
cpplog::set_color(false);

// also write to file
cpplog::set_file("app.log");
```

---

## Features

- Single header, zero dependencies
- C++17, works on Linux, Windows, macOS
- `{}` placeholder formatting
- Log levels: DEBUG, INFO, WARN, ERROR
- Colored terminal output (ANSI)
- Optional file output
- Thread-safe

## Build examples / tests

```bash
cmake -B build
cmake --build build
./build/tests
./build/example_basic
```

---

## License

MIT
