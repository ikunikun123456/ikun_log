
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
#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <cstring>
#include <ikun_log/detail/buffer.h>
#include <ikun_log/detail/tsc_to_ns.h>
#include <ikun_log/log_level.h>
#include <source_location>

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

consteval std::array<std::array<char, 4>, 1000> GetTimeArr() {
  std::array<std::array<char, 4>, 1000> a{};
  for (int i = 0; i < 1000; ++i) {
    a[i][0] = char('0' + i / 100);
    a[i][1] = char('0' + (i / 10) % 10);
    a[i][2] = char('0' + i % 10);
    a[i][3] = '\0';
  }
  return a;
}

constexpr std::array<std::array<char, 4>, 1000> time_arr = GetTimeArr();

inline static void GetNs(std::size_t num, Buffer &buf) {
  int count = num % 1000;
  buf.arr[buf.size + 6] = time_arr[count][0];
  buf.arr[buf.size + 7] = time_arr[count][1];
  buf.arr[buf.size + 8] = time_arr[count][2];
  num /= 1000;
  count = num % 1000;
  buf.arr[buf.size + 3] = time_arr[count][0];
  buf.arr[buf.size + 4] = time_arr[count][1];
  buf.arr[buf.size + 5] = time_arr[count][2];
  num /= 1000;
  count = num % 1000;
  buf.arr[buf.size] = time_arr[count][0];
  buf.arr[buf.size + 1] = time_arr[count][1];
  buf.arr[buf.size + 2] = time_arr[count][2];
  buf.arr[buf.size + 9] = '\t';
  buf.size += 10;
}

inline void Format_(Buffer &buf, Loglevel loglevel,
                    const detail::FixString<30> &loc) {
  static thread_local detail::TscToNs tsc_to_ns{};
  static thread_local std::array<char, 20> local_time{};
  static thread_local std::size_t old_time_point = 0;
  // 计算当前时间
  std::size_t time_point = tsc_to_ns.GetNs();
  auto temp_time_point = time_point - old_time_point;
  // 更新秒时间
  if (temp_time_point >= 1'000'000'000) [[unlikely]] {
    temp_time_point %= 1'000'000'000;
    old_time_point = time_point - temp_time_point;
    auto temp_local_time =
        std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    std::format_to_n(local_time.begin(), 20, "{:%Y-%m-%d %H:%M:%S}",
                     temp_local_time);
  }
  std::memcpy(buf.arr.data() + buf.size, local_time.data(), local_time.size());
  buf.size += local_time.size();
  // 纳秒时间
  GetNs(temp_time_point, buf);
  // 线程id
  static thread_local std::array<char, 24> thread_id{};
  static thread_local auto thread_id_size = [&]() {
    auto r =
        std::to_chars(thread_id.begin(), thread_id.end() - 1,
                      std::hash<std::thread::id>{}(std::this_thread::get_id()));
    *r.ptr = '\t';
    return std::strlen(thread_id.data());
  }();
  std::memcpy(buf.arr.data() + buf.size, thread_id.data(), thread_id_size);
  buf.size += thread_id_size;
  // 日志水平
  auto loglevelstr = StringFromLoglevel(loglevel);
  std::memcpy(buf.arr.data() + buf.size, loglevelstr.data(),
              loglevelstr.size());
  buf.size += loglevelstr.size();
  // 文件名字和行号
  std::memcpy(buf.arr.data() + buf.size, loc.str.data(), loc.size);
  buf.size += loc.size;
}
}; // namespace ikun::log::detail
#endif