#include "algo/min_stack/min_queue.hpp"

#include <gtest/gtest.h>

namespace test::mstack::unit {

namespace mstack = ::algo::mstack;

template <
    template <typename, typename, typename> typename StackQueue,  //
    typename T, typename Strategy, typename TestFunctor>
void minStackQueueTest(TestFunctor fun) {
  StackQueue<T, std::less<T>, Strategy> stack;
  fun(stack);
}

template <template <typename, typename, typename> typename StackQueue, typename TestFunctor>
void minStackQueueTest(TestFunctor fun) {
  using list = mstack::AdaptContainer<std::list>;
  using deque = mstack::AdaptContainer<std::deque>;
  using vector = mstack::AdaptContainer<std::vector>;

  minStackQueueTest<StackQueue, int, mstack::AutomaticStrategy>(fun);

  minStackQueueTest<StackQueue, int, mstack::ForceContainerStrategy<list, true>>(fun);
  minStackQueueTest<StackQueue, int, mstack::ForceContainerStrategy<list, false>>(fun);

  minStackQueueTest<StackQueue, int, mstack::ForceContainerStrategy<deque, true>>(fun);
  minStackQueueTest<StackQueue, int, mstack::ForceContainerStrategy<deque, false>>(fun);

  // minStackQueueTest<StackQueue, int, ForceContainerStrategy<vector, true>>(fun);
  minStackQueueTest<StackQueue, int, mstack::ForceContainerStrategy<vector, false>>(fun);
}

TEST(MinStackTest, Simple) {
  minStackQueueTest<mstack::MinStack>([](auto& mstack) {
    mstack.push(5);
    mstack.push(8);
    mstack.push(2);
    mstack.push(10);
    mstack.push(1);

    EXPECT_EQ(1, mstack.min());
    EXPECT_EQ(1, mstack.top());
    mstack.pop();

    EXPECT_EQ(2, mstack.min());
    EXPECT_EQ(10, mstack.top());
    mstack.pop();

    EXPECT_EQ(2, mstack.min());
    EXPECT_EQ(2, mstack.top());
    mstack.pop();

    EXPECT_EQ(5, mstack.min());
    EXPECT_EQ(8, mstack.top());
    mstack.pop();

    EXPECT_EQ(5, mstack.min());
    EXPECT_EQ(5, mstack.top());
    mstack.pop();

    ASSERT_TRUE(mstack.isEmpty());
  });
}

TEST(MinQueueTest, Simple) {
  minStackQueueTest<mstack::MinQueue>([](auto& mqueue) {
    mqueue.push(5);
    mqueue.push(8);
    mqueue.push(2);
    mqueue.push(10);
    mqueue.push(1);

    EXPECT_EQ(1, mqueue.min());
    EXPECT_EQ(5, mqueue.top());
    mqueue.pop();

    EXPECT_EQ(1, mqueue.min());
    EXPECT_EQ(8, mqueue.top());
    mqueue.pop();

    EXPECT_EQ(1, mqueue.min());
    EXPECT_EQ(2, mqueue.top());
    mqueue.pop();

    EXPECT_EQ(1, mqueue.min());
    EXPECT_EQ(10, mqueue.top());
    mqueue.pop();

    EXPECT_EQ(1, mqueue.min());
    EXPECT_EQ(1, mqueue.top());
    mqueue.pop();

    ASSERT_TRUE(mqueue.isEmpty());
  });
}

}  // namespace test::mstack::unit