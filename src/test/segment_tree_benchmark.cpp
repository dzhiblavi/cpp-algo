#include <benchmark/benchmark.h>

#include "algo/segment_tree/segment_tree.h"
#include "utils/generate.h"

namespace algo::sgt {

static void BM_segment_tree_build(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-10000, +10000, dims);
  for (auto _ : state) {
    segment_tree<int, test::min_op<int>, 1> sgt(as);
  }
  state.SetComplexityN(state.range(0));
}

static void BM_segment_tree_query(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-10000, +10000, dims);
  segment_tree<int, test::min_op<int>, 1> sgt(as);
  size_t ql[1], qr[1];
  for (auto _ : state) {
    state.PauseTiming();
    test::random_range(ql, qr, dims);
    state.ResumeTiming();

    benchmark::DoNotOptimize(sgt.get(ql, qr));
  }
  state.SetComplexityN(state.range(0));
}

static void BM_segment_tree_update(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-10000, +10000, dims);
  segment_tree<int, test::min_op<int>, 1> sgt(as);
  size_t idxs[1];
  for (auto _ : state) {
    state.PauseTiming();
    test::random_index(idxs, dims);
    int value = std::uniform_int_distribution<int>(-10000, 10000)(test::generator());
    state.ResumeTiming();

    sgt.set(idxs, value);
  }
  state.SetComplexityN(state.range(0));
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
