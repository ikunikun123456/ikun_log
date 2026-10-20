
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
#ifndef IKUN_LOG_APPEND_H
#define IKUN_LOG_APPEND_H

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace ikun::log {

class StdoutAppend {
public:
  StdoutAppend() noexcept = default;
  ~StdoutAppend() noexcept = default;

  void Append(const char *msg, std::size_t len) const noexcept {
    std::size_t count = std::fwrite(msg, 1, len, stdout);
    if (count < len) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "log write failed";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  void Flush() const noexcept {
    int result = std::fflush(stdout);
    if (result == EOF) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "log flush failed";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  bool IsSuccessed() noexcept {
    if (stdout == nullptr) [[unlikely]] {
      return false;
    }
    return true;
  }
};

class FileAppend {
public:
  FileAppend(const char *name) noexcept {
    file_ = fopen(name, "a");
    if (file_ == nullptr) [[unlikely]] {
      file_ = stdout;
    }
    std::setvbuf(file_, nullptr, _IOFBF, 131072);
  }
  FileAppend(std::string_view name) noexcept : FileAppend(name.data()) {}

  FileAppend(const FileAppend &) = delete;
  FileAppend operator=(const FileAppend &) = delete;
  FileAppend(FileAppend &&other) noexcept : file_(other.file_) {
    other.file_ = nullptr; // ← 必须加这一句
  }
  FileAppend &operator=(FileAppend &&other) noexcept {
    if (this != &other) {
      file_ = other.file_;
      other.file_ = nullptr; // ← 同样必须有
    }
    return *this;
  }

  ~FileAppend() {
    if (file_ == stdout)
      return;
    if (file_ != nullptr)
      fclose(file_);
    file_ = nullptr;
  }

  void Append(const char *msg, std::size_t len) const noexcept {
    std::size_t count = std::fwrite(msg, 1, len, file_);
    if (count < len) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend write failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  void Flush() const noexcept {
    int result = std::fflush(file_);
    if (result == EOF) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend flush failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  bool IsSuccessed() noexcept {
    if (file_ == stdout) [[unlikely]] {
      return false;
    }
    return true;
  }

  void SetFile(const char *filename) noexcept {
    auto file = std::fopen(filename, "a");
    if (file == nullptr) [[unlikely]] {
      file = stdout;
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend flush failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
    if (file_ != nullptr && file_ != stdout) [[likely]] {
      int result = std::fclose(file_);
      if (result == EOF) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
        constexpr auto error_msg = "StdoutAppend flush failed\n";
        std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
      }
    }
    file_ = file;
    std::setvbuf(file_, nullptr, _IOFBF, 131072);
  }

private:
  FILE *file_;
};

class AsyncFileAppend {
public:
  AsyncFileAppend(const char *name) noexcept {
    file_ = fopen(name, "a");
    if (file_ == nullptr) [[unlikely]] {
      file_ = stdout;
    }
    std::setvbuf(file_, nullptr, _IOFBF, 131072);
  }
  AsyncFileAppend(std::string_view name) noexcept
      : AsyncFileAppend(name.data()) {}

  AsyncFileAppend(const AsyncFileAppend &) = delete;
  AsyncFileAppend operator=(const AsyncFileAppend &) = delete;
  AsyncFileAppend(AsyncFileAppend &&other) noexcept : file_(other.file_) {
    other.file_ = nullptr; // ← 必须加这一句
  }
  AsyncFileAppend &operator=(AsyncFileAppend &&other) noexcept {
    if (this != &other) {
      file_ = other.file_;
      other.file_ = nullptr; // ← 同样必须有
    }
    return *this;
  }

  ~AsyncFileAppend() {
    if (file_ == stdout)
      return;
    if (file_ != nullptr)
      fclose(file_);
    file_ = nullptr;
  }

  void Append(const char *msg, std::size_t len) const noexcept {
    std::size_t count = fwrite_unlocked(msg, 1, len, file_);
    if (count < len) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend write failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  void Flush() const noexcept {
    int result = std::fflush(file_);
    if (result == EOF) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend flush failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
  }

  bool IsSuccessed() noexcept {
    if (file_ == stdout) [[unlikely]] {
      return false;
    }
    return true;
  }

  void SetFile(const char *filename) noexcept {
    auto file = std::fopen(filename, "a");
    if (file == nullptr) [[unlikely]] {
      file = stdout;
#ifdef IKUN_LOG_USE_STDERR
      constexpr auto error_msg = "FileAppend flush failed\n";
      std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
    }
    if (file_ != nullptr && file_ != stdout) [[likely]] {
      int result = std::fclose(file_);
      if (result == EOF) [[unlikely]] {
#ifdef IKUN_LOG_USE_STDERR
        constexpr auto error_msg = "StdoutAppend flush failed\n";
        std::fwrite(error_msg, 1, std::strlen(error_msg), stderr);
#endif
      }
    }
    file_ = file;
    std::setvbuf(file_, nullptr, _IOFBF, 131072);
  }

private:
  FILE *file_;
};

} // namespace ikun::log

#endif