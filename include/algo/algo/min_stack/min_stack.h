#pragma once

#include <list>
#include <stack>

namespace algo::mstack {

struct AutomaticStrategy {};

template <typename ContainerHolder, bool IndirectMinStorage>
struct ForceContainerStrategy {
  using Container = typename ContainerHolder::template type<int>;

  static_assert(
      !IndirectMinStorage || std::is_same_v<std::list<int>, Container> || std::is_same_v<std::deque<int>, Container>,
      "Indirect min storage can be used with std::list or std::deque only");
};

template <template <typename, typename> typename ContainerTemplate, typename... FixArgs>
struct AdaptContainer {
  template <typename T>
  using type = ContainerTemplate<T, FixArgs...>;
};

namespace detail {

template <typename T, typename Compare, typename Strategy>
class StackElement;

template <typename T, typename Compare, typename ContainerHolder>
class StackElement<T, Compare, ForceContainerStrategy<ContainerHolder, false>> {
 public:
  using ContainerHolderType = ContainerHolder;

  explicit StackElement(const T& value) : value_{value}, min_value_{value} {}

  StackElement(const T& value, const T& old_min_value, Compare& comp)
      : value_{value}, min_value_{std::min(value, old_min_value, comp)} {}

  const T& min() const noexcept {
    return min_value_;
  }
  T& value() noexcept {
    return value_;
  }
  const T& value() const noexcept {
    return value_;
  }

 private:
  T value_;
  T min_value_;
};

template <typename T, typename Compare, typename ContainerHolder>
class StackElement<T, Compare, ForceContainerStrategy<ContainerHolder, true>> {
 public:
  using ContainerHolderType = ContainerHolder;

  explicit StackElement(const T& value) : value_{value}, min_value_ptr_{&value_} {}

  StackElement(const T& value, const T& old_min_value, Compare& comp)
      : value_{value}, min_value_ptr_{comp(value, old_min_value) ? &value_ : &old_min_value} {}

  const T& min() const noexcept {
    return *min_value_ptr_;
  }
  T& value() noexcept {
    return value_;
  }
  const T& value() const noexcept {
    return value_;
  }

 private:
  T value_;
  const T* min_value_ptr_;
};

template <typename T, typename Compare, typename Strategy>
struct ChooseStackElement {
  using type = StackElement<T, Compare, Strategy>;
};

template <typename T, typename Compare>
requires(sizeof(T) <= 8 && std::is_copy_constructible_v<T>)
struct ChooseStackElement<T, Compare, AutomaticStrategy> {
  using type = StackElement<T, Compare, ForceContainerStrategy<AdaptContainer<std::deque>, false>>;
};

template <typename T, typename Compare>
requires(sizeof(T) > 8 || !std::is_copy_constructible_v<T>)
struct ChooseStackElement<T, Compare, AutomaticStrategy> {
  using type = StackElement<T, Compare, ForceContainerStrategy<AdaptContainer<std::deque>, true>>;
};

}  // namespace detail

template <typename T, typename Compare = std::less<T>, typename Strategy = AutomaticStrategy>
class MinStack {
  using StackElement = typename detail::ChooseStackElement<T, Compare, Strategy>::type;
  using ContainerType = typename StackElement::ContainerHolderType::template type<StackElement>;

 public:
  MinStack() noexcept = default;

  void push(const T& value) {
    if (stack_.empty()) {
      stack_.emplace_back(value);
    } else {
      stack_.emplace_back(value, min(), comp_);
    }
  }

  void pop() noexcept {
    stack_.pop_back();
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    push(T{std::forward<Args>(args)...});
  }

  [[nodiscard]] T& top() noexcept {
    return front().value();
  }
  [[nodiscard]] const T& top() const noexcept {
    return front().value();
  }
  [[nodiscard]] const T& min() const noexcept {
    return front().min();
  }

  void reserve(size_t capacity) {
    stack_.reserve(capacity);
  }
  [[nodiscard]] size_t size() const noexcept {
    return stack_.size();
  }
  [[nodiscard]] bool isEmpty() const noexcept {
    return size() == 0;
  }

 private:
  StackElement& front() noexcept {
    return stack_.back();
  }
  const StackElement& front() const noexcept {
    return stack_.back();
  }

  [[no_unique_address]] Compare comp_;
  ContainerType stack_;
};

}  // namespace algo::mstack