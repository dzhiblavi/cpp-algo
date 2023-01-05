#include <benchmark/benchmark.h>

#include "algo/sparse_tree/sparse_tree.h"
#include "utils/generate.h"

namespace algo::spt {

static void BM_sparse_tree_build(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-10000, +10000, dims);
  for (auto _ : state) {
    sparse_tree<int, test::min_op<int>, 1> spt(as);
  }
  state.SetComplexityN(state.range(0));
}

static void BM_sparse_tree_query(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-10000, +10000, dims);
  sparse_tree<int, test::min_op<int>, 1> spt(as);
    size_t ql[1], qr[1];
  for (auto _ : state) {
    state.PauseTiming();
    test::random_range(ql, qr, dims);
    state.ResumeTiming();
    benchmark::DoNotOptimize(spt.get(ql, qr));
  }
  state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_sparse_tree_build)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

BENCHMARK(BM_sparse_tree_query)
  ->Threads(8)
  ->RangeMultiplier(2)->Range(1, 1ULL << 20)
  ->Complexity();

}  // namespace algo::spt
