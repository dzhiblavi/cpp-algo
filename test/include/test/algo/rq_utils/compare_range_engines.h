#pragma once

#include "algo/common/types.h"
#include "algo/utility/nd_container.h"
#include "generate.h"

#include <cstddef>
#include <gtest/gtest.h>

namespace test::rq_utils {

template <typename EngineA, typename EngineB, typename Element = int, typename Output = int, size_t NDims = 1>
void compareRangeEnginesImmutable(const std::array<size_t, NDims>& dims, size_t samples) {
  auto as = generate<Element, NDims>(-100, 100, dims);
  EngineA engine_a{algo::utility::asView(as)};
  EngineB engine_b{algo::utility::asView(as)};

  auto a_query_handle = engine_a.getRangeQueryHandle();
  auto b_query_handle = engine_b.getRangeQueryHandle();
  algo::types::Index<NDims> ql, qr;

  while (samples--) {
    randomRange(ql, qr, dims);
    a_query_handle.getRange().left = b_query_handle.getRange().left = ql;
    a_query_handle.getRange().right = b_query_handle.getRange().right = qr;

    auto query_engine_a = Output(engine_a.query(a_query_handle));
    auto query_engine_b = Output(engine_b.query(b_query_handle));
    ASSERT_EQ(query_engine_a, query_engine_b);
  }
}

template <typename EngineA, typename EngineB, typename Element = int, typename Output = int, size_t NDims = 1>
void compareRangeEnginesMutable(const std::array<size_t, NDims>& dims, size_t samples) {
  auto as = generate<Element, NDims>(-100, 100, dims);
  EngineA engine_a{algo::utility::asView(as)};
  EngineB engine_b{algo::utility::asView(as)};

  auto a_range_query_handle = engine_a.getRangeQueryHandle();
  auto b_range_query_handle = engine_b.getRangeQueryHandle();
  algo::types::Index<NDims> ql, qr, idxs;

  auto a_query_handle = engine_a.getQueryHandle();
  auto b_query_handle = engine_b.getQueryHandle();

  while (samples--) {
    randomRange(ql, qr, dims);
    a_range_query_handle.getRange().left = b_range_query_handle.getRange().left = ql;
    a_range_query_handle.getRange().right = b_range_query_handle.getRange().right = qr;

    auto query_engine_a = Output(engine_a.query(a_range_query_handle));
    auto query_engine_b = Output(engine_b.query(b_range_query_handle));
    ASSERT_EQ(query_engine_a, query_engine_b);

    auto value = std::uniform_int_distribution<Element>(-100, 100)(utility::random::generator());
    randomIndex(idxs, dims);
    a_query_handle.getIndex() = b_query_handle.getIndex() = idxs;

    engine_a.update(a_query_handle, value);
    engine_b.update(b_query_handle, value);
  }
}

template <typename EngineA, typename EngineB, typename Element = int, typename Output = int, size_t NDims = 1>
void compareRangeEnginesMutableRange(const std::array<size_t, NDims>& dims, size_t samples) {
  auto as = generate<Element, NDims>(-10, 10, dims);
  EngineA engine_a{algo::utility::asView(as)};
  EngineB engine_b{algo::utility::asView(as)};

  auto a_query_handle = engine_a.getRangeQueryHandle();
  auto b_query_handle = engine_b.getRangeQueryHandle();
  algo::types::Index<NDims> ql, qr;

  while (samples--) {
    randomRange(ql, qr, dims);
    a_query_handle.getRange().left = b_query_handle.getRange().left = ql;
    a_query_handle.getRange().right = b_query_handle.getRange().right = qr;

    auto query_engine_a = Output(engine_a.query(a_query_handle));
    auto query_engine_b = Output(engine_b.query(b_query_handle));
    ASSERT_EQ(query_engine_a, query_engine_b);

    randomRange(ql, qr, dims);
    a_query_handle.getRange().left = b_query_handle.getRange().left = ql;
    a_query_handle.getRange().right = b_query_handle.getRange().right = qr;
    auto value = std::uniform_int_distribution<Element>(-10, 10)(utility::random::generator());

    engine_a.updateRange(a_query_handle, value);
    engine_b.updateRange(b_query_handle, value);
  }
}

}  // namespace test::rq_utils
