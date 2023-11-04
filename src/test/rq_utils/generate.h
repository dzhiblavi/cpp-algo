#pragma once

#include "algo/utility/nd_container.h"
#include "test/utility/random.h"

#include <concepts>
#include <functional>
#include <random>

namespace test::rq_utils {

template <size_t Dims>
void generatePoint(std::array<int, Dims>& a) {
  for (auto& x : a) {
    x = utility::random::uniform(-1000, 1000);
  }
}

template <size_t Dims>
std::vector<std::array<int, Dims>> generatePoints(size_t count) {
  std::vector<std::array<int, Dims>> r(count);
  for (auto& a : r) {
    generatePoint(a);
  }
  return r;
}

namespace detail {

template <std::integral T, size_t NDims, size_t I>
algo::utility::NDVector<T, NDims - I> generate(auto& gen, const std::array<size_t, NDims>& dims) {
  algo::utility::NDVector<T, NDims - I> v(dims[I]);
  if constexpr (I == NDims - 1) {
    std::generate(v.begin(), v.end(), gen);
  } else {
    for (size_t i = 0; i < dims[I]; ++i) {
      v[i] = generate<T, NDims, I + 1>(gen, dims);
    }
  }
  return v;
}

}  // namespace detail

template <typename T, size_t NDims>
algo::utility::NDVector<T, NDims> shapeAs(algo::utility::NDView<T, NDims> auto from) {
  algo::utility::NDVector<T, NDims> v;
  v.resize(from.frontDimension());

  if constexpr (NDims > 1) {
    for (size_t i = 0; i < from.frontDimension(); ++i) {
      v[i] = shapeAs<T, NDims - 1>(from.at(i));
    }
  }
  return v;
}

template <typename T, size_t NDims>
void copyView(algo::utility::NDView<T, NDims> auto from, algo::utility::NDView<T, NDims> auto to) {
  assert(from.frontDimension() == to.frontDimension());
  const size_t size = from.frontDimension();

  if constexpr (NDims == 1) {
    for (size_t i = 0; i < size; ++i) {
      to.at(i) = from.at(i);
    }
  } else {
    for (size_t i = 0; i < size; ++i) {
      copyView<T, NDims - 1>(from.at(i), to.at(i));
    }
  }
}

template <std::integral T, size_t NDims>
auto generate(T l, T r, const std::array<size_t, NDims>& dims) {
  std::uniform_int_distribution<T> distr(l, r);
  auto gen = std::bind(distr, std::ref(utility::random::generator()));
  return detail::generate<T, NDims, 0>(gen, dims);
}

template <size_t NDims>
void randomIndex(std::array<size_t, NDims>& idxs, const std::array<size_t, NDims>& dims) {
  for (size_t i = 0; i < NDims; ++i) {
    idxs[i] = std::uniform_int_distribution<size_t>(0, dims[i] - 1)(utility::random::generator());
  }
}

template <size_t NDims>
void randomRange(
    std::array<size_t, NDims>& left, std::array<size_t, NDims>& right, const std::array<size_t, NDims>& dims) {
  for (size_t i = 0; i < NDims; ++i) {
    left[i] = std::uniform_int_distribution<size_t>(0, dims[i] - 1)(utility::random::generator());
    right[i] = std::uniform_int_distribution<size_t>(left[i], dims[i] - 1)(utility::random::generator());
  }
}

}  // namespace test::rq_utils
