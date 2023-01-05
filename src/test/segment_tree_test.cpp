#include <gtest/gtest.h>
#include <random>

#include "algo/segment_tree/segment_tree.h"
#include "utils/range_query.h"
#include "utils/generate.h"
#include "utils/compare_range_engines.h"

namespace algo::sgt {

template <size_t NDims>
void TestSGTvsNaive(const size_t (&dims)[NDims]) {
  test::CompareRangeEnginesMutable<
    sgt::segment_tree<int, test::min_op<int>, NDims>,
    test::naive_range_query<int, test::min_op<int>, NDims>
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

TEST(sgt, dims_1) { TestSGTvsNaive({ 10000 }); }

TEST(sgt, dims_2) { TestSGTvsNaive({ 73, 237 }); }

TEST(sgt, dims_3) { TestSGTvsNaive({ 13, 27, 49 }); }

TEST(sgt, dims_4) { TestSGTvsNaive({ 9, 13, 21, 17 }); }

}  // namespace algo::sgt

