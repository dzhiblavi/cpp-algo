#ifndef ALGO_TEST_COMPARE_RANGE_ENGINES_H
#define ALGO_TEST_COMPARE_RANGE_ENGINES_H

#include <gtest/gtest.h>
#include <cstddef>

#include "generate.h"

namespace algo::test {

template <typename EngineA, typename EngineB, size_t NDims>
void CompareRangeEnginesImmutable(const size_t (&dims)[NDims], size_t samples) {
  auto as = test::generate<int, NDims>(-100, 100, dims);
  EngineA engine_a(as);
  EngineB engine_b(as);
  size_t ql[NDims], qr[NDims];
  while (samples--) {
      test::random_range(ql, qr, dims);
      int query_engine_a = engine_a.get(ql, qr);
      int query_engine_b = engine_b.get(ql, qr);
      ASSERT_EQ(query_engine_a, query_engine_b);
  }
}

template <typename EngineA, typename EngineB, size_t NDims>
void CompareRangeEnginesMutable(const size_t (&dims)[NDims], size_t samples) {
  auto as = test::generate<int, NDims>(-100, 100, dims);
  EngineA engine_a(as);
  EngineB engine_b(as);
  size_t ql[NDims], qr[NDims], idxs[NDims];
  while (samples--) {
      test::random_range(ql, qr, dims);
      int query_engine_a = engine_a.get(ql, qr);
      int query_engine_b = engine_b.get(ql, qr);
      ASSERT_EQ(query_engine_a, query_engine_b);

      int value = std::uniform_int_distribution<int>(-100, 100)(generator());
      test::random_index(idxs, dims);
      engine_a.set(idxs, value);
      engine_b.set(idxs, value);
  }
}

}  // namespace algo::test

#endif  // ALGO_TEST_COMPARE_RANGE_ENGINES_H

