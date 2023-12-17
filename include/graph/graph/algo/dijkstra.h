#pragma once

#include "graph/repr/concepts.h"
#include "graph/repr/traits.h"

#include <set>

namespace algo::graph::algo {

template <
    repr::WeightedGraph G, typename NodeId = typename G::NodeId,
    typename WeightType = typename G::EdgePayload::WeightType>
auto denseDijkstra(const G& graph, NodeId from) {
  auto dist = repr::mapNodes<WeightType>(graph, std::numeric_limits<WeightType>::max());
  auto used = repr::mapNodes<WeightType>(graph);
  dist[from] = WeightType{0};

  for (size_t _ = 0; _ < graph.nodeCount(); ++_) {
    NodeId v = -1;
    for (NodeId u : repr::nodes(graph)) {
      if (!used[u] && (v == -1 || dist[u] < dist[v])) {
        v = u;
      }
    }

    if (dist[v] == std::numeric_limits<WeightType>::max()) {
      break;
    }

    used[v] = true;

    for (auto [to, edge_ref] : graph.edgesFrom(v)) {
      const auto new_dist = dist[v] + edge_ref.weight();

      if (new_dist < dist[to]) {
        dist[to] = new_dist;
      }
    }
  }

  return dist;
}

template <
    repr::WeightedGraph G, typename NodeId = typename G::NodeId,
    typename WeightType = typename G::EdgePayload::WeightType>
auto sparseDijkstra(const G& graph, NodeId from) {
  auto dist = repr::mapNodes<WeightType>(graph, std::numeric_limits<WeightType>::max());
  auto queue = std::set<std::pair<WeightType, NodeId>>();

  dist[from] = WeightType{0};
  queue.emplace(WeightType{0}, from);

  while (!queue.empty()) {
    NodeId v = queue.begin()->second;
    queue.erase(queue.begin());

    for (auto [to, edge_ref] : graph.edgesFrom(v)) {
      const auto new_dist = dist[v] + edge_ref.weight();
      const auto old_dist = dist[to];

      if (new_dist < old_dist) {
        queue.erase(std::make_pair(old_dist, to));
        dist[to] = new_dist;
        queue.emplace(new_dist, to);
      }
    }
  }

  return dist;
}

}  // namespace algo::graph::algo
