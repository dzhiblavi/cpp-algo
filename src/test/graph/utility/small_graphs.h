#pragma once

#include "algo/graph/repr/concepts.h"

#include <ranges>
#include <vector>

namespace algo::graph::util {

namespace detail {

template <repr::Graph G, typename NodeId = typename G::NodeId>
void makeGraphFromEdges(const std::vector<std::pair<NodeId, NodeId>>& edges, G& g) {
  size_t node_count = 0;
  for (const auto& [from, to] : edges) {
    node_count = std::max(node_count, 1 + static_cast<size_t>(from));
    node_count = std::max(node_count, 1 + static_cast<size_t>(to));
  }

  g.addNodes(node_count);
  for (const auto& [from, to] : edges) {
    if (g.hasEdge(from, to)) {
      continue;
    }
    g.addEdge(from, to);
  }
}

inline const auto& getGraphExamples() {
  static std::vector<std::vector<std::pair<int, int>>> examples = {
      {
          //
      },
      {
          {0, 0},
      },
      {
          {0, 1},
      },
      {
          {0, 0},
          {1, 1},
          {2, 2},
      },
      {
          {0, 1},
          {1, 2},
          {2, 0},
          {2, 3},
          {3, 4},
          {4, 5},
          {5, 3},
          {6, 7},
          {7, 6},
      },
      {
          {0, 1},
          {1, 2},
          {3, 4},
          {4, 0},
      },
  };

  return examples;
}

}  // namespace detail

[[maybe_unused]] static constexpr size_t kMaxNodeCount = 128;

inline auto smallGraphs() {
  return std::ranges::ref_view(detail::getGraphExamples()) |  //
         std::views::transform([](const auto& edges) {
           return [&edges](auto& graph) { detail::makeGraphFromEdges(edges, graph); };
         });
}

}  // namespace algo::graph::util