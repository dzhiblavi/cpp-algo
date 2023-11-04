#pragma once

#include "algo/graph/repr/traits.h"

#include <algorithm>
#include <stack>
#include <vector>

namespace algo::graph::algo {

namespace detail {

template <repr::DirectGraph G>
void topologicalSortDfs(typename G::NodeId node_id, const G& graph, auto& used_map, auto& order) {
  assert(!used_map[node_id]);
  used_map[node_id] = true;

  for (auto [to_id, _] : graph.edgesFrom(node_id)) {
    if (used_map[to_id]) {
      continue;
    }

    topologicalSortDfs(to_id, graph, used_map, order);
  }

  order.push_back(node_id);
}

}  // namespace detail

template <repr::DirectGraph G>
std::vector<typename G::NodeId> topologicalSort(const G& graph) {
  using NodeId = typename G::NodeId;

  auto used_map = repr::mapNodes<bool>(graph);

  std::vector<NodeId> order;
  order.reserve(graph.nodeCount());

  for (NodeId node_id : repr::nodes(graph)) {
    if (used_map[node_id]) {
      continue;
    }

    detail::topologicalSortDfs(node_id, graph, used_map, order);
  }

  std::reverse(order.begin(), order.end());
  return order;
}

}  // namespace algo::graph::algo