#include "algo/fenwick_tree/fenwick_tree.h"
#include "algo/segment_tree/segment_tree.h"
#include "algo/sparse_tree/sparse_tree.h"
#include "utils/compare_range_engines.h"
#include "utils/generate.h"
#include "utils/range_query.h"

#include <gtest/gtest.h>
#include <random>

namespace algo {

template <size_t NDims>
void TestCompareSGTvsSPT(const std::array<size_t, NDims>& dims) {
  test::compareRangeEnginesImmutable<
      sgt::SimpleSegmentTree<int, test::MinOp<int>, NDims>,
      spt::SparseTree<int, test::MinOp<int>, NDims>>(dims, 1000000);
}

template <size_t NDims>
void TestCompareSGTvsFWT(const std::array<size_t, NDims>& dims) {
  test::compareRangeEnginesImmutable<
      sgt::SimpleSegmentTree<int, test::SumOp<int>, NDims>,
      fwt::FenwickTree<int, fwt::SumOp<int>, NDims>>(dims, 1000000);
}

TEST(sgt_vs_spt, dims_1) {
  TestCompareSGTvsSPT<1>({1000000});
}

TEST(sgt_vs_spt, dims_2) {
  TestCompareSGTvsSPT<2>({128, 512});
}

TEST(sgt_vs_spt, dims_3) {
  TestCompareSGTvsSPT<3>({10, 30, 60});
}

TEST(sgt_vs_spt, dims_4) {
  TestCompareSGTvsSPT<4>({5, 11, 18, 23});
}

TEST(sgt_vs_fwt, dims_1) {
  TestCompareSGTvsFWT<1>({1000000});
}

TEST(sgt_vs_fwt, dims_2) {
  TestCompareSGTvsFWT<2>({128, 512});
}

TEST(sgt_vs_fwt, dims_3) {
  TestCompareSGTvsFWT<3>({10, 30, 60});
}

TEST(sgt_vs_fwt, dims_4) {
  TestCompareSGTvsFWT<4>({5, 11, 18, 23});
}

}  // namespace algo
