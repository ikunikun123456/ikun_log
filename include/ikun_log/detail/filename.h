
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
#ifndef FILENAME_H
#define FILENAME_H

#include <array>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

namespace ikun::log::detail {

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

inline std::string FilenameFromCurrenttime() {
  try {
    auto path = std::string("./");
    auto temp_path = std::getenv("IKUN_LOG_DIR");
    if (temp_path != nullptr) {
      path = temp_path;
    }
    std::array<char, 30> local_time{};
    std::format_to_n(local_time.begin(), 19, "{:%Y-%m-%d_%H-%M-%S}",
                     std::chrono::system_clock::now());
    memcpy(local_time.begin() + 19, ".txt", 4);
    return path + local_time.data();
  } catch (const std::exception &e) {
#ifdef IKUN_LOG_USE_STDERR
    constexpr auto error_msg = "filename create failed";
    std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
    std::fwrite(e.what(), 1, std::strlen(e.what()), stderr);
    std::array<char, 50> time_arr{"./async_log.txt"};
    return std::string(time_arr.data());
#endif
  }
}
} // namespace ikun::log::detail

#endif