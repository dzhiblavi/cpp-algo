#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace algo::graph::repr {

struct empty_payload_t {};

[[maybe_unused]] constexpr empty_payload_t empty_payload{};

template <bool StaticStorage, size_t MaxSize>
struct StorageType {
  static constexpr bool kIsStatic = StaticStorage;
  static constexpr size_t kMaxSize = MaxSize;
};

template <size_t Reserved = 0>
using DynamicStorage = StorageType<false, Reserved>;

template <size_t MaxSize>
using StaticStorage = StorageType<true, MaxSize>;

}  // namespace algo::graph::repr