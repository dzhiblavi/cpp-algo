#pragma once

#include <deque>

namespace algo::utility {

template <typename T, bool FIFO>
class Queue {
 public:
  Queue() noexcept = default;

  void push(const T& value) {
    if constexpr (FIFO) {
      deque_.push_front(value);
    } else {
      deque_.push_back(value);
    }
  }

  T pop() noexcept {
    auto value = deque_.back();
    deque_.pop_back();
    return value;
  }

  [[nodiscard]] size_t size() const noexcept { return deque_.size(); }
  [[nodiscard]] bool empty() const noexcept { return deque_.empty(); }

 private:
  std::deque<T> deque_;
};

}  // namespace algo::utility