#include <gtest/gtest.h>
#include <random>

#include "algo/fenwick_tree/fenwick_tree.h"
#include "utils/range_query.h"
#include "utils/generate.h"
#include "utils/compare_range_engines.h"

namespace algo::fwt {

template <size_t NDims>
void TestFWTvsNaive(const size_t (&dims)[NDims]) {
  test::CompareRangeEnginesImmutable<
    fwt::fenwick_tree<int, fenwick_sum_op<int>, NDims>,
    test::naive_range_query<int, test::sum_op<int>, NDims>
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

TEST(fwt, dims_1) { TestFWTvsNaive({ 10000 }); }

TEST(fwt, dims_2) { TestFWTvsNaive({ 73, 237 }); }

TEST(fwt, dims_3) { TestFWTvsNaive({ 13, 27, 49 }); }

TEST(fwt, dims_4) { TestFWTvsNaive({ 9, 13, 21, 17 }); }

}  // namespace algo::sgt

