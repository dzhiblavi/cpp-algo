#include "algo/segment_tree/segment_tree.h"
#include "utils/benchmark.h"

namespace algo::sgt {

static void BM_segment_tree_build(benchmark::State& state) {
  test::rq_build_bench<segment_tree<int, test::min_op<int>, 1>>(state);
}

static void BM_segment_tree_query(benchmark::State& state) {
  test::rq_query_bench<segment_tree<int, test::min_op<int>, 1>>(state);
}

static void BM_segment_tree_update(benchmark::State& state) {
  test::rq_update_bench<segment_tree<int, test::min_op<int>, 1>>(state);
}

BENCHMARK(BM_segment_tree_build)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_segment_tree_query)
  // No threads because sgt query is not thread-safe
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_segment_tree_update)
  // No threads because sgt update is not thread-safe
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

}  // namespace algo::spt
