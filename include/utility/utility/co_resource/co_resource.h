#pragma once

#include <coroutine>
#include <memory>
#include <utility>

namespace util {

namespace detail {

template <typename T>
class PromiseType;

}  // namespace detail

template <typename T>
class CoResource {
  friend class detail::PromiseType<T>;
  using handle_type = std::coroutine_handle<detail::PromiseType<T>>;

 public:
  using promise_type = detail::PromiseType<T>;

  CoResource(const CoResource&) = delete;
  CoResource& operator=(const CoResource&) = delete;

  CoResource(CoResource&& rhs) noexcept : handle_{std::exchange(rhs.handle_, nullptr)} {}

  CoResource& operator=(CoResource&& rhs) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(rhs));
    return *this;
  }

  ~CoResource() noexcept {
    if (!handle_) {
      return;
    }

    handle_.resume();
    handle_.destroy();
  }

 private:
  explicit CoResource(handle_type handle) noexcept : handle_{std::move(handle)} {}

  handle_type handle_;
};

namespace detail {

template <typename T>
class PromiseType {
  using handle_type = typename CoResource<T>::handle_type;

 public:
  constexpr std::suspend_never initial_suspend() const noexcept {
    return {};
  }

  constexpr std::suspend_always final_suspend() const noexcept {
    return {};
  }

  constexpr std::suspend_always yield_value(const T& value) noexcept {
    value_ = &value;
    return {};
  }

  constexpr void return_void() const noexcept {}

  [[nodiscard]] constexpr auto get_return_object() noexcept {
    return CoResource<T>(handle_type::from_promise(*this));
  }

  void unhandled_exception() noexcept {
    // TODO(dzhiblavi@)
  }

 private:
  const T* value_ = nullptr;
};

}  // namespace detail

}  // namespace util
