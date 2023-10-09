#include "algo/fenwick_tree/fenwick_tree.h"
#include "algo/segment_tree/segment_tree.h"
#include "algo/sparse_tree/sparse_tree.h"
#include "test/utils/compare_range_engines.h"
#include "test/utils/generate.h"
#include "test/utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace algo::unit {

template <size_t NDims>
void TestCompareSGTvsSPT(const std::array<size_t, NDims>& dims) {
  test::rq::compareRangeEnginesImmutable<
      sgt::SimpleSegmentTree<int, test::rq::MinOp<int>, NDims>, spt::SparseTree<int, test::rq::MinOp<int>, NDims>>(
      dims, 1000000);
}

template <size_t NDims>
void TestCompareSGTvsFWT(const std::array<size_t, NDims>& dims) {
  test::rq::compareRangeEnginesImmutable<
      sgt::SimpleSegmentTree<int, test::rq::SumOp<int>, NDims>, fwt::FenwickTree<int, fwt::SumOp<int>, NDims>>(
      dims, 1000000);
}

TEST(SegmentTreeVsSparseTreeTest, Dims1) { TestCompareSGTvsSPT<1>({1000000}); }
TEST(SegmentTreeVsSparseTreeTest, Dims2) { TestCompareSGTvsSPT<2>({128, 512}); }
TEST(SegmentTreeVsSparseTreeTest, Dims3) { TestCompareSGTvsSPT<3>({10, 30, 60}); }
TEST(SegmentTreeVsSparseTreeTest, Dims4) { TestCompareSGTvsSPT<4>({5, 11, 18, 23}); }

TEST(SegmentTreeVsFenwickTreeTest, Dims1) { TestCompareSGTvsFWT<1>({1000000}); }
TEST(SegmentTreeVsFenwickTreeTest, Dims2) { TestCompareSGTvsFWT<2>({128, 512}); }
TEST(SegmentTreeVsFenwickTreeTest, Dims3) { TestCompareSGTvsFWT<3>({10, 30, 60}); }
TEST(SegmentTreeVsFenwickTreeTest, Dims4) { TestCompareSGTvsFWT<4>({5, 11, 18, 23}); }

}  // namespace algo::unit
