#include <gtest/gtest.h>

#include <random>

#include "algo/segment_tree/segment_tree.h"
#include "algo/sparse_tree/sparse_tree.h"
#include "utils/range_query.h"
#include "utils/generate.h"
#include "utils/compare_range_engines.h"

namespace algo {

template <size_t NDims>
void TestCompareSGTvsSPT(const size_t (&dims)[NDims]) {
  test::CompareRangeEnginesImmutable<
    sgt::segment_tree<int, test::min_op<int>, NDims>,
    spt::sparse_tree<int, test::min_op<int>, NDims>
  >(dims, 1000000);
}

TEST(sgt_vs_spt, dims_1) { TestCompareSGTvsSPT({ 1000000 }); }

TEST(sgt_vs_spt, dims_2) { TestCompareSGTvsSPT({ 128, 512 }); }

TEST(sgt_vs_spt, dims_3) { TestCompareSGTvsSPT({ 10, 30, 60 }); }

TEST(sgt_vs_spt, dims_4) { TestCompareSGTvsSPT({ 5, 11, 18, 23 }); }

}  // namespace algo

