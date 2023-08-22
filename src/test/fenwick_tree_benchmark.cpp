#include "algo/fenwick_tree/fenwick_tree.h"
#include "utils/benchmark.h"

namespace algo::fwt {

static void BM_fenwick_tree_build(benchmark::State& state) {
  test::rqBuildBenchmark<FenwickTree<int, SumOp<int>, 1>>(state);
}

static void BM_fenwick_tree_query(benchmark::State& state) {
  test::rqQueryBenchmark<FenwickTree<int, SumOp<int>, 1>>(state);
}

static void BM_fenwick_tree_update(benchmark::State& state) {
  test::rqUpdateBenchmark<FenwickTree<int, SumOp<int>, 1>>(state);
}

BENCHMARK(BM_fenwick_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_fenwick_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_fenwick_tree_update)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace algo::fwt
