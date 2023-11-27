#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace util {

template <auto Start, auto End, auto Inc, typename F>
constexpr void constexprFor(F&& f) {
  if constexpr (Start < End) {
    f(std::integral_constant<decltype(Start), Start>());
    constexprFor<Start + Inc, End, Inc>(std::forward<F>(f));
  }
}

template <typename F, typename... Args>
constexpr void constexprFor(F&& f, Args&&... args) {
  (f(std::forward<Args>(args)), ...);
}

template <typename F, typename TupleLike>
constexpr void constexprFor(F&& f, TupleLike&& tuple) {
  constexpr size_t size = std::tuple_size_v<TupleLike>;
  constexprFor<size_t{0}, size, size_t{1}>([&](auto i) { f(std::get<i.value>(tuple)); });
}

}  // namespace util