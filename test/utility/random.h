#pragma once

#include <random>

namespace test::utility::random {

std::mt19937& generator();

template <std::integral T>
[[nodiscard]] T uniform(T l, T r) noexcept {
  return std::uniform_int_distribution<T>(l, r)(generator());
}

template <std::floating_point T>
[[nodiscard]] T uniform(T l, T r) noexcept {
  return std::uniform_real_distribution<T>(l, r)(generator());
}

}  // namespace test::utility::random