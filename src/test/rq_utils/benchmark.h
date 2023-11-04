#include "generate.h"
#include "src/algo/utility/nd_container.h"

#include <benchmark/benchmark.h>

namespace test::rq_utils {

template <typename Engine>
void rqBuildBenchmark(benchmark::State& state) {
  std::array<size_t, 1> dims = {size_t(state.range(0))};
  auto as = generate(-100, +100, dims);
  for (auto _ : state) {
    Engine e{algo::utility::asView(as)};
  }
  state.SetComplexityN(state.range(0));
}

template <typename Engine>
void rqQueryBenchmark(benchmark::State& state) {
  std::array<size_t, 1> dims = {size_t(state.range(0))};
  auto as = generate(-100, +100, dims);
  Engine e{algo::utility::asView(as)};
  auto query_handle = e.getRangeQueryHandle();

  for (auto _ : state) {
    state.PauseTiming();
    randomRange(query_handle.getRange().left, query_handle.getRange().right, dims);
    state.ResumeTiming();

    benchmark::DoNotOptimize(e.query(query_handle));
  }
  state.SetComplexityN(state.range(0));
}

template <typename Engine>
void rqUpdateBenchmark(benchmark::State& state) {
  std::array<size_t, 1> dims = {size_t(state.range(0))};
  auto as = generate(-100, +100, dims);
  Engine e{algo::utility::asView(as)};
  auto query_handle = e.getQueryHandle();

  for (auto _ : state) {
    state.PauseTiming();
    randomIndex(query_handle.getIndex(), dims);
    int value = std::uniform_int_distribution<int>(-100, 100)(utility::random::generator());
    state.ResumeTiming();

    e.update(query_handle, value);
  }
  state.SetComplexityN(state.range(0));
}

}  // namespace test::rq_utils
