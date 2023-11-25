#include "algo/sparse_tree/sparse_tree.h"
#include "test/algo/rq_utils/benchmark.h"
#include "test/algo/rq_utils/range_query.h"

namespace test::spt::benchmark {

namespace spt = ::algo::spt;

static void BM_sparse_tree_build(::benchmark::State& state) {
  rq_utils::rqBuildBenchmark<spt::SparseTree<int, rq_utils::MinOp<int>, 1>>(state);
}

static void BM_sparse_tree_query(::benchmark::State& state) {
  rq_utils::rqQueryBenchmark<spt::SparseTree<int, rq_utils::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_sparse_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_sparse_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace test::spt::benchmark

BENCHMARK_MAIN();