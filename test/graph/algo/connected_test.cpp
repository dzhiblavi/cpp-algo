#include "graph/algo/connected.h"

#include "test/graph/utility/fixture.h"
#include "test/graph/utility/small_graphs.h"

#include <gtest/gtest.h>
#include <range/v3/view/enumerate.hpp>
#include <set>
#include <vector>

namespace test::graph::algo::unit {

namespace {

const auto& indirectTests() {
  static const std::vector<std::vector<std::vector<uint32_t>>> expected_components{
      {},
      {{0}},
      {{0, 1}},
      {{0}, {1}, {2}},
      {
          {0, 1, 2, 3, 4, 5},
          {6, 7},
      },
      {{0, 1, 2, 3, 4}},
  };
  return expected_components;
}

const auto& directTests() {
  static const std::vector<std::vector<std::vector<uint32_t>>> expected_components{
      {},
      {{0}},
      {{0}, {1}},
      {{0}, {1}, {2}},
      {
          {0, 1, 2},
          {3, 4, 5},
          {6, 7},
      },
      {{0}, {1}, {2}, {3}, {4}},
  };
  return expected_components;
}

}  // namespace

namespace algo = ::algo::graph::algo;

void sort_comps(auto& components) {
  for (auto& comp : components) {
    std::sort(comp.begin(), comp.end());
  }
  std::sort(components.begin(), components.end());
}

template <typename Graph>
using ConnectedIndirectTest = util::BaseGraphTest<Graph>;
using ConnectedTestIndirectGraphTypes =
    util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kIndirect, util::kMaxNodeCount>;

TYPED_TEST_SUITE(ConnectedIndirectTest, ConnectedTestIndirectGraphTypes);

TYPED_TEST(ConnectedIndirectTest, SmallGraphs) {
  using GraphType = TypeParam;

  for (auto [index, make_graph] : ranges::views::enumerate(util::smallGraphs())) {
    GraphType g;
    make_graph(g);

    const auto& expected = indirectTests()[index++];
    auto components = algo::connectedComponents(g);
    sort_comps(components);
    EXPECT_EQ(expected, components);
  }
}

template <typename Graph>
using ConnectedDirectTest = util::BaseGraphTest<Graph>;
using ConnectedTestDirectGraphTypes =
    util::GTestGraphTypes<util::GraphKind::kAll, util::DirectKind::kDirect, util::kMaxNodeCount>;

TYPED_TEST_SUITE(ConnectedDirectTest, ConnectedTestDirectGraphTypes);

TYPED_TEST(ConnectedDirectTest, SmallGraphs) {
  using GraphType = TypeParam;

  for (auto [index, make_graph] : ranges::views::enumerate(util::smallGraphs())) {
    GraphType g;
    make_graph(g);

    const auto& expected = directTests()[index++];
    auto components = algo::stronglyConnectedComponents(g, g.transposedView());
    sort_comps(components);
    EXPECT_EQ(expected, components);
  }
}

}  // namespace test::graph::algo::unit