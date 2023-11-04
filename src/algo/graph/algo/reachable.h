#pragma once

#include "algo/graph/repr/traits.h"
#include "algo/graph/utility/queue.h"

#include <stack>

namespace algo::graph::algo {

template <repr::Graph G, bool BFS = false, typename NodeId = typename G::NodeId>
[[nodiscard]] bool isReachable(const G& graph, NodeId from, NodeId to) {
  if (from == to) [[unlikely]] {
    return true;
  }

  auto used = repr::mapNodes<bool>(graph);
  utility::Queue<NodeId, /* FIFO = */ BFS> queue;

  queue.push(from);
  used[from] = true;

  while (!queue.empty()) {
    const auto v = queue.pop();

    for (auto [u, _] : graph.edgesFrom(v)) {
      if (used[u]) {
        continue;
      }
      if (u == to) {
        return true;
      }

      queue.push(u);
      used[u] = true;
    }
  }

  return false;
}

}  // namespace algo::graph::algo