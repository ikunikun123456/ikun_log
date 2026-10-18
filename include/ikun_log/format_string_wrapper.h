
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

#ifndef FORMAT_STRING_WRAPPER_H
#define FORMAT_STRING_WRAPPER_H

#include "ikun_log/detail/filename.h"
#include <cstring>
#include <source_location>
#include <stdint.h>
#include <string_view>

namespace ikun::log {

struct format_string_wrapper {
  consteval format_string_wrapper(
      const char *str, detail::FixString<30> loc = detail::StringFromLocation(
                           std::source_location::current()))
      : str_{str}, loc_{loc} {
    len_ = std::char_traits<char>::length(str);
  }
  operator std::string_view() const { return {str_, len_}; }

  std::string_view get() const { return str_; }

  const char *str_;
  detail::FixString<30> loc_;
  std::size_t len_;
};

}; // namespace ikun::log

#endif