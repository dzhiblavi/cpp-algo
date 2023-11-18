#pragma once

#include "graph/repr/traits.h"

#include <optional>

namespace algo::graph::algo {

namespace detail {

static constexpr uint8_t kNotVisited = 0;
static constexpr uint8_t kGray = 1;
static constexpr uint8_t kBlack = 2;

template <repr::DirectGraph G, typename NodeId = typename G::NodeId>
bool hasCycle(const G& graph, NodeId v, auto& clr) {
  clr[v] = kGray;

  for (auto [u, _] : graph.edgesFrom(v)) {
    switch (clr[u]) {
      case kBlack:
        break;
      case kGray:
        return true;
      case kNotVisited:
        if (hasCycle(graph, u, clr)) {
          return true;
        }
    }
  }

  clr[v] = kBlack;
  return false;
}

template <repr::IndirectGraph G, typename NodeId = typename G::NodeId>
bool hasCycle(const G& graph, NodeId v, auto& clr, std::optional<NodeId> parent) {
  clr[v] = kGray;

  for (auto [u, _] : graph.edgesFrom(v)) {
    if (parent.has_value() && *parent == u) {
      continue;
    }

    switch (clr[u]) {
      case kBlack:
        break;
      case kGray:
        return true;
      case kNotVisited:
        if (hasCycle(graph, u, clr, std::optional<NodeId>{v})) {
          return true;
        }
    }
  }

  clr[v] = kBlack;
  return false;
}

}  // namespace detail

template <repr::Graph G>
[[nodiscard]] bool hasCycle(const G& graph) {
  auto color = repr::mapNodes<uint8_t>(graph);

  for (auto node : repr::nodes(graph)) {
    if (color[node] != detail::kNotVisited) {
      continue;
    }

    bool has_cycle = [&] {
      if constexpr (repr::DirectGraph<G>) {
        return detail::hasCycle(graph, node, color);
      } else {
        return detail::hasCycle(graph, node, color, {});
      }
    }();

    if (has_cycle) {
      return true;
    }
  }

  return false;
}

}  // namespace algo::graph::algo