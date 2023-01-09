#include <benchmark/benchmark.h>

#include "generate.h"

namespace algo::test {

template <typename Engine>
void rq_build_bench(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-100, +100, dims);
  for (auto _ : state) {
    Engine e(as);
  }
  state.SetComplexityN(state.range(0));
}

template <typename Engine>
void rq_query_bench(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-100, +100, dims);
  Engine e(as);
  size_t ql[1], qr[1];
  for (auto _ : state) {
    state.PauseTiming();
    test::random_range(ql, qr, dims);
    state.ResumeTiming();

    benchmark::DoNotOptimize(e.query(ql, qr));
  }
  state.SetComplexityN(state.range(0));
}

template <typename Engine>
void rq_update_bench(benchmark::State& state) {
  size_t dims[1] = { size_t(state.range(0)) };
  auto as = test::generate(-100, +100, dims);
  Engine e(as);
  size_t idxs[1];
  for (auto _ : state) {
    state.PauseTiming();
    test::random_index(idxs, dims);
    int value = std::uniform_int_distribution<int>(-100, 100)(test::generator());
    state.ResumeTiming();

    e.update(idxs, value);
  }
  state.SetComplexityN(state.range(0));
}

}  // namespace algo::test

