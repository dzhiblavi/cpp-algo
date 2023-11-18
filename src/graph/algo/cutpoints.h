#pragma once

#include "graph/repr/traits.h"

#include <optional>
#include <vector>

namespace algo::graph::algo {

namespace detail {

template <repr::IndirectGraph Graph, typename NodeId = typename Graph::NodeId>
void findAllCutPoints(
    uint32_t& timer, NodeId node_id, std::optional<NodeId> parent, const auto& graph, auto& t_in, auto& f_up,
    auto& used, auto& cut_points) {
  assert(!used[node_id]);
  used[node_id] = true;
  t_in[node_id] = f_up[node_id] = timer++;
  uint32_t num_children = 0;
  bool is_cutpoint = false;

  for (auto [to_id, _] : graph.edgesFrom(node_id)) {
    if (parent && *parent == to_id) {
      continue;
    }

    if (used[to_id]) {
      f_up[node_id] = std::min(f_up[node_id], t_in[to_id]);
    } else {
      findAllBridges(timer, to_id, node_id, graph, t_in, f_up, used, cut_points);
      f_up[node_id] = std::min(f_up[node_id], f_up[to_id]);

      if (f_up[to_id] >= t_in[node_id] && parent.has_value()) {
        is_cutpoint = true;
      }

      ++num_children;
    }
  }

  if (!parent.has_value() && num_children > 1) {
    is_cutpoint = true;
  }

  if (is_cutpoint) {
    cut_points.emplace_back(node_id);
  }
}

}  // namespace detail

template <repr::IndirectGraph Graph, typename NodeId = typename Graph::NodeId>
std::vector<NodeId> findAllCutPoints(const Graph& graph) {
  auto t_in = repr::mapNodes<uint32_t>(graph);
  auto f_up = repr::mapNodes<uint32_t>(graph);
  auto used = repr::mapNodes<bool>(graph);
  uint32_t timer{0};
  std::vector<NodeId> cut_points;

  for (NodeId node_id : repr::nodes(graph)) {
    if (used[node_id]) {
      continue;
    }

    detail::findAllCutPoints(timer, node_id, std::nullopt, graph, t_in, f_up, used, cut_points);
  }

  return cut_points;
}

}  // namespace algo::graph::algo
