#pragma once

#include <vector>

namespace algo::utility {

namespace detail {

template <typename C>
struct CountDimensions;

template <typename C>
static constexpr size_t countDimensions = CountDimensions<std::decay_t<C>>::value;

template <typename C>
struct GetValueType;

template <typename C>
struct GetValueType<const C> {
  using type = std::add_const_t<typename GetValueType<C>::type>;
};

template <typename C>
using getValueType = typename GetValueType<std::remove_reference_t<C>>::type;

// vector spec
template <typename T>
struct CountDimensions<std::vector<T>> : std::integral_constant<size_t, 1> {};

template <typename C>
struct CountDimensions<std::vector<std::vector<C>>>
    : std::integral_constant<size_t, 1 + CountDimensions<std::vector<C>>::value> {};

template <typename T>
struct GetValueType<std::vector<T>> {
  using type = T;
};

template <typename C>
struct GetValueType<std::vector<std::vector<C>>> {
  using type = typename GetValueType<std::vector<C>>::type;
};

template <typename T, size_t NDims>
struct NDVectorHolder {
  using type = std::vector<typename NDVectorHolder<T, NDims - 1>::type>;
};

template <typename T>
struct NDVectorHolder<T, 1> {
  using type = std::vector<T>;
};

template <typename T>
struct NDVectorHolder<T, 0> {
  using type = T;
};

}  // namespace detail

template <typename T, size_t NDims>
using NDVector = typename detail::NDVectorHolder<T, NDims>::type;

}  // namespace algo::utility
