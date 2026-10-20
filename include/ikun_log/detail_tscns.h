
/*
MIT License

Copyright (c) 2022 Meng Rao <raomeng1@gmail.com>

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
// changed 2026 firstestikun firstestikun@outlook.com
#ifndef IKUN_LOG_DETAIL_TSCNS_H
#define IKUN_LOG_DETAIL_TSCNS_H

#include <chrono>
#include <thread>

namespace ikun::log::detail {

class Tscns {
public:
  Tscns(int64_t init_calibrate_ns = 20'000'000,
        int64_t internal_calibrate_ns = 10'000'000'000) {
    internal_cailbrate_ns_ = internal_calibrate_ns;
    SyncTime(base_tsc_, base_ns_);
    int64_t expired_ns = base_ns_ + init_calibrate_ns;
    while (GetSysNs() < expired_ns)
      std::this_thread::yield();
    int64_t cur_tsc;
    int64_t cur_ns;
    SyncTime(cur_tsc, cur_ns);
    ns_per_tsc_ =
        static_cast<double>(cur_ns - base_ns_) / (cur_tsc - base_tsc_);
    next_cailbrate_tsc_ = cur_tsc + internal_cailbrate_ns_ * ns_per_tsc_;
  }
  ~Tscns() {}

  int64_t GetNs() {
#if defined(_M_AMD64) || defined(__i386__) || defined(__x86_64__)
    int64_t tsc = GetTsc();
    if (tsc > next_cailbrate_tsc_) [[unlikely]] {
      calibrate();
    }
    return base_ns_ + static_cast<int64_t>((tsc - base_tsc_) * ns_per_tsc_);
#else
    return GetSysNs()
#endif
  }

  void calibrate() {
    int64_t cur_tsc;
    int64_t cur_ns;
    SyncTime(cur_tsc, cur_ns);
    int64_t ns_err = (cur_tsc - base_tsc_) * ns_per_tsc_ - cur_ns;
    if (ns_err > 1000000)
      ns_err = 1000000;
    if (ns_err < -1000000)
      ns_err = -1000000;
    ns_per_tsc_ =
        ns_per_tsc_ * (1.0 - (ns_err + ns_err - base_ns_err_) /
                                 ((cur_tsc - base_tsc_) * ns_per_tsc_));
    next_cailbrate_tsc_ = cur_tsc + internal_cailbrate_ns_ * ns_per_tsc_;
    base_ns_ = cur_ns;
    base_tsc_ = cur_tsc;
    base_ns_err_ = ns_err;
  }

private:
  void SyncTime(int64_t &tsc, int64_t &ns) {
#ifdef _MSC_VER
    constexpr int N = 32;
#else
    constexpr int N = 8;
#endif
    int64_t tsc_arr[N];
    int64_t ns_arr[N];
    for (int i = 0; i < N; i++) {
      ns_arr[i] = GetSysNs();
      tsc_arr[i] = GetTsc();
    }

#ifdef _MSC_VER
    int j = 1;
    for (int i = 2; i < N; i++) {
      if (ns_arr[i] == ns_arr[i - 1])
        continue;
      tsc_arr[j - 1] = tsc_arr[i - 1];
      ns_arr[j++] = ns_arr[i];
    }
    j--;
#else
    int j = N - 2;
#endif

    int best = 1;
    for (int i = 2; i < j; i++) {
      if (tsc_arr[i] - tsc_arr[i - 1] < tsc_arr[best] - tsc_arr[best])
        best = i;
    }
    tsc = (tsc_arr[best] + tsc_arr[best - 1]) / 2;
    ns = ns_arr[best];
    return;
  }

private:
  int64_t GetTsc() {
#if defined(_MSC_VER) && defined(_M_AMD64)
    return __rdtsc()
#elif defined(__i386__) || defined(__x86_64__)
    return __builtin_ia32_rdtsc();
#else
    return GetSysNs()
#endif
  }

  int64_t GetSysNs() {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch())
        .count();
  }

  int64_t internal_cailbrate_ns_;
  int64_t base_tsc_;
  int64_t base_ns_;
  double ns_per_tsc_;
  int64_t next_cailbrate_tsc_;
  int64_t base_ns_err_ = 0;
};
}; // namespace ikun::log::detail

#endif