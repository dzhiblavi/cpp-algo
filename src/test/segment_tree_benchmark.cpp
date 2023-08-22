#include "algo/segment_tree/segment_tree.h"
#include "utils/benchmark.h"
#include "utils/range_query.h"

namespace algo::sgt {

static void BM_segment_tree_build(benchmark::State& state) {
  test::rqBuildBenchmark<SimpleSegmentTree<int, test::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_query(benchmark::State& state) {
  test::rqQueryBenchmark<SimpleSegmentTree<int, test::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_update(benchmark::State& state) {
  test::rqUpdateBenchmark<SimpleSegmentTree<int, test::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_segment_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_update)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace algo::sgt
