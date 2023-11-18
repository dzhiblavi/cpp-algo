#pragma once

#include <type_traits>
#include <utility>

namespace test::util::list {

template <typename... Args>
struct list {};

namespace impl {

template <typename TL>
struct Size;

template <typename... Ts>
struct Size<list<Ts...>> : public std::integral_constant<size_t, sizeof...(Ts)> {};

template <typename TL, typename T>
struct Contains;

template <typename T, typename... Ts>
struct Contains<list<Ts...>, T> : public std::integral_constant<bool, (std::is_same_v<T, Ts> || ...)> {};

template <template <typename...> typename Template, typename TL>
struct ApplyToTemplate;

template <template <typename...> typename Template, typename... Ts>
struct ApplyToTemplate<Template, list<Ts...>> {
  using type = Template<Ts...>;
};

template <typename... T1, typename... T2>
auto concat2(list<T1...>, list<T2...>) -> list<T1..., T2...>;

template <typename... TLs>
struct Concat;

template <>
struct Concat<> {
  using type = list<>;
};

template <typename TL, typename... TLs>
struct Concat<TL, TLs...> {
  using rest = typename Concat<TLs...>::type;
  using type = decltype(concat2(TL{}, rest{}));
};

}  // namespace impl

template <typename TL>
[[maybe_unused]] static constexpr size_t size = impl::Size<TL>::value;

template <typename TL, typename T>
[[maybe_unused]] static constexpr bool contains = impl::Contains<TL, T>::value;

template <template <typename...> typename Template, typename TL>
using apply = typename impl::ApplyToTemplate<Template, TL>::type;

template <typename... TLs>
using concat = typename impl::Concat<TLs...>::type;

template <typename TL, typename T>
using push_back = concat<TL, list<T>>;

template <typename TL, typename T>
using push_front = concat<list<T>, TL>;

namespace impl {

template <typename... TLs>
struct Flatten;

template <typename TL, typename... TLs>
struct Flatten<list<TL, TLs...>> {
  using rest = typename Flatten<list<TLs...>>::type;
  using type = concat<TL, rest>;
};

template <>
struct Flatten<list<>> {
  using type = list<>;
};

template <typename T>
struct SameTypePredicate {
  template <typename U>
  using type = std::is_same<U, T>;
};

template <typename Pred, typename TL>
struct Filter;

template <typename Pred>
struct Filter<Pred, list<>> {
  using type = list<>;
};

template <typename Pred, typename T1, typename... Ts>
struct Filter<Pred, list<T1, Ts...>> {
  using rest = typename Filter<Pred, list<Ts...>>::type;
  using conc = concat<list<T1>, rest>;
  using pred = typename Pred::template type<T1>;
  using type = std::conditional_t<pred::value, rest, conc>;
};

template <typename TL1, typename TL2>
struct Product2;

template <typename TL2>
struct Product2<list<>, TL2> {
  using type = list<>;
};

template <typename TL1, typename... TLs>
struct Product2<list<TL1, TLs...>, list<>> {
  using type = list<>;
};

template <typename E, typename... Es, typename... TLs>
struct Product2<list<E, Es...>, list<TLs...>> {
  using right = list<TLs...>;
  using rest = typename Product2<list<Es...>, right>::type;
  using type = concat<list<concat<list<E>, TLs>>..., rest>;
};

template <typename... TLs>
struct Product;

template <typename... Ts>
struct Product<list<Ts...>> {
  using type = list<list<Ts>...>;
};

template <typename TL, typename... TLs>
struct Product<TL, TLs...> {
  using rest = typename Product<TLs...>::type;
  using type = typename Product2<TL, rest>::type;
};

template <typename TL>
struct Unique;

template <>
struct Unique<list<>> {
  using type = list<>;
};

template <typename T, typename... Ts>
struct Unique<list<T, Ts...>> {
  using rest = typename Unique<list<Ts...>>::type;
  using type = std::conditional_t<contains<list<Ts...>, T>, rest, push_back<rest, T>>;
};

template <typename TL1, typename TL2>
struct Intersect2;

template <typename TL1>
struct Intersect2<TL1, list<>> {
  using type = list<>;
};

template <typename TL1, typename T, typename... Ts>
struct Intersect2<TL1, list<T, Ts...>> {
  using rest = typename Intersect2<TL1, list<Ts...>>::type;
  using type = std::conditional_t<contains<TL1, T>, push_back<rest, T>, rest>;
};

template <typename... TLs>
struct Intersect;

template <typename TL>
struct Intersect<TL> {
  using type = TL;
};

template <typename TL, typename... TLs>
struct Intersect<TL, TLs...> {
  using rest = typename Intersect<TLs...>::type;
  using type = typename Intersect2<TL, rest>::type;
};

template <typename From, typename TL>
struct Subtract;

template <typename TL, typename T, typename... Ts>
struct Subtract<list<T, Ts...>, TL> {
  using rest = typename Subtract<list<Ts...>, TL>::type;
  using type = std::conditional_t<contains<TL, T>, rest, push_back<rest, T>>;
};

}  // namespace impl

template <typename... TLs>
using flatten = typename impl::Flatten<TLs...>::type;

template <typename Pred, typename TL>
using filter_pred = typename impl::Filter<Pred, TL>::type;

template <typename T, typename TL>
using filter = filter_pred<impl::SameTypePredicate<T>, TL>;

template <typename... TLs>
using product = typename impl::Product<TLs...>::type;

template <typename TL>
using unique = typename impl::Unique<TL>::type;

template <typename... TLs>
using intersect = typename impl::Intersect<TLs...>::type;

template <typename... TLs>
using unite = unique<concat<TLs...>>;

template <typename From, typename TL>
using subtract = typename impl::Subtract<From, TL>::type;

namespace unit {

static_assert(0 == size<list<>>);
static_assert(1 == size<list<int>>);
static_assert(2 == size<list<int, int>>);
static_assert(2 == size<list<int, float>>);

static_assert(!contains<list<>, int>);
static_assert(contains<list<int>, int>);
static_assert(!contains<list<int>, float>);
static_assert(contains<list<int, int, float, int, int>, float>);
static_assert(contains<list<int, int, int, int, float>, float>);

static_assert(std::is_same_v<list<>, apply<list, list<>>>);
static_assert(std::is_same_v<list<int>, apply<list, list<int>>>);
static_assert(std::is_same_v<list<int, float>, apply<list, list<int, float>>>);

static_assert(std::is_same_v<concat<>, list<>>);
static_assert(std::is_same_v<concat<list<>, list<>>, list<>>);
static_assert(std::is_same_v<concat<list<>, list<int>, list<>>, list<int>>);
static_assert(std::is_same_v<concat<list<int>, list<>, list<float>>, list<int, float>>);

static_assert(std::is_same_v<list<>, flatten<list<list<>, list<>, list<>>>>);
static_assert(std::is_same_v<list<int>, flatten<list<list<>, list<int>, list<>>>>);
static_assert(std::is_same_v<list<int, float, int, int>, flatten<list<list<int>, list<float>, list<int, int>>>>);

static_assert(std::is_same_v<list<int, int, int>, filter<void, list<int, void, int, int>>>);

static_assert(std::is_same_v<list<>, product<list<>, list<>>>);
static_assert(std::is_same_v<list<>, product<list<int>, list<>>>);
static_assert(std::is_same_v<list<list<int, int>>, product<list<int>, list<int>>>);
static_assert(std::is_same_v<list<list<int, char, float>>, product<list<int>, list<char>, list<float>>>);
static_assert(std::is_same_v<
              list<list<int, char>, list<int, short>, list<float, char>, list<float, short>>,
              product<list<int, float>, list<char, short>>>);

static_assert(std::is_same_v<list<>, unique<list<>>>);
static_assert(std::is_same_v<list<int, float, char>, unique<list<int, int, float, char, int, char, float, int>>>);

static_assert(std::is_same_v<list<>, intersect<list<>, list<>>>);
static_assert(std::is_same_v<list<>, intersect<list<int>, list<float>>>);
static_assert(std::is_same_v<list<int>, intersect<list<int, char>, list<float, int>>>);
static_assert(std::is_same_v<list<int, float>, intersect<list<int, char, float>, list<float, int>>>);

static_assert(std::is_same_v<list<>, unite<list<>, list<>>>);
static_assert(std::is_same_v<list<int>, unite<list<>, list<int>>>);
static_assert(std::is_same_v<list<int>, unite<list<int>, list<>>>);
static_assert(std::is_same_v<list<int>, unite<list<int>, list<int>>>);
static_assert(std::is_same_v<list<int, float>, unite<list<int, float>, list<float, int>>>);

}  // namespace unit

}  // namespace test::util::list