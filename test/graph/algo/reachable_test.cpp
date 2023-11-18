#include "graph/algo/reachable.h"

#include "test/graph/utility/fixture.h"
#include "test/graph/utility/small_graphs.h"

#include <gtest/gtest.h>
#include <range/v3/view/enumerate.hpp>
#include <set>
#include <vector>

namespace test::graph::algo::unit {

namespace {

const auto& indirectTests() {
  static const std::vector<std::vector<std::tuple<int, int, bool>>> tests{
      {},
      {
          {0, 0, true},
      },
      {
          {0, 1, true},
          {1, 0, true},
      },
      {
          {0, 1, false},
          {1, 2, false},
          {2, 0, false},
          {1, 1, true},
      },
      {
          {0, 1, true},
          {0, 6, false},
          {1, 3, true},
          {4, 0, true},
          {7, 5, false},
          {3, 0, true},
          {6, 2, false},
      },
      {
          {3, 0, true},
          {2, 4, true},
      },
  };
  return tests;
}

const auto& directTests() {
  static const std::vector<std::vector<std::tuple<int, int, bool>>> tests{
      {},
      {
          {0, 0, true},
      },
      {
          {0, 1, true},
          {1, 0, false},
      },
      {
          {0, 1, false},
          {1, 2, false},
          {2, 0, false},
          {1, 1, true},
      },
      {
          {0, 1, true},
          {0, 6, false},
          {1, 3, true},
          {4, 0, false},
          {7, 5, false},
          {3, 0, false},
          {6, 2, false},
      },
      {
          {3, 0, true},
          {2, 4, false},
      },
  };
  return tests;
}

}  // namespace

namespace algo = ::algo::graph::algo;
namespace repr = ::algo::graph::repr;

template <typename Graph>
using ReachableTest = util::BaseGraphTest<Graph>;
using ReachableTestGraphTypes =
    util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kAll, util::kMaxNodeCount>;

TYPED_TEST_SUITE(ReachableTest, ReachableTestGraphTypes);

template <typename GraphType, bool BFS>
void testReachable() {
  static constexpr bool Direct = repr::DirectGraph<GraphType>;

  for (auto [index, make_graph] : ranges::views::enumerate(util::smallGraphs())) {
    const auto& tests = (Direct == util::DirectKind::kDirect ? directTests() : indirectTests())[index++];

    GraphType g;
    make_graph(g);

    for (const auto& [from, to, reachable] : tests) {
      const auto res = algo::isReachable<GraphType, BFS>(g, from, to);
      EXPECT_EQ(reachable, res);
    }
  }
}

TYPED_TEST(ReachableTest, SmallGraphsBfs) { testReachable<TypeParam, true>(); }
TYPED_TEST(ReachableTest, SmallGraphsDfs) { testReachable<TypeParam, false>(); }

}  // namespace test::graph::algo::unit