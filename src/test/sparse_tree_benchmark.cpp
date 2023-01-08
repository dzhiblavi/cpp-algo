#include "algo/sparse_tree/sparse_tree.h"
#include "utils/benchmark.h"

namespace algo::spt {

static void BM_sparse_tree_build(benchmark::State& state) {
  test::rq_build_bench<sparse_tree<int, test::min_op<int>, 1>>(state);
}

static void BM_sparse_tree_query(benchmark::State& state) {
  test::rq_query_bench<sparse_tree<int, test::min_op<int>, 1>>(state);
}

BENCHMARK(BM_sparse_tree_build)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_sparse_tree_query)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

}  // namespace algo::spt
