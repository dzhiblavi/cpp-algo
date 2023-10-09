#include "algo/fenwick_tree/fenwick_tree.h"

#include "test/utils/compare_range_engines.h"
#include "test/utils/generate.h"
#include "test/utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace algo::fwt::unit {

template <size_t NDims>
void TestFWTvsNaive(const std::array<size_t, NDims>& dims) {
  test::rq::compareRangeEnginesImmutable<
      fwt::FenwickTree<int, SumOp<int>, NDims>, test::rq::NaiveRangeQueryEngine<int, test::rq::SumOp<int>, NDims> >(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(FenwickTreeTest, Dim1) { TestFWTvsNaive<1>({10000}); }
TEST(FenwickTreeTest, Dim2) { TestFWTvsNaive<2>({73, 237}); }
TEST(FenwickTreeTest, Dim3) { TestFWTvsNaive<3>({13, 27, 49}); }
TEST(FenwickTreeTest, Dim4) { TestFWTvsNaive<4>({9, 13, 21, 17}); }

}  // namespace algo::fwt::unit
