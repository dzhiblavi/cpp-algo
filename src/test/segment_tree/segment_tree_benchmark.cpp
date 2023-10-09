#include "algo/segment_tree/segment_tree.h"
#include "test/utils/benchmark.h"
#include "test/utils/range_query.h"

namespace algo::sgt::benchmark {

static void BM_segment_tree_build(::benchmark::State& state) {
  test::rq::rqBuildBenchmark<SimpleSegmentTree<int, test::rq::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_query(::benchmark::State& state) {
  test::rq::rqQueryBenchmark<SimpleSegmentTree<int, test::rq::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_update(::benchmark::State& state) {
  test::rq::rqUpdateBenchmark<SimpleSegmentTree<int, test::rq::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_segment_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_update)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace algo::sgt::benchmark
