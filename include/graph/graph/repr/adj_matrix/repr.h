#pragma once

#include "graph/repr/storage_type.h"

#include <array>
#include <concepts>
#include <optional>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

namespace algo::graph::repr {

namespace detail {

template <typename NodeId, typename EdgePayload, typename Self>
class AdjMatrixGraph {
 public:
  auto edgesFrom(NodeId from) noexcept {
    assert(from < self().nodeCount());

    return ranges::views::iota(NodeId{0}, static_cast<NodeId>(self().nodeCount())) |
           ranges::views::filter([this, from](NodeId to) { return hasEdge(from, to); }) |
           ranges::views::transform([this, from](NodeId to) {
             return std::make_pair(to, std::ref(getEdge(from, to)));
           });
  }

  auto edgesFrom(NodeId from) const noexcept {
    assert(from < self().nodeCount());

    return ranges::views::iota(NodeId{0}, static_cast<NodeId>(self().nodeCount())) |
           ranges::views::filter([this, from](NodeId to) { return hasEdge(from, to); }) |
           ranges::views::transform([this, from](NodeId to) {
             return std::make_pair(to, std::ref(getEdge(from, to)));
           });
  }

  auto allEdges() const noexcept {
    return ranges::views::iota(NodeId{0}, static_cast<NodeId>(self().nodeCount())) |
           ranges::views::transform([this](NodeId from) { return edgesFrom(from); }) |  //
           ranges::views::join;
  }

  void addNodes(size_t count) noexcept {
    self().resize(self().nodeCount() + count);
  }

  template <typename... Args>
  requires std::is_constructible_v<EdgePayload, Args...>
  void addEdge(NodeId from, NodeId to, Args&&... args) {
    assert(!hasEdge(from, to));
    ++self().edgeCountMutable();
    self().get(from, to).emplace(std::forward<Args>(args)...);
  }

  void removeEdge(NodeId from, NodeId to) noexcept {
    assert(hasEdge(from, to));
    self().get(from, to) = std::nullopt;
    --self().edgeCountMutable();
  }

  [[nodiscard]] bool hasEdge(NodeId from, NodeId to) const noexcept {
    assert(from < self().nodeCount() && to < self().nodeCount());
    return self().get(from, to).has_value();
  }

  [[nodiscard]] EdgePayload& getEdge(NodeId from, NodeId to) noexcept {
    assert(hasEdge(from, to));
    return *self().get(from, to);
  }

  [[nodiscard]] const EdgePayload& getEdge(NodeId from, NodeId to) const noexcept {
    assert(hasEdge(from, to));
    return *self().get(from, to);
  }

 private:
  Self& self() noexcept {
    return *static_cast<Self*>(this);
  }
  const Self& self() const noexcept {
    return *static_cast<const Self*>(this);
  }
};

}  // namespace detail

}  // namespace algo::graph::repr