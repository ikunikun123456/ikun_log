
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
#ifndef IKUN_LOG_UTIL_H
#define IKUN_LOG_UTIL_H

#include <array>
#include <cstring>
#include <ikun_log/detail_buffer.h>
#include <ikun_log/detail_tscns.h>
#include <ikun_log/log_type.h>

namespace ikun::log::detail {

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

inline void Format_(Buffer &buf, Loglevel loglevel, const FixString<30> &loc) {
  static thread_local detail::Tscns tscns{};
  static thread_local std::array<char, 20> local_time{};
  static thread_local std::size_t old_time_point = 0;
  // 计算当前时间
  std::size_t time_point = tscns.GetNs();
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