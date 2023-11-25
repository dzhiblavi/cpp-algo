#include "wptr/wptr.h"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>

namespace test::unit {

struct VTable {
  vtable_boilerplate;
  vtable_mutable_method(int, run, int);
  vtable_mutable_method(int, does, std::string, float);
  vtable_declare_thunks(destroyThunk, runThunk, doesThunk);
};

struct Wptr {
  wptr_boilerplate(Wptr, VTable);
  wptr_mutable_method(int, run, int);
  wptr_mutable_method(int, does, std::string, float);
};

static_assert(sizeof(Wptr) == 2 * sizeof(void*));

class Impl1 {
 public:
  int run(int value) noexcept {
    return value * 10;
  }

  int does(std::string s, float x) {
    return s.size() + x;
  }
};

class Impl2 {
 public:
  int run(int value) noexcept {
    return value * 1000;
  }

  int does(std::string s, float x) {
    return s.size() - x;
  }
};

void callRun(Wptr& dyn) {
  std::cout << fmt::format("run(10) = {}", dyn.run(10)) << std::endl;
  std::cout << fmt::format("does(abc, 15) = {}", dyn.does("abc", 15.f)) << std::endl;
}

TEST(Wptr, Sandbox) {
  auto i1 = Wptr::make<Impl1>();
  auto i2 = Wptr::make<Impl2>();

  callRun(i1);
  callRun(i2);
}

}  // namespace test::unit