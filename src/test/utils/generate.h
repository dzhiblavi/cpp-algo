#ifndef ALGO_TEST_GENERATE_H
#define ALGO_TEST_GENERATE_H

#include <random>
#include <concepts>
#include <functional>

#include "range_query.h"

namespace algo::test {

std::mt19937& generator();

namespace detail_ {

template <std::integral T, size_t NDims, size_t I>
test::multivector<T, NDims - I> generate(auto& gen, const size_t (&dims)[NDims]) {
  test::multivector<T, NDims - I> v(dims[I]);
  if constexpr (I == NDims - 1) {
    std::generate(v.begin(), v.end(), gen);
  } else {
    for (size_t i = 0; i < dims[I]; ++i) {
      v[i] = generate<T, NDims, I + 1>(gen, dims);
    }
  }
  return v;
}

}  // namespace detail_

template <std::integral T, size_t NDims>
test::multivector<T, NDims> generate(T l, T r, const size_t (&dims)[NDims]) {
  std::uniform_int_distribution<T> distr(l, r);
  auto gen = std::bind(distr, std::ref(generator()));
  return detail_::generate<T, NDims, 0>(gen, dims);
}

template <size_t NDims>
void random_index(size_t (&idxs)[NDims], const size_t (&dims)[NDims]) {
  for (size_t i = 0; i < NDims; ++i) {
    idxs[i] = std::uniform_int_distribution<size_t>(0, dims[i] - 1)(generator());
  }
}

template <size_t NDims>
void random_range(size_t (&left)[NDims], size_t (&right)[NDims], const size_t (&dims)[NDims]) {
  for (size_t i = 0; i < NDims; ++i) {
    left[i] = std::uniform_int_distribution<size_t>(0, dims[i] - 1)(generator());
    right[i] = std::uniform_int_distribution<size_t>(left[i], dims[i] - 1)(generator());
  }
}

}  // namespace algo::test

#endif  // ALGO_TEST_GENERATE_H

