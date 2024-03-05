#include "di/di.h"

#include <gtest/gtest.h>

namespace di {

struct A0 {
  A0() = default;
};

struct A1 {
  A1(int x) : x{x} {}
  int x;
};

struct A2 {
  A2(int x, float y) : x{x}, y{y} {}

  int x;
  float y;
};

struct Interface {
  virtual ~Interface() = default;
  virtual int foo() = 0;
};

struct Implementation : public Interface {
  explicit Implementation(int value) : value{value} {}

  int foo() override {
    return value;
  }

  int value;
};

struct AggregateUnique {
  explicit AggregateUnique(std::unique_ptr<Interface> subobj) : subobj_{std::move(subobj)} {}

  int fooBar() {
    return subobj_->foo();
  }

 private:
  std::unique_ptr<Interface> subobj_;
};

struct AggregateShared {
  explicit AggregateShared(std::shared_ptr<Interface> subobj) : subobj_{std::move(subobj)} {}

  int fooBar() {
    return subobj_->foo();
  }

 private:
  std::shared_ptr<Interface> subobj_;
};

TEST(Detail, isNArgsConstructible) {
  static_assert(di::detail::isNArgsConstructible<A0, 0>());
  static_assert(di::detail::isNArgsConstructible<A1, 1>());
  static_assert(di::detail::isNArgsConstructible<A2, 2>());
}

TEST(Detail, getNumConstructorArguments) {
  static_assert(0 == di::detail::getNumConstructorArguments<A0>());
  static_assert(1 == di::detail::getNumConstructorArguments<A1>());
  static_assert(2 == di::detail::getNumConstructorArguments<A2>());
}

TEST(Injector, injectValue) {
  auto injector = di::makeInjector(
      di::bind<int>.to(10),
      di::bind<float>.to(22.f)  //
  );

  [[maybe_unused]] auto a0 = injector.create<A0>();
  auto a1 = injector.create<A1>();
  EXPECT_EQ(10, a1.x);

  auto a2 = injector.create<A2>();
  EXPECT_EQ(10, a2.x);
  EXPECT_EQ(22.f, a2.y);

  auto a2u = injector.create<std::unique_ptr<A2>>();
  EXPECT_EQ(10, a2u->x);
  EXPECT_EQ(22.f, a2u->y);

  auto a2s = injector.create<std::shared_ptr<A2>>();
  EXPECT_EQ(10, a2s->x);
  EXPECT_EQ(22.f, a2s->y);
}

TEST(Injector, Recursive) {
  struct X {
    X(A1 a1, A2 a2) : a1{a1}, a2{a2} {}

    A1 a1;
    A2 a2;
  };

  auto injector = di::makeInjector(
      di::bind<int>.to(10),
      di::bind<float>.to(22.f)  //
  );

  auto x = injector.create<X>();
  EXPECT_EQ(10, x.a1.x);
}

TEST(Injector, injectPointer) {
  struct X {
    X(int* raw_ptr) : value_ptr{raw_ptr} {}
    std::unique_ptr<int> value_ptr;
  };

  auto injector = di::makeInjector(  //
      di::bind<int>.to(10)           //
  );

  auto x = injector.create<X>();
  EXPECT_EQ(10, *x.value_ptr);
}

TEST(Injector, injectComplexPointer) {
  struct Y {
    Y(int* y) : y{y} {}
    std::unique_ptr<int> y;
  };

  struct X {
    X(int* x, Y y) : x{x}, y{std::move(y)} {}
    std::unique_ptr<int> x;
    Y y;
  };

  auto injector = di::makeInjector(  //
      di::bind<int>.to(10)           //
  );

  {
    auto x = injector.create<X>();
    EXPECT_EQ(10, *x.x);
    EXPECT_EQ(10, *x.y.y);

    *x.x = 239;
    *x.y.y = 1030;
    EXPECT_EQ(239, *x.x);
    EXPECT_EQ(1030, *x.y.y);
  }

  {
    auto* x = injector.create<X*>();
    EXPECT_EQ(10, *x->x);
    EXPECT_EQ(10, *x->y.y);

    *x->x = 239;
    *x->y.y = 1030;
    EXPECT_EQ(239, *x->x);
    EXPECT_EQ(1030, *x->y.y);
  }
}

TEST(Injector, injectReference) {
  auto injector = di::makeInjector(             //
      di::bind<int>.to(239),                    //
      di::bind<Interface>.to<Implementation>()  //
  );

  auto aggr = injector.create<std::shared_ptr<AggregateShared>>();
  EXPECT_EQ(239, aggr->fooBar());
}

TEST(Injector, injectInterfaceUnique) {
  auto injector = di::makeInjector(
      di::bind<int>.to(239),                    //
      di::bind<Interface>.to<Implementation>()  //
  );

  auto aggr = injector.create<AggregateUnique>();
  EXPECT_EQ(239, aggr.fooBar());
}

TEST(Injector, injectInterfaceShared) {
  auto injector = di::makeInjector(
      di::bind<int>.to(239),                    //
      di::bind<Interface>.to<Implementation>()  //
  );

  auto aggr = injector.create<std::unique_ptr<AggregateShared>>();
  EXPECT_EQ(239, aggr->fooBar());
}

}  // namespace di
