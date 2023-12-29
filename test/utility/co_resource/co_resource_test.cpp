#include "utility/co_resource/co_resource.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

namespace util {

CoResource<std::mutex> lockGuard(std::mutex& m) {
  m.lock();
  co_yield m;
  m.unlock();
}

TEST(CoResource, Basic) {
  int total = 0;
  const int thread_count = 1;
  const int iterations = 1;
  const int expected_total = thread_count * iterations;

  std::mutex m;
  auto action = [&] {
    for (int i = 0; i < iterations; ++i) {
      auto lock = lockGuard(m);
      ++total;
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back(action);
  }
  std::ranges::for_each(threads, &std::thread::join);

  EXPECT_EQ(expected_total, total);
}

}  // namespace util
