#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace di::detail {

template <typename Exclude, size_t Tag>
struct AnyType {
  template <typename T>
  requires(!std::is_same_v<std::decay_t<T>, Exclude>)
  operator T();
};

template <typename T, size_t N>
consteval bool isNArgsConstructible() {
  return []<size_t... Indices>(std::index_sequence<Indices...>) {
    return std::is_constructible_v<T, AnyType<T, Indices>...>;
  }(std::make_index_sequence<N>());
}

template <typename T, size_t N = 10>
consteval size_t getNumConstructorArguments() {
  if constexpr (N == 0) {
    static_assert(std::is_constructible_v<T>, "Object is not constructible from under 10 arguments");
    return 0;
  } else if constexpr (isNArgsConstructible<T, N>()) {
    return N;
  } else {
    return getNumConstructorArguments<T, N - 1>();
  }
}

}  // namespace di::detail
