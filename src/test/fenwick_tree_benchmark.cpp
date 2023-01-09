#include "algo/fenwick_tree/fenwick_tree.h"
#include "utils/benchmark.h"

namespace algo::fwt {

static void BM_fenwick_tree_build(benchmark::State& state) {
  test::rq_build_bench<fenwick_tree<int, fenwick_sum_op<int>, 1>>(state);
}

static void BM_fenwick_tree_query(benchmark::State& state) {
  test::rq_query_bench<fenwick_tree<int, fenwick_sum_op<int>, 1>>(state);
}

static void BM_fenwick_tree_update(benchmark::State& state) {
  test::rq_update_bench<fenwick_tree<int, fenwick_sum_op<int>, 1>>(state);
}

BENCHMARK(BM_fenwick_tree_build)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_fenwick_tree_query)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_fenwick_tree_update)
  // No threads because fwt update is not thread-safe
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

}  // namespace algo::spt

