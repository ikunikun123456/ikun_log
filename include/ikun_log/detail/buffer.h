
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
#ifndef BUFFER_H
#define BUFFER_H

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ikun_log/detail/filename.h>
#include <ikun_log/log_level.h>

#include <string_view>
#include <vector>

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

namespace ikun::log::detail {

namespace {

constexpr int BUFFER_SIZE = 470;

}; // namespace

struct Buffer {
  Buffer() noexcept = default;
  Buffer(Buffer &&other) noexcept {
    if (!other.vec.empty()) [[unlikely]] {
      vec = std::move(other.vec);
      size = other.size;
      other.size = 0;
      return;
    }
    size = other.size;
    other.size = 0;
    std::memcpy(arr.data(), other.arr.data(), size);
  }
  Buffer &operator=(Buffer &&other) noexcept {
    if (this == &other) [[unlikely]]
      return *this;
    if (!other.vec.empty()) [[unlikely]] {
      vec = std::move(other.vec);
      size = other.size;
      other.size = 0;
      return *this;
    }
    vec.clear();
    size = other.size;
    other.size = 0;
    std::memcpy(arr.data(), other.arr.data(), size);
    return *this;
  };

  inline std::string_view Data() const noexcept {
    if (vec.empty()) [[likely]] {
      return {arr.data(), size};
    }
    return {vec.data(), size};
  }

  inline void reset() noexcept {
    size = 0;
    if (!vec.empty()) [[unlikely]]
      vec.clear();
  }

  std::vector<char> vec{};
  std::size_t size = 0;
  std::array<char, BUFFER_SIZE> arr;
};

} // namespace ikun::log::detail

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif