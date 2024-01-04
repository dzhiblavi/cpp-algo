#include "algo/kdtree/kdtree.h"
#include "test/algo/rq_utils/generate.h"

#include <benchmark/benchmark.h>

namespace test::kdtree::benchmark {

namespace kdtree = ::algo::kdtree;

template <size_t Dims>
static void BM_kdtree_build(::benchmark::State& state) {
  for (auto _ : state) {
    state.PauseTiming();
    auto points = rq_utils::generatePoints<Dims>(state.range());
    state.ResumeTiming();

    kdtree::KDTree<std::array<int, Dims>, Dims> tree{points};
  }

  state.SetComplexityN(state.range());
}

template <size_t Dims, typename BenchmarkedOperation, typename ArgsGenerator>
static void kdtreeGenericBenchmark(::benchmark::State& state, BenchmarkedOperation operation, ArgsGenerator generator) {
  auto points = rq_utils::generatePoints<Dims>(state.range());
  kdtree::KDTree<std::array<int, Dims>, Dims> tree{points};
  std::array<int, Dims> q;

  for (auto _ : state) {
    state.PauseTiming();
    rq_utils::generatePoint(q);
    const auto args_tuple = generator();
    state.ResumeTiming();

    std::apply([&](auto... args) { ::benchmark::DoNotOptimize(operation(tree, q, args...)); }, args_tuple);
  }

  state.SetComplexityN(state.range());
}

template <size_t Dims>
static void BM_kdtree_nn_search(::benchmark::State& state) {
  kdtreeGenericBenchmark<Dims>(
      state,                                                       //
      [](auto& tree, const auto& q) { return tree.nnSearch(q); },  //
      [] { return std::tuple<>{}; });
}

template <size_t Dims, size_t K>
static void BM_kdtree_knn_search(::benchmark::State& state) {
  kdtreeGenericBenchmark<Dims>(
      state,                                                           //
      [](auto& tree, const auto& q) { return tree.knnSearch(q, K); },  //
      [] { return std::tuple<>{}; });
}

template <size_t Dims, uint64_t Radius>
static void BM_kdtree_radius_search(::benchmark::State& state) {
  kdtreeGenericBenchmark<Dims>(
      state,                                                                   //
      [](auto& tree, const auto& q) { return tree.radiusSearch(q, Radius); },  //
      [] { return std::tuple<>{}; });
}

static constexpr size_t kMaxTreeSize = 1ULL << 14;

BENCHMARK(BM_kdtree_build<2>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_build<3>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_build<5>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();

BENCHMARK(BM_kdtree_nn_search<2>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_nn_search<3>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_nn_search<5>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();

BENCHMARK(BM_kdtree_knn_search<2, 2>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_knn_search<2, 8>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_knn_search<2, 32>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_knn_search<2, 128>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();

BENCHMARK(BM_kdtree_radius_search<2, 10>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_radius_search<2, 100>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();
BENCHMARK(BM_kdtree_radius_search<2, 1000>)->RangeMultiplier(2)->Range(1, kMaxTreeSize)->Complexity();

}  // namespace test::kdtree::benchmark

BENCHMARK_MAIN();
