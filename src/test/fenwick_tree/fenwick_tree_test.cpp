#include "algo/fenwick_tree/fenwick_tree.h"

#include "test/rq_utils/compare_range_engines.h"
#include "test/rq_utils/generate.h"
#include "test/rq_utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace test::fwt::unit {

namespace fwt = ::algo::fwt;

template <size_t NDims>
void TestFWTvsNaive(const std::array<size_t, NDims>& dims) {
  rq_utils::compareRangeEnginesImmutable<
      fwt::FenwickTree<int, fwt::SumOp<int>, NDims>, rq_utils::NaiveRangeQueryEngine<int, rq_utils::SumOp<int>, NDims>>(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(FenwickTreeTest, Dim1) { TestFWTvsNaive<1>({10000}); }
TEST(FenwickTreeTest, Dim2) { TestFWTvsNaive<2>({73, 237}); }
TEST(FenwickTreeTest, Dim3) { TestFWTvsNaive<3>({13, 27, 49}); }
TEST(FenwickTreeTest, Dim4) { TestFWTvsNaive<4>({9, 13, 21, 17}); }

}  // namespace test::fwt::unit
