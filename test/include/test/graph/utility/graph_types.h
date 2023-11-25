#pragma once

#include "graph/repr/adj_list/adj_list.h"
#include "graph/repr/adj_matrix/adj_matrix.h"
#include "utility/templates.h"
#include "utility/type_list.h"

namespace test::graph::util {

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

namespace list = ::util::list;

template <
    GraphKind::UnderlyingType Kind,  //
    bool Direct,                     //
    typename StorageType>
using ResolveKind = list::concat<
    std::conditional_t<
        contains(Kind, GraphKind::kAdjMatrix),
        list::list<algo::graph::repr::AdjMatrixGraph<
            uint32_t, algo::graph::repr::empty_payload_t, StorageType, Direct, std::size_t>>,
        list::list<>>,
    std::conditional_t<
        contains(Kind, GraphKind::kAdjList),
        list::list<algo::graph::repr::AdjListGraph<uint32_t, algo::graph::repr::empty_payload_t, Direct, std::size_t>>,
        list::list<>>>;

template <
    GraphKind::UnderlyingType Kind,  //
    bool Direct,                     //
    std::size_t StorageReserve>
using ResolveStorage = list::concat<
    ResolveKind<Kind, Direct, algo::graph::repr::StaticStorage<StorageReserve>>,  //
    ResolveKind<Kind, Direct, algo::graph::repr::DynamicStorage<0>>,              //
    ResolveKind<Kind, Direct, algo::graph::repr::DynamicStorage<StorageReserve>>>;

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
using GraphTypes = detail::ResolveDirect<Kind, Direct, StorageReserve>;

}  // namespace test::graph::util