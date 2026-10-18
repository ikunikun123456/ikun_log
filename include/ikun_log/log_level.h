
/*
MIT License

Copyright (c) 2026 firstestikun firstestikun@outlook.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

#include <array>
#include <iterator>
#include <string_view>

namespace ikun::log {
enum class Loglevel {
  TRACE = 0,
  DEBUG = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  OFF = 5,
  UNKNOWN = 6
};

namespace {
constexpr std::array<std::string_view, 7> arr{
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "OFF", "UNKNOWN"};
}

inline constexpr std::string_view
StringFromLoglevel(Loglevel loglevel) noexcept {
  auto idx = static_cast<std::size_t>(loglevel);
  return arr[idx];
}

inline constexpr Loglevel
LoglevelFromString(std::string_view loglevel) noexcept {
  for (std::size_t i = 0; auto &str : arr) {
    if (i >= arr.size()) {
      return static_cast<Loglevel>(i - 1);
    }
    if (loglevel == str) {
      return static_cast<Loglevel>(i);
    }

    i++;
  }
  auto size = std::size(arr);
  return static_cast<Loglevel>(size - 1);
}

} // namespace ikun::log

#if defined(IKUN_LOG_ACTIVE_LEVEL_TRACE)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::TRACE
#elif defined(IKUN_LOG_ACTIVE_LEVEL_DEBUG)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::DEBUG
#elif defined(IKUN_LOG_ACTIVE_LEVEL_WARN)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::WARN
#elif defined(IKUN_LOG_ACTIVE_LEVEL_ERROR)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::ERROR
#else
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::INFO
#endif

#endif