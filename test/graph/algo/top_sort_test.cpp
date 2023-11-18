#include "graph/algo/top_sort.h"

#include "graph/algo/cycle.h"
#include "graph/algo/reachable.h"
#include "test/graph/utility/fixture.h"
#include "test/graph/utility/small_graphs.h"

#include <gtest/gtest.h>
#include <set>
#include <vector>

namespace test::graph::algo::unit {

namespace algo = ::algo::graph::algo;

template <typename Graph>
using TopSortTest = util::BaseGraphTest<Graph>;
using TopSortTestGraphTypes =
    util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kDirect, util::kMaxNodeCount>;

TYPED_TEST_SUITE(TopSortTest, TopSortTestGraphTypes);

TYPED_TEST(TopSortTest, SmallGraphs) {
  using GraphType = TypeParam;

  for (auto make_graph : util::smallGraphs()) {
    GraphType g;
    make_graph(g);

    if (algo::hasCycle(g)) {
      return;
    }

    auto order = algo::topologicalSort(g);
    EXPECT_EQ(g.nodeCount(), order.size());

    for (size_t post = 1; post < order.size(); ++post) {
      for (size_t pre = 0; pre < post; ++pre) {
        EXPECT_FALSE(algo::isReachable(g, order[post], order[pre]));
      }
    }
  }
}

}  // namespace test::graph::algo::unit