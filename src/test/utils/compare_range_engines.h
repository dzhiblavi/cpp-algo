#ifndef ALGO_TEST_COMPARE_RANGE_ENGINES_H
#define ALGO_TEST_COMPARE_RANGE_ENGINES_H

#include <gtest/gtest.h>
#include <cstddef>

#include "generate.h"

namespace algo::test {

template <
  typename EngineA, typename EngineB,
  typename Element = int, typename Output = int, size_t NDims = 1>
void CompareRangeEnginesImmutable(const size_t (&dims)[NDims], size_t samples) {
  auto as = test::generate<Element, NDims>(-100, 100, dims);
  EngineA engine_a(as);
  EngineB engine_b(as);
  size_t ql[NDims], qr[NDims];
  while (samples--) {
      test::random_range(ql, qr, dims);
      auto query_engine_a = Output(engine_a.query(ql, qr));
      auto query_engine_b = Output(engine_b.query(ql, qr));
      ASSERT_EQ(query_engine_a, query_engine_b);
  }
}

template <
  typename EngineA, typename EngineB,
  typename Element = int, typename Output = int, size_t NDims = 1>
void CompareRangeEnginesMutable(const size_t (&dims)[NDims], size_t samples) {
  auto as = test::generate<Element, NDims>(-100, 100, dims);
  EngineA engine_a(as);
  EngineB engine_b(as);
  size_t ql[NDims], qr[NDims], idxs[NDims];
  while (samples--) {
      test::random_range(ql, qr, dims);
      auto query_engine_a = Output(engine_a.query(ql, qr));
      auto query_engine_b = Output(engine_b.query(ql, qr));
      ASSERT_EQ(query_engine_a, query_engine_b);

      auto value = std::uniform_int_distribution<Element>(-100, 100)(generator());
      test::random_index(idxs, dims);
      engine_a.update(idxs, value);
      engine_b.update(idxs, value);
  }
}

template <
  typename EngineA, typename EngineB,
  typename Element = int, typename Output = int, size_t NDims = 1>
void CompareRangeEnginesMutableRange(const size_t (&dims)[NDims], size_t samples) {
  auto as = test::generate<Element, NDims>(-10, 10, dims);
  EngineA engine_a(as);
  EngineB engine_b(as);
  size_t ql[NDims], qr[NDims];
  while (samples--) {
      test::random_range(ql, qr, dims);
      auto query_engine_a = Output(engine_a.query(ql, qr));
      auto query_engine_b = Output(engine_b.query(ql, qr));
      ASSERT_EQ(query_engine_a, query_engine_b);

      test::random_range(ql, qr, dims);
      auto value = std::uniform_int_distribution<Element>(-10, 10)(generator());
      engine_a.update_range(ql, qr, value);
      engine_b.update_range(ql, qr, value);
  }
}

}  // namespace algo::test

#endif  // ALGO_TEST_COMPARE_RANGE_ENGINES_H

