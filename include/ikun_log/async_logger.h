
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
#ifndef ASYNC_LOGGER_H
#define ASYNC_LOGGER_H

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <format>
#include <ikun_log/append/file_append.h>
#include <ikun_log/detail/buffer.h>
#include <ikun_log/detail/cas_queue.h>
#include <ikun_log/detail/tsc_to_ns.h>
#include <ikun_log/detail/util.h>
#include <ikun_log/log_level.h>
#include <ikun_log/type.h>
#include <source_location>
#include <string_view>
#include <thread>

#ifndef IKUN_LOG_QUEUE_CAPACITY
static constexpr int QUEUE_CAPACITY = 8192;
#else
static constexpr int QUEUE_CAPACITY = IKUN_LOG_QUEUE_CAPACITY;
#endif

namespace ikun::log {
template <Append T> class AsyncLogger {
public:
  AsyncLogger(const char *name, Loglevel loglevel, T append) noexcept
      : name_(name), loglevel_(loglevel), append_(std::move(append)),
        stop_(false) {
    thread_ = std::thread(&AsyncLogger::ThreadFunc, this);
  }

  ~AsyncLogger() {
    stop_.store(true, std::memory_order::relaxed);
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  template <class... Args>
    requires(sizeof...(Args) >= 1)
  inline void Trace(format_string_wrapper msg, Args &&...args) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
      LogFormat(msg, Loglevel::TRACE, msg.loc_, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
    requires(sizeof...(Args) >= 1)
  inline void Debug(format_string_wrapper msg, Args &&...args) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
      LogFormat(msg, Loglevel::DEBUG, msg.loc_, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
    requires(sizeof...(Args) >= 1)
  inline void Info(format_string_wrapper msg, Args &&...args) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
      LogFormat(msg, Loglevel::INFO, msg.loc_, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
    requires(sizeof...(Args) >= 1)
  inline void Warn(format_string_wrapper msg, Args &&...args) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
      LogFormat(msg, Loglevel::WARN, msg.loc_, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
    requires(sizeof...(Args) >= 1)
  inline void Error(format_string_wrapper msg, Args &&...args) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
      LogFormat(msg, Loglevel::ERROR, msg.loc_, std::forward<Args>(args)...);
    }
  }
  inline void Trace(std::string_view msg,
                    detail::FixString<30> loc = detail::StringFromLocation(
                        std::source_location::current())) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
      Log(msg, Loglevel::TRACE, loc);
    }
  }

  inline void Debug(std::string_view msg,
                    detail::FixString<30> loc = detail::StringFromLocation(
                        std::source_location::current())) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
      Log(msg, Loglevel::DEBUG, loc);
    }
  }

  inline void Info(std::string_view msg,
                   detail::FixString<30> loc = detail::StringFromLocation(
                       std::source_location::current())) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
      Log(msg, Loglevel::INFO, loc);
    }
  }

  inline void Warn(std::string_view msg,
                   detail::FixString<30> loc = detail::StringFromLocation(
                       std::source_location::current())) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
      Log(msg, Loglevel::WARN, loc);
    }
  }

  inline void Error(std::string_view msg,
                    detail::FixString<30> loc = detail::StringFromLocation(
                        std::source_location::current())) noexcept {
    if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
      Log(msg, Loglevel::ERROR, loc);
    }
  }

  void Flush() noexcept { return append_.flush(); }

  void SetLoglevel(Loglevel loglevel) noexcept { loglevel_ = loglevel; }

  Loglevel GetLoglevel() noexcept { return loglevel_; }

  void Log(std::string_view msg, Loglevel loglevel,
           const detail::FixString<30> &loc) noexcept {
    return Log_(msg.data(), msg.size(), loglevel, loc);
  }

  template <class... Args>
  void LogFormat(std::string_view msg, Loglevel loglevel,
                 const detail::FixString<30> &loc, Args &&...args) noexcept {
    return LogFormat_(msg, loglevel, loc, std::forward<Args>(args)...);
  }

private:
  template <class... Args>
  void LogFormat_(std::string_view msg, Loglevel loglevel,
                  const detail::FixString<30> &loc, Args &&...args) noexcept {
    static thread_local std::string str;
    try {
      [[maybe_unused]] static thread_local bool result = [&]() {
        str.reserve(350);
        return true;
      }();
      // 消息格式化
      std::vformat_to(std::back_inserter(str), msg,
                      std::make_format_args(args...));
      Log_(str.data(), str.length(), loglevel, loc);
    } catch (const std::exception &e) {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "LogFormat format failed";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
      std::fwrite(e.what(), 1, std::strlen(e.what()), stderr);
#endif
    }
    str.clear();
  }

  void Log_(const char *msg, std::size_t len, Loglevel loglevel,
            const detail::FixString<30> &loc) noexcept {
    static thread_local detail::Buffer buf;
    try {
      detail::CpuWait cpuwait;
      // 进行格式化
      detail::Format_(buf, loglevel, loc);
      // 消息
      if (buf.size + len + 1 < buf.arr.size()) [[likely]] {
        std::memcpy(buf.arr.data() + buf.size, msg, len);
        buf.arr[buf.size + len] = '\n';
      } else {
        buf.vec.resize(buf.size + len + 10);
        std::memcpy(buf.vec.data(), buf.arr.data(), buf.size);
        std::memcpy(buf.vec.data() + buf.size, msg, len);
        buf.vec[buf.size + len] = '\n';
      }
      buf.size += len + 1;

      while (true) {
        auto result = queue_.emplace(std::move(buf));
        if (result) [[likely]] {
          buf.reset();
          return;
        }
        cpuwait.wait();
      }

    } catch (const std::exception &e) {
      buf.reset();
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "format failed";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
      std::fwrite(e.what(), 1, std::strlen(e.what()), stderr);
#endif
    }
  }

  bool Shouldlog(Loglevel loglevel) const noexcept {
    if (loglevel >= loglevel_) {
      return true;
    }
    return false;
  }

  void ThreadFunc() {
    using namespace std::literals::chrono_literals;
    std::size_t count = 0;
    detail::Backoff backoff;
    detail::Buffer buf;
    while (!stop_.load(std::memory_order::relaxed)) [[likely]] {
      for (; count < count + 20000; count++) {
        auto result = queue_.pop(buf);
        if (!result) [[unlikely]] {
          for (int i = 0; i < 8; i++) {
            backoff.pause();
            if (!queue_.empty()) [[likely]] {
              continue;
            }
          }
          if (!queue_.empty()) [[likely]] {
            backoff.reset();
            continue;
          }
          if constexpr (std::is_same_v<T, FileAppend>) {
            if (count > 10000) [[unlikely]] {
              append_.SetFile(detail::FilenameFromCurrenttime().c_str());
            }
          }
          while (queue_.empty() && !stop_.load()) [[unlikely]] {
            std::this_thread::sleep_for(1ms);
          }
          break;
        }
        auto buf_view = buf.Data();
        append_.Append(buf_view.data(), buf_view.size());
      }
    }
    std::this_thread::sleep_for(100ms);
    while (true) {
      auto result = queue_.pop(buf);
      if (!result) {
        break;
      }
      auto buf_view = buf.Data();
      append_.Append(buf_view.data(), buf_view.size());
    }
    append_.Flush();
  }

  std::string name_;
  Loglevel loglevel_;
  T append_;
  std::atomic<bool> stop_;
  detail::CasQueue<detail::Buffer, QUEUE_CAPACITY> queue_;
  std::thread thread_;
};
}; // namespace ikun::log

#endif