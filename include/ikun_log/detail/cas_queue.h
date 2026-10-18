/*  Multi-producer/multi-consumer bounded queue.
 *  Copyright (c) 2010-2011, Dmitry Vyukov. All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. THIS
 * SOFTWARE IS PROVIDED BY DMITRY VYUKOV "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL DMITRY VYUKOV OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. The views and conclusions
 * contained in the software and documentation are those of the authors and
 * should not be interpreted as representing official policies, either expressed
 * or implied, of Dmitry Vyukov.
 */
#ifndef CAS_QUEUE_H
#define CAS_QUEUE_H

#include "ikun_log/detail/cpuwait.h"
#include <ikun_log/detail/backoff.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace ikun::log::detail {

#ifndef IKUN_LOG_CACHE_LINE_SIZE
constexpr std::int64_t CacheLine = 64;
#else
constexpr std::int64_t CacheLine = IKUN_LOG_CACHE_LINE_SIZE;
#endif

template <typename T, int capacity> class CasQueue {
private:
  static_assert((capacity & (capacity - 1)) == 0, "capacity must be power 2");
  static_assert(std::is_move_constructible_v<T>,
                "T must be move constructible");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T must be move constructible no throw exception");

  struct alignas(CacheLine * 2) Slot {
    std::atomic<int64_t> _sequence{0};
    alignas(T) std::array<std::byte, sizeof(T)> _value;
    T *data() { return std::launder(reinterpret_cast<T *>(_value.data())); }
  };

  alignas(CacheLine * 2) std::atomic<int64_t> _dequeue_pos{0};
  alignas(CacheLine * 2) std::atomic<int64_t> _enqueue_pos{0};

  alignas(CacheLine * 2) std::array<Slot, capacity> _buffer;

public:
  CasQueue() {
    for (int i = 0; i < capacity; i++) {
      _buffer[i]._sequence.store(i, std::memory_order_relaxed);
    }
  }

  ~CasQueue() {
    auto front = _dequeue_pos.load(std::memory_order_relaxed);
    auto back = _enqueue_pos.load(std::memory_order_relaxed);

    while (front != back) {
      auto index = front & (capacity - 1);
      if (_buffer[index]._sequence.load(std::memory_order_acquire) ==
          front + 1) {
        std::destroy_at(_buffer[index].data());
      }
      ++front;
    }
  }

  template <typename... Args> bool emplace(Args &&...args) {
    auto pos = _enqueue_pos.load(std::memory_order_relaxed);
    CpuWait cpuwait;
    Slot *slot;

    while (true) {
      slot = &_buffer[pos & (capacity - 1)];
      auto _slot_pos = slot->_sequence.load(std::memory_order_acquire);
      auto diff = _slot_pos - pos;
      if (diff == 0) {
        if (_enqueue_pos.compare_exchange_weak(pos, pos + 1,
                                               std::memory_order_relaxed)) {
          break;
        }
        cpuwait.wait();
      } else if (diff < 0) {
        return false;
      } else {
        pos = _enqueue_pos.load(std::memory_order_relaxed);
        cpuwait.wait();
      }
    }

    std::construct_at(slot->data(), std::forward<Args>(args)...);
    slot->_sequence.store(pos + 1, std::memory_order_release);
    return true;
  }

  bool pop(T &item) {
    auto pos = _dequeue_pos.load(std::memory_order_relaxed);
    CpuWait cpuwait;
    Slot *slot;

    while (true) {
      slot = &_buffer[pos & (capacity - 1)];
      auto _slot_pos = slot->_sequence.load(std::memory_order_acquire);
      auto diff = _slot_pos - pos;
      if (diff == 1) {
        if (_dequeue_pos.compare_exchange_weak(pos, pos + 1,
                                               std::memory_order_relaxed)) {
          break;
        }
        cpuwait.wait();
      } else if (diff == 0) {
        return false;
      } else {
        pos = _dequeue_pos.load(std::memory_order_relaxed);
        cpuwait.wait();
      }
    }
    item = std::move(*(slot->data()));
    std::destroy_at(slot->data());
    slot->_sequence.store(pos + capacity, std::memory_order_release);
    return true;
  }

  // 获取当前队列大小
  [[nodiscard]] int64_t size() const noexcept {
    int64_t head = _dequeue_pos.load(std::memory_order_relaxed);
    int64_t tail = _enqueue_pos.load(std::memory_order_relaxed);
    return head - tail;
  }

  // 检查是否为空
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }
};
} // namespace ikun::log::detail

#endif