#include "algo/sparse_tree/sparse_tree.h"

#include "test/algo/rq_utils/compare_range_engines.h"
#include "test/algo/rq_utils/generate.h"
#include "test/algo/rq_utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace test::spt::unit {

namespace spt = ::algo::spt;

template <size_t NDims>
void TestSPTvsNaive(const std::array<size_t, NDims>& dims) {
  rq_utils::compareRangeEnginesImmutable<
      spt::SparseTree<int, rq_utils::MinOp<int>, NDims>,
      rq_utils::NaiveRangeQueryEngine<int, rq_utils::MinOp<int>, NDims>>(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(SparseTreeTest, Dims1) {
  TestSPTvsNaive<1>({10000});
}
TEST(SparseTreeTest, Dims2) {
  TestSPTvsNaive<2>({73, 237});
}
TEST(SparseTreeTest, Dims3) {
  TestSPTvsNaive<3>({13, 27, 49});
}
TEST(SparseTreeTest, Dims4) {
  TestSPTvsNaive<4>({9, 13, 21, 17});
}

}  // namespace test::spt::unit
