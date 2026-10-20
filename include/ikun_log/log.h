

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
#ifndef IKUN_LOG_LOG_H
#define IKUN_LOG_LOG_H

#include <ikun_log/append.h>
#include <ikun_log/async_logger.h>
#include <ikun_log/detail_util.h>
#include <ikun_log/log_type.h>
#include <ikun_log/logger.h>

namespace ikun::log {

class Register {
public:
  static Register &GetInstance() noexcept {
    static Register instance;
    return instance;
  }

  Logger<StdoutAppend> &GetDefaultLog() noexcept {
    static auto default_log =
        Logger<StdoutAppend>("default", Loglevel::INFO, StdoutAppend());
    return default_log;
  }

  AsyncLogger<AsyncFileAppend> &GetDefaultAsyncLog() noexcept {
    static auto default_async_log = AsyncLogger<AsyncFileAppend>(
        "async_default", Loglevel::INFO,
        AsyncFileAppend(detail::FilenameFromCurrenttime().c_str()));
    return default_async_log;
  }

private:
  Register() noexcept {}
};

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void Trace(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
    Register::GetInstance().GetDefaultLog().LogFormat(
        msg, Loglevel::TRACE, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void Debug(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
    Register::GetInstance().GetDefaultLog().LogFormat(
        msg, Loglevel::DEBUG, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void Info(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
    Register::GetInstance().GetDefaultLog().LogFormat(
        msg, Loglevel::INFO, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void Warn(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
    Register::GetInstance().GetDefaultLog().LogFormat(
        msg, Loglevel::WARN, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void Error(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
    Register::GetInstance().GetDefaultLog().LogFormat(
        msg, Loglevel::ERROR, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void AsyncTrace(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
    if constexpr (sizeof...(args) > 0) {
      Register::GetInstance().GetDefaultAsyncLog().LogFormat(
          msg, Loglevel::TRACE, msg.loc_, std::forward<Args>(args)...);
    } else {
      Register::GetInstance().GetDefaultAsyncLog().Log(
          msg.get(), Loglevel::TRACE, msg.loc_);
    }
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void AsyncDebug(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
    Register::GetInstance().GetDefaultAsyncLog().LogFormat(
        msg, Loglevel::DEBUG, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void AsyncInfo(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
    Register::GetInstance().GetDefaultAsyncLog().LogFormat(
        msg, Loglevel::INFO, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void AsyncWarn(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
    Register::GetInstance().GetDefaultAsyncLog().LogFormat(
        msg, Loglevel::WARN, msg.loc_, std::forward<Args>(args)...);
  }
}

template <class... Args>
  requires(sizeof...(Args) >= 1)
inline void AsyncError(format_string_wrapper msg, Args &&...args) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
    Register::GetInstance().GetDefaultAsyncLog().LogFormat(
        msg, Loglevel::ERROR, msg.loc_, std::forward<Args>(args)...);
  }
}

inline void Trace(std::string_view msg,
                  FixString<30> loc = StringFromLocation(
                      std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
    Register::GetInstance().GetDefaultLog().Log(msg, Loglevel::TRACE, loc);
  }
}

inline void Debug(std::string_view msg,
                  FixString<30> loc = StringFromLocation(
                      std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
    Register::GetInstance().GetDefaultLog().Log(msg, Loglevel::DEBUG, loc);
  }
}

inline void Info(std::string_view msg,
                 FixString<30> loc = StringFromLocation(
                     std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
    Register::GetInstance().GetDefaultLog().Log(msg, Loglevel::INFO, loc);
  }
}

inline void Warn(std::string_view msg,
                 FixString<30> loc = StringFromLocation(
                     std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
    Register::GetInstance().GetDefaultLog().Log(msg, Loglevel::WARN, loc);
  }
}

inline void Error(std::string_view msg,
                  FixString<30> loc = StringFromLocation(
                      std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
    Register::GetInstance().GetDefaultLog().Log(msg, Loglevel::ERROR, loc);
  }
}

inline void AsyncTrace(std::string_view msg,
                       FixString<30> loc = StringFromLocation(
                           std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::TRACE) {
    Register::GetInstance().GetDefaultAsyncLog().Log(msg, Loglevel::TRACE, loc);
  }
}

inline void AsyncDebug(std::string_view msg,
                       FixString<30> loc = StringFromLocation(
                           std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::DEBUG) {
    Register::GetInstance().GetDefaultAsyncLog().Log(msg, Loglevel::DEBUG, loc);
  }
}

inline void AsyncInfo(std::string_view msg,
                      FixString<30> loc = StringFromLocation(
                          std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::INFO) {
    Register::GetInstance().GetDefaultAsyncLog().Log(msg, Loglevel::INFO, loc);
  }
}

inline void AsyncWarn(std::string_view msg,
                      FixString<30> loc = StringFromLocation(
                          std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::WARN) {
    Register::GetInstance().GetDefaultAsyncLog().Log(msg, Loglevel::WARN, loc);
  }
}

inline void AsyncError(std::string_view msg,
                       FixString<30> loc = StringFromLocation(
                           std::source_location::current())) noexcept {
  if constexpr (IKUN_LOG_ACTIVE_LEVEL <= Loglevel::ERROR) {
    Register::GetInstance().GetDefaultAsyncLog().Log(msg, Loglevel::ERROR, loc);
  }
}

} // namespace ikun::log

#endif