#include "algo/sparse_tree/sparse_tree.h"
#include "utils/benchmark.h"
#include "utils/range_query.h"

namespace algo::spt {

static void BM_sparse_tree_build(benchmark::State& state) {
  test::rqBuildBenchmark<SparseTree<int, test::MinOp<int>, 1>>(state);
}

static void BM_sparse_tree_query(benchmark::State& state) {
  test::rqQueryBenchmark<SparseTree<int, test::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_sparse_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_sparse_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace algo::spt
