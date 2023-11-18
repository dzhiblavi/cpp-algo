#include "graph/algo/cycle.h"

#include "test/graph/utility/fixture.h"
#include "test/graph/utility/small_graphs.h"

#include <gtest/gtest.h>
#include <range/v3/view/enumerate.hpp>
#include <set>
#include <vector>

namespace test::graph::algo::unit {

namespace {

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

}  // namespace

namespace repr = ::algo::graph::repr;
namespace algo = ::algo::graph::algo;

template <typename Graph>
using CycleTest = util::BaseGraphTest<Graph>;
using CycleTestGraphTypes = util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kAll, util::kMaxNodeCount>;

TYPED_TEST_SUITE(CycleTest, CycleTestGraphTypes);

TYPED_TEST(CycleTest, SmallGraphs) {
  using GraphType = TypeParam;
  constexpr bool Direct = repr::DirectGraph<GraphType>;

  for (auto [index, make_graph] : ranges::views::enumerate(util::smallGraphs())) {
    GraphType g;
    make_graph(g);

    const bool has_cycle = (Direct == util::DirectKind::kDirect ? directTests() : indirectTests())[index++];
    EXPECT_EQ(has_cycle, algo::hasCycle(g));
  }
}

TEST(A, B) {}

}  // namespace test::graph::algo::unit