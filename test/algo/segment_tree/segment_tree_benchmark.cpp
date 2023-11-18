#include "algo/segment_tree/segment_tree.h"
#include "test/rq_utils/benchmark.h"
#include "test/rq_utils/range_query.h"

namespace test::sgt::benchmark {

namespace sgt = ::algo::sgt;

static void BM_segment_tree_build(::benchmark::State& state) {
  rq_utils::rqBuildBenchmark<sgt::SimpleSegmentTree<int, rq_utils::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_query(::benchmark::State& state) {
  rq_utils::rqQueryBenchmark<sgt::SimpleSegmentTree<int, rq_utils::MinOp<int>, 1>>(state);
}

static void BM_segment_tree_update(::benchmark::State& state) {
  rq_utils::rqUpdateBenchmark<sgt::SimpleSegmentTree<int, rq_utils::MinOp<int>, 1>>(state);
}

BENCHMARK(BM_segment_tree_build)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_query)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();
BENCHMARK(BM_segment_tree_update)->RangeMultiplier(2)->Range(1, 1ULL << 20)->Complexity();

}  // namespace test::sgt::benchmark
