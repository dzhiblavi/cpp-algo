#include "algo/string/suffix_tree.h"
#include "test/algo/string/emaxx_suffix_tree.h"
#include "test/algo/string/random.h"
#include "test/utility/random.h"

#include <benchmark/benchmark.h>

namespace test::string::benchmark {

template <typename SuffixTree>
static void BM_suffix_tree_build(::benchmark::State& state) {
    // utility::random::resetGenerator();

    for (auto _ : state) {
        state.PauseTiming();
        auto str = generateRandomString(state.range(0), state.range(1));
        state.ResumeTiming();
        [[maybe_unused]] auto tree = SuffixTree(str);
    }

    state.SetComplexityN(state.range());
}

BENCHMARK(BM_suffix_tree_build<algo::string::SuffixTree<int>>)
    ->ArgsProduct({
        ::benchmark::CreateRange(1 << 14, 1 << 20, 2),
        //{1, 2, 6, 12, 26},
        {6},
    })
    ->Complexity();

BENCHMARK(BM_suffix_tree_build<ReferenceSuffixTree>)
    ->ArgsProduct({
        ::benchmark::CreateRange(1 << 14, 1 << 20, 2),
        //{1, 2, 6, 12, 26},
        {6},
    })
    ->Complexity();

}  // namespace test::string::benchmark

BENCHMARK_MAIN();
