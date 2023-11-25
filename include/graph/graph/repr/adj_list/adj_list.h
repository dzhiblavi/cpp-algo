#pragma once

#include "graph/repr/concepts.h"
#include "graph/repr/traits.h"

#include <algorithm>
#include <cassert>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/transform.hpp>
#include <vector>

namespace algo::graph::repr {

template <typename NodeIdType, typename EdgePayloadType, bool Direct, std::integral SizeType>
class AdjListGraph : public ContinuousNodeIndexTag<true>, public DirectTag<Direct> {
 public:
  using NodeId = NodeIdType;
  using EdgePayload = EdgePayloadType;

  AdjListGraph() noexcept = default;

  auto edgesFrom(NodeId from) noexcept {
    assert(from < nodeCount());
    return ranges::views::all(lists_[from]);
  }

  auto edgesFrom(NodeId from) const noexcept {
    assert(from < nodeCount());
    return ranges::views::all(lists_[from]);
  }

  auto allEdges() const noexcept {
    return nodes(*this) |                                                               //
           ranges::views::transform([this](NodeId from) { return edgesFrom(from); }) |  //
           ranges::views::join;
  }

  void addNodes(size_t count) noexcept { lists_.resize(nodeCount() + count); }

  template <typename... Args>
  requires std::is_constructible_v<EdgePayload, Args...>
  void addEdge(NodeId from, NodeId to, Args&&... args) {
    assert(!hasEdge(from, to));
    ++edge_count_;

    if constexpr (Direct) {
      lists_[from].emplace_back(std::piecewise_construct, std::forward_as_tuple(to), std::forward_as_tuple(args...));
    } else {
      lists_[from].emplace_back(std::piecewise_construct, std::forward_as_tuple(to), std::tuple(args...));
      lists_[to].emplace_back(std::piecewise_construct, std::forward_as_tuple(from), std::tuple(args...));
    }
  }

  // void removeEdge(NodeId from, NodeId to) noexcept {
  //   assert(hasEdge(from, to));
  //   // self().get(from, to) = std::nullopt;
  //   --edge_count_;
  // }

  [[nodiscard]] bool hasEdge(NodeId from, NodeId to) const noexcept {
    assert(from < nodeCount() && to < nodeCount());
    return lists_[from].end() != ranges::find(lists_[from], to, [](const auto& edge) { return std::get<0>(edge); });
  }

  // [[nodiscard]] EdgePayload& getEdge(NodeId from, NodeId to) noexcept {
  //   assert(hasEdge(from, to));
  //   return *self().get(from, to);
  // }

  // [[nodiscard]] const EdgePayload& getEdge(NodeId from, NodeId to) const noexcept {
  //   assert(hasEdge(from, to));
  //   return *self().get(from, to);
  // }

  [[nodiscard]] size_t nodeCount() const noexcept { return lists_.size(); }
  [[nodiscard]] size_t edgeCount() const noexcept { return edge_count_; }

  auto transposedView() {
    if constexpr (!Direct) {
      return *this;
    } else {
      AdjListGraph g;
      g.addNodes(nodeCount());

      for (auto node : nodes(*this)) {
        for (auto [to, edge] : edgesFrom(node)) {
          g.addEdge(to, node, edge);
        }
      }

      return g;
    }
  }

 private:
  size_t edge_count_{0};
  std::vector<std::vector<std::pair<NodeId, EdgePayloadType>>> lists_;
};

}  // namespace algo::graph::repr