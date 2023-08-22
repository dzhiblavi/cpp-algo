#include "algo/fenwick_tree/fenwick_tree.h"

#include "utils/compare_range_engines.h"
#include "utils/generate.h"
#include "utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace algo::fwt {

template <size_t NDims>
void TestFWTvsNaive(const std::array<size_t, NDims>& dims) {
  test::compareRangeEnginesImmutable<
      fwt::FenwickTree<int, SumOp<int>, NDims>,
      test::NaiveRangeQueryEngine<int, test::SumOp<int>, NDims> >(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(fwt, dims_1) {
  TestFWTvsNaive<1>({10000});
}

TEST(fwt, dims_2) {
  TestFWTvsNaive<2>({73, 237});
}

TEST(fwt, dims_3) {
  TestFWTvsNaive<3>({13, 27, 49});
}

TEST(fwt, dims_4) {
  TestFWTvsNaive<4>({9, 13, 21, 17});
}

}  // namespace algo::fwt
