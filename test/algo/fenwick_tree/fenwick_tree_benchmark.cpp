#include "algo/fenwick_tree/fenwick_tree.h"
#include "test/algo/rq_utils/benchmark.h"

namespace test::fwt::benchmark {

namespace fwt = ::algo::fwt;

static void BM_fenwick_tree_build(::benchmark::State& state) {
  rq_utils::rqBuildBenchmark<fwt::FenwickTree<int, fwt::SumOp<int>, 1>>(state);
}

static void BM_fenwick_tree_query(::benchmark::State& state) {
  rq_utils::rqQueryBenchmark<fwt::FenwickTree<int, fwt::SumOp<int>, 1>>(state);
}

static void BM_fenwick_tree_update(::benchmark::State& state) {
  rq_utils::rqUpdateBenchmark<fwt::FenwickTree<int, fwt::SumOp<int>, 1>>(state);
}

BENCHMARK(BM_fenwick_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_fenwick_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_fenwick_tree_update)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace test::fwt::benchmark

BENCHMARK_MAIN();