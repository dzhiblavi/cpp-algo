#include "algo/leftist_heap/leftist_heap.h"

#include <array>
#include <compare>
#include <gtest/gtest.h>
#include <iostream>

namespace test::leftist_heap::unit {

namespace lheap = ::algo::leftist_heap;

struct IntValue : public lheap::LeftistHeapHook<IntValue> {
  explicit IntValue(int value) : value(value) {}

  auto operator<(const IntValue& rhs) const noexcept {
    return value < rhs.value;
  }

  int value;
};

using Heap = lheap::IntrusiveLeftistHeap<IntValue>;

TEST(LeftistHeapTest, InsertTwoElements) {
  Heap heap;

  auto x = IntValue(0);
  auto y = IntValue(10);

  heap.insert(x);
  heap.insert(y);

  Heap::debug::checkInvariants(heap);
  EXPECT_FALSE(heap.isEmpty());
  EXPECT_EQ(heap.top().value, 0);
}

TEST(LeftistHeapTest, MergeManyElements) {
  Heap heap;

  std::array<IntValue, 5> a = {
      IntValue(40), IntValue(20), IntValue(30), IntValue(10), IntValue(0),
  };

  for (size_t i = 0; i < a.size(); ++i) {
    heap = Heap::merge(heap, Heap::singleton(a[i]));
    Heap::debug::checkInvariants(heap);
    EXPECT_FALSE(heap.isEmpty());
    EXPECT_EQ(heap.top().value, std::min_element(a.begin(), a.begin() + i + 1)->value);
  }
}

TEST(LeftistHeapTest, InsertManyElements) {
  Heap heap;

  std::array<IntValue, 5> a = {
      IntValue(40), IntValue(20), IntValue(30), IntValue(10), IntValue(0),
  };

  for (size_t i = 0; i < a.size(); ++i) {
    heap.insert(a[i]);
    Heap::debug::checkInvariants(heap);
    EXPECT_FALSE(heap.isEmpty());
    EXPECT_EQ(heap.top().value, std::min_element(a.begin(), a.begin() + i + 1)->value);
  }
}

}  // namespace test::leftist_heap::unit
