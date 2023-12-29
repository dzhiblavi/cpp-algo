#include "utility/functional/func.h"

#include <gtest/gtest.h>

namespace util::functional {

TEST(Func, Composition) {
  auto f = [](int x) { return 2 * x; };
  auto g = [](int x) { return x + 1; };

  auto fg = compose(f, g);
  EXPECT_EQ(22, fg(10));

  auto gf = compose(g, f);
  EXPECT_EQ(21, gf(10));
}

TEST(Func, Playground) {
  int x = 10;

  auto f = fn([x](int y, int z) mutable { return x + y + z; });  // int -> int -> int
  {
    auto g = f(10);
    EXPECT_EQ(40, g(20));
    EXPECT_EQ(35, f(5)(20));
  }

  {
    auto g = [x](int y) mutable { return x * y; };  // int -> int
    auto fg = f * g;                                // int -> int -> int
    EXPECT_EQ(33, fg(2)(3));                        // f(g(2))(3) = f(20)(3) = 23 + 10 = 33
  }
}

}  // namespace util::functional
