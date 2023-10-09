#include "algo/sparse_tree/sparse_tree.h"
#include "test/utils/benchmark.h"
#include "test/utils/range_query.h"

namespace algo::spt::benchmark {

static void BM_sparse_tree_build(::benchmark::State& state) {
  test::rq::rqBuildBenchmark<SparseTree<int, test::rq::MinOp<int>, 1>>(state);
}

static void BM_sparse_tree_query(::benchmark::State& state) {
  test::rq::rqQueryBenchmark<SparseTree<int, test::rq::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_sparse_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_sparse_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace algo::spt::benchmark
