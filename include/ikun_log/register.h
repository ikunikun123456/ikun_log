
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
#ifndef REGISTER_H
#define REGISTER_H

#include <ikun_log/append/stdout_append.h>
#include <ikun_log/async_logger.h>
#include <ikun_log/log_level.h>
#include <ikun_log/logger.h>

#include <cstdio>

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
}; // namespace ikun::log

#endif