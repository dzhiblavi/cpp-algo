#pragma once

#include "algo/graph/repr/adj_list/adj_list.h"
#include "algo/graph/repr/adj_matrix/adj_matrix.h"
#include "test/utility/type_list.h"

namespace algo::graph::util {

template <std::unsigned_integral T>
constexpr bool contains(T set, T element) noexcept {
  return set & element;
}

struct GraphKind {
  using UnderlyingType = uint32_t;

  static constexpr UnderlyingType kAdjMatrix = 1 << 0;
  static constexpr UnderlyingType kAdjList = 1 << 1;
  static constexpr UnderlyingType kAll = ~UnderlyingType(0);
};

struct DirectKind {
  using UnderlyingType = uint8_t;

  static constexpr UnderlyingType kDirect = 1 << 0;
  static constexpr UnderlyingType kIndirect = 1 << 1;
  static constexpr UnderlyingType kAll = ~UnderlyingType(0);
};

namespace detail {

template <
    GraphKind::UnderlyingType Kind,  //
    bool Direct,                     //
    typename StorageType>
using ResolveKind = list::list<
    std::conditional_t<
        contains(Kind, GraphKind::kAdjMatrix),
        repr::AdjMatrixGraph<uint32_t, repr::empty_payload_t, StorageType, Direct, std::size_t>, int>,
    std::conditional_t<
        contains(Kind, GraphKind::kAdjList), repr::AdjListGraph<uint32_t, repr::empty_payload_t, Direct, std::size_t>,
        int>>;

template <
    GraphKind::UnderlyingType Kind,  //
    bool Direct,                     //
    std::size_t StorageReserve>
using ResolveStorage = list::concat<
    ResolveKind<Kind, Direct, repr::StaticStorage<StorageReserve>>,  //
    ResolveKind<Kind, Direct, repr::DynamicStorage<0>>,              //
    ResolveKind<Kind, Direct, repr::DynamicStorage<StorageReserve>>>;

template <
    GraphKind::UnderlyingType Kind,     //
    DirectKind::UnderlyingType Direct,  //
    std::size_t StorageReserve>
using ResolveDirect = list::concat<
    std::conditional_t<contains(Direct, DirectKind::kDirect), ResolveStorage<Kind, true, StorageReserve>, list::list<>>,
    std::conditional_t<
        contains(Direct, DirectKind::kIndirect), ResolveStorage<Kind, false, StorageReserve>, list::list<>>>;

}  // namespace detail

template <
    GraphKind::UnderlyingType Kind,     //
    DirectKind::UnderlyingType Direct,  //
    std::size_t StorageReserve>
using GraphTypes = list::filter<int, detail::ResolveDirect<Kind, Direct, StorageReserve>>;

}  // namespace algo::graph::util