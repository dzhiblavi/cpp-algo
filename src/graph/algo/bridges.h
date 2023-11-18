#pragma once

#include "graph/repr/traits.h"

#include <optional>
#include <vector>

namespace algo::graph::algo {

namespace detail {

template <repr::IndirectGraph Graph, typename NodeId = typename Graph::NodeId>
void findAllBridges(
    uint32_t& timer, NodeId node_id, std::optional<NodeId> parent, const auto& graph, auto& t_in, auto& f_up,
    auto& used, auto& bridges) {
  assert(!used[node_id]);
  used[node_id] = true;
  t_in[node_id] = f_up[node_id] = timer++;

  for (auto [to_id, _] : graph.edgesFrom(node_id)) {
    if (parent && *parent == to_id) {
      continue;
    }

    if (used[to_id]) {
      f_up[node_id] = std::min(f_up[node_id], t_in[to_id]);
    } else {
      findAllBridges(timer, to_id, node_id, graph, t_in, f_up, used, bridges);
      f_up[node_id] = std::min(f_up[node_id], f_up[to_id]);

      if (f_up[to_id] > t_in[node_id]) {
        bridges.emplace_back(node_id, to_id);
      }
    }
  }
}

}  // namespace detail

template <repr::IndirectGraph Graph, typename NodeId = typename Graph::NodeId>
std::vector<std::pair<NodeId, NodeId>> findAllBridges(const Graph& graph) {
  auto t_in = repr::mapNodes<uint32_t>(graph);
  auto f_up = repr::mapNodes<uint32_t>(graph);
  auto used = repr::mapNodes<bool>(graph);
  uint32_t timer{0};
  std::vector<std::pair<NodeId, NodeId>> bridges;

  for (NodeId node_id : repr::nodes(graph)) {
    if (used[node_id]) {
      continue;
    }

    detail::findAllBridges(timer, node_id, std::nullopt, graph, t_in, f_up, used, bridges);
  }

  return bridges;
}

}  // namespace algo::graph::algo