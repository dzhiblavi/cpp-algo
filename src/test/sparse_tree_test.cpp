#include <gtest/gtest.h>
#include <random>

#include "algo/sparse_tree/sparse_tree.h"
#include "utils/range_query.h"
#include "utils/generate.h"
#include "utils/compare_range_engines.h"

namespace algo::spt {

template <size_t NDims>
void TestSPTvsNaive(const size_t (&dims)[NDims]) {
  test::CompareRangeEnginesImmutable<
    spt::sparse_tree<int, test::min_op<int>, NDims>,
    test::naive_range_query<int, test::min_op<int>, NDims>
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

TEST(spt, dims_1) { TestSPTvsNaive({ 10000 }); }

TEST(spt, dims_2) { TestSPTvsNaive({ 73, 237 }); }

TEST(spt, dims_3) { TestSPTvsNaive({ 13, 27, 49 }); }

TEST(spt, dims_4) { TestSPTvsNaive({ 9, 13, 21, 17 }); }

}  // namespace algo::spt

