#pragma once

#include "algo/min_stack/min_stack.hpp"

namespace algo::mstack {

template <typename T, typename Compare = std::less<T>, typename Strategy = AutomaticStrategy>
class MinQueue {
 public:
  MinQueue() noexcept = default;

  void push(const T& value) { push_stack_.push(value); }
  void push(T&& value) { push_stack_.push(std::move(value)); }

  template <typename... Args>
  void emplace(Args&&... args) {
    push_stack_.emplace(std::forward<Args>(args)...);
  }

  void pop() noexcept {
    flushPushIfPopIsEmpty();
    pop_stack_.pop();
  }

  [[nodiscard]] T& top() noexcept {
    flushPushIfPopIsEmpty();
    return pop_stack_.top();
  }

  [[nodiscard]] const T& top() const noexcept {
    flushPushIfPopIsEmpty();
    return pop_stack_.top();
  }

  [[nodiscard]] const T& min() const noexcept {
    if (push_stack_.isEmpty()) {
      return pop_stack_.min();
    }
    if (pop_stack_.isEmpty()) {
      return push_stack_.min();
    }
    return std::min(pop_stack_.min(), push_stack_.min(), comp_);
  }

  void reserve(size_t capacity) {
    push_stack_.reserve(capacity);
    pop_stack_.reserve(capacity);
  }

  [[nodiscard]] size_t size() const noexcept { return push_stack_.size() + pop_stack_.size(); }
  [[nodiscard]] size_t isEmpty() const noexcept { return size() == 0; }

 private:
  void flushPushIfPopIsEmpty() const {
    if (!pop_stack_.isEmpty()) {
      return;
    }

    while (!push_stack_.isEmpty()) {
      pop_stack_.push(std::move(push_stack_.top()));
      push_stack_.pop();
    }
  }

  [[no_unique_address]] Compare comp_;
  mutable MinStack<T, Compare, Strategy> push_stack_;
  mutable MinStack<T, Compare, Strategy> pop_stack_;
};

}  // namespace algo::mstack