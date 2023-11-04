#include "algo/graph/algo/cycle.h"

#include "test/graph/utility/fixture.h"
#include "test/graph/utility/small_graphs.h"

#include <gtest/gtest.h>
#include <set>
#include <vector>

namespace algo::graph::algo {

const auto& indirectTests() {
  static const std::vector<bool> tests{
      false, true, false, true, true, false,
  };
  return tests;
}

const auto& directTests() {
  static const std::vector<bool> tests{
      false, true, false, true, true, false,
  };
  return tests;
}

template <typename Graph>
using CycleTest = util::BaseGraphTest<Graph>;
using CycleTestGraphTypes = util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kAll, util::kMaxNodeCount>;

TYPED_TEST_SUITE(CycleTest, CycleTestGraphTypes);

TYPED_TEST(CycleTest, SmallGraphs) {
  using GraphType = TypeParam;
  constexpr bool Direct = repr::DirectGraph<GraphType>;

  size_t index = 0;
  for (auto make_graph : util::smallGraphs()) {
    GraphType g;
    make_graph(g);

    const bool has_cycle = (Direct == util::DirectKind::kDirect ? directTests() : indirectTests())[index++];
    EXPECT_EQ(has_cycle, hasCycle(g));
  }
}

}  // namespace algo::graph::algo