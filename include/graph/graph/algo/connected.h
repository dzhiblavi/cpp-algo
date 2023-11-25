#pragma once

#include "graph/algo/top_sort.h"
#include "graph/repr/traits.h"

#include <cassert>
#include <vector>

namespace algo::graph::algo {

namespace detail {

template <typename Graph>
void connectedComponentsDfs(typename Graph::NodeId node_id, const Graph& graph, auto& used_map, auto& component) {
  assert(!used_map[node_id]);
  used_map[node_id] = true;
  component.push_back(node_id);

  for (auto [to_id, _] : graph.edgesFrom(node_id)) {
    if (used_map[to_id]) {
      continue;
    }

    connectedComponentsDfs(to_id, graph, used_map, component);
  }
}

}  // namespace detail

template <repr::IndirectGraph G>
std::vector<std::vector<typename G::NodeId>> connectedComponents(const G& graph) {
  using NodeId = typename G::NodeId;

  auto used_map = repr::mapNodes<bool>(graph);
  std::vector<std::vector<NodeId>> components;

  for (NodeId node_id : repr::nodes(graph)) {
    if (used_map[node_id]) {
      continue;
    }

    components.emplace_back();
    detail::connectedComponentsDfs(node_id, graph, used_map, components.back());
  }

  return components;
}

template <repr::DirectGraph G, repr::DirectGraph Gt>
std::vector<std::vector<typename G::NodeId>> stronglyConnectedComponents(const G& graph, const Gt& graph_transposed) {
  using NodeId = typename G::NodeId;

  auto used_map = repr::mapNodes<bool>(graph_transposed);
  std::vector<std::vector<NodeId>> components;

  for (NodeId node_id : topologicalSort(graph)) {
    if (used_map[node_id]) {
      continue;
    }

    components.emplace_back();
    detail::connectedComponentsDfs(node_id, graph_transposed, used_map, components.back());
  }

  return components;
}

}  // namespace algo::graph::algo