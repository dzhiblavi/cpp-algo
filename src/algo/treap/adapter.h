#ifndef ALGO_TREAP_ADAPTER_H
#define ALGO_TREAP_ADAPTER_H

#include <concepts>
#include <cstddef>
#include <utility>

namespace algo::treap {

template <typename T>
class value_adapter {
 public:
  using ValueType = T;

  template <typename... Args> requires std::constructible_from<ValueType, Args...>
  explicit value_adapter(Args&&... args)
    noexcept(std::is_nothrow_constructible_v<ValueType, Args...>)
    : value_(std::forward<Args>(args)...) {}

  T& Value() noexcept { return value_; };
  const T& Value() const noexcept { return value_; };

 private:
  ValueType value_;
};

template <typename SizeType = size_t>
class size_adapter {
 public:
  using SelfT = size_adapter<SizeType>;

  size_adapter() noexcept = default;

  SizeType Size() const noexcept { return size_; }

 protected:
  void UpdateAdapter(const SelfT* left, const SelfT* right) noexcept {
    size_ = 1;
    if (left) size_ += left->Size();
    if (right) size_ += right->Size();
  }

 private:
  SizeType size_ = 1;
};

}  // namespace algo::treap

#endif  // ALGO_TREAP_ADAPTER_H

