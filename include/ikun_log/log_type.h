
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

#ifndef IKUN_LOG_LOG_TYPE_H
#define IKUN_LOG_LOG_TYPE_H

#include <array>
#include <source_location>
#include <string_view>

template <typename T>
concept Appendable = requires(T &t, const char *data, std::size_t len) {
  { t.Append(data, len) } -> std::same_as<void>;
  { t.Flush() } -> std::same_as<void>;
};

template <typename T>
concept FileAppendable = Appendable<T> && requires(T &t, const char *filename) {
  { t.SetFile(filename) } -> std::same_as<void>;
};

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

template <int N> struct FixString {
  std::array<char, N> str;
  int size;
};

inline consteval std::string_view BaseName(std::string_view path) noexcept {
  auto pos = path.find_last_of("/\\");
  path = pos == std::string_view::npos ? path : path.substr(pos + 1);
  if (path.size() > 20) {
    return path.substr(0, 20);
  }
  return path;
}

inline consteval FixString<20> StringFromStdsize(std::size_t num) noexcept {
  std::array<char, 10> nums{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 20> str{};
  int i = 0;
  while (true) {
    int count = num % 10;
    str[i] = nums.at(count);
    num = num / 10;
    if (num == 0) {
      break;
    }
    i++;
  }
  int temp = i;
  std::array<char, 20> new_str{};
  for (int j = 0; j <= temp; j++, i--) {
    new_str[j] = str[i];
  }
  temp++;
  if (temp >= 8) {
    temp = 7;
  }
  return {new_str, temp};
}

inline consteval FixString<30>
StringFromLocation(const std::source_location &location) {
  std::string_view filename = BaseName(location.file_name());
  FixString<20> str = StringFromStdsize(location.line());
  std::array<char, 30> new_str{'\t'};
  int i = 1;
  for (; i <= static_cast<int>(filename.size()); i++) {
    new_str[i] = filename.at(i - 1);
  }
  new_str[i++] = ':';
  for (int j = 0; i < str.size + static_cast<int>(filename.size()) + 2;) {
    new_str[i++] = str.str[j++];
  }
  new_str[i++] = '\t';
  return {new_str, i};
};

struct format_string_wrapper {
  consteval format_string_wrapper(
      const char *str,
      FixString<30> loc = StringFromLocation(std::source_location::current()))
      : str_{str}, loc_{loc} {
    len_ = std::char_traits<char>::length(str);
  }
  operator std::string_view() const { return {str_, len_}; }

  std::string_view get() const { return str_; }

  const char *str_;
  FixString<30> loc_;
  std::size_t len_;
};

} // namespace ikun::log

#endif