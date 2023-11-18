#pragma once

#include "graph/repr/concepts.h"

#include <ranges>
#include <unordered_map>
#include <vector>

namespace algo::graph::repr {

template <Graph G>
decltype(auto) nodes(const G& graph) noexcept(noexcept(graph.nodes())) {
  return graph.nodes();
}

template <ContinuousNodeIndexGraph G>
auto nodes(const G& graph) noexcept {
  using NodeId = typename G::NodeId;
  return std::views::iota(NodeId{0}, static_cast<NodeId>(graph.nodeCount()));
}

template <typename To, Graph G>
auto mapNodes(const G& graph) {
  std::unordered_map<typename G::NodeId, To> map;
  map.reserve(graph.nodeCount());
  return map;
}

template <typename To, ContinuousNodeIndexGraph G>
auto mapNodes(const G& graph) {
  std::vector<To> map(graph.nodeCount());
  map.resize(graph.nodeCount());
  return map;
}

template <typename To, Graph G>
auto mapNodes(const G& graph, const To& initial) {
  auto map = mapNodes(graph);
  for (auto node_id : nodes(graph)) {
    map.emplace(node_id, initial);
  }
  return map;
}

template <typename To, ContinuousNodeIndexGraph G>
auto mapNodes(const G& graph, const To& initial) {
  auto map = mapNodes<To>(graph);
  std::fill(map.begin(), map.end(), initial);
  return map;
}

}  // namespace algo::graph::repr