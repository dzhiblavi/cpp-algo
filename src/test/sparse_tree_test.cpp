#include "algo/sparse_tree/sparse_tree.h"

#include "utils/compare_range_engines.h"
#include "utils/generate.h"
#include "utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace algo::spt {

template <size_t NDims>
void TestSPTvsNaive(const std::array<size_t, NDims>& dims) {
  test::compareRangeEnginesImmutable<
      spt::SparseTree<int, test::MinOp<int>, NDims>,
      test::NaiveRangeQueryEngine<int, test::MinOp<int>, NDims> >(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(spt, dims_1) {
  TestSPTvsNaive<1>({10000});
}

TEST(spt, dims_2) {
  TestSPTvsNaive<2>({73, 237});
}

TEST(spt, dims_3) {
  TestSPTvsNaive<3>({13, 27, 49});
}

TEST(spt, dims_4) {
  TestSPTvsNaive<4>({9, 13, 21, 17});
}

}  // namespace algo::spt
