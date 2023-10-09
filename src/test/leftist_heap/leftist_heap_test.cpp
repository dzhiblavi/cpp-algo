#include "algo/leftist_heap/leftist_heap.h"

#include <compare>
#include <gtest/gtest.h>
#include <iostream>

namespace algo::leftist_heap::unit {

struct IntValue : public LeftistHeapHook<IntValue> {
  auto operator<(const IntValue& rhs) const noexcept { return value < rhs.value; }

  int value;
};

using Heap = IntrusiveLeftistHeap<IntValue>;

TEST(LeftistHeapTest, InsertTwoElements) {
  Heap heap;

  auto x = IntValue{.value = 0};
  auto y = IntValue{.value = 10};

  heap.insert(x);
  heap.insert(y);

  Heap::debug::checkInvariants(heap);
  EXPECT_FALSE(heap.isEmpty());
  EXPECT_EQ(heap.top().value, 0);
}

TEST(LeftistHeapTest, MergeManyElements) {
  Heap heap;

  std::array<IntValue, 5> a = {
      IntValue{.value = 40}, IntValue{.value = 20}, IntValue{.value = 30}, IntValue{.value = 10}, IntValue{.value = 0},
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
      IntValue{.value = 40}, IntValue{.value = 20}, IntValue{.value = 30}, IntValue{.value = 10}, IntValue{.value = 0},
  };

  for (size_t i = 0; i < a.size(); ++i) {
    heap.insert(a[i]);
    Heap::debug::checkInvariants(heap);
    EXPECT_FALSE(heap.isEmpty());
    EXPECT_EQ(heap.top().value, std::min_element(a.begin(), a.begin() + i + 1)->value);
  }
}

}  // namespace algo::leftist_heap::unit