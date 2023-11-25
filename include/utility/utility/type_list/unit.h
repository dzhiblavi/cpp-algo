#pragma once

#include "utility/type_list/impl.h"

namespace util::list::unit {

// static_assert(0 == list<>::size());
// static_assert(1 == list<int>::size());
// static_assert(2 == list<int, int>::size());
// static_assert(2 == list<int, float>::size());

// static_assert(!list<>::contains<int>());
// static_assert(list<int>::contains<int>());
// static_assert(!list<int>::contains<float>());
// static_assert(list<int, int, float, int, int>::contains<float>());
// static_assert(list<int, int, int, int, float>::contains<float>());

static_assert(std::is_same_v<concat<>, list<>>);
static_assert(std::is_same_v<concat<list<>, list<>>, list<>>);
static_assert(std::is_same_v<concat<list<>, list<int>, list<>>, list<int>>);
static_assert(std::is_same_v<concat<list<int>, list<>, list<float>>, list<int, float>>);

static_assert(std::is_same_v<list<>, flatten<list<list<>, list<>, list<>>>>);
static_assert(std::is_same_v<list<int>, flatten<list<list<>, list<int>, list<>>>>);
static_assert(std::is_same_v<list<int, float, int, int>, flatten<list<list<int>, list<float>, list<int, int>>>>);

static_assert(std::is_same_v<list<>, set::product<list<>, list<>>>);
static_assert(std::is_same_v<list<>, set::product<list<int>, list<>>>);
static_assert(std::is_same_v<list<list<int, int>>, set::product<list<int>, list<int>>>);
static_assert(std::is_same_v<list<list<int, char, float>>, set::product<list<int>, list<char>, list<float>>>);
static_assert(std::is_same_v<
              list<list<int, char>, list<int, short>, list<float, char>, list<float, short>>,
              set::product<list<int, float>, list<char, short>>>);

static_assert(std::is_same_v<list<>, set::unique<list<>>>);
static_assert(std::is_same_v<list<char, float, int>, set::unique<list<int, int, float, char, int, char, float, int>>>);

static_assert(std::is_same_v<list<>, set::intersect<list<>, list<>>>);
static_assert(std::is_same_v<list<>, set::intersect<list<int>, list<float>>>);
static_assert(std::is_same_v<list<int>, set::intersect<list<int, char>, list<float, int>>>);
static_assert(std::is_same_v<list<int, float>, set::intersect<list<int, char, float>, list<float, int>>>);

static_assert(std::is_same_v<list<>, set::unite<list<>, list<>>>);
static_assert(std::is_same_v<list<int>, set::unite<list<>, list<int>>>);
static_assert(std::is_same_v<list<int>, set::unite<list<int>, list<>>>);
static_assert(std::is_same_v<list<int>, set::unite<list<int>, list<int>>>);
static_assert(std::is_same_v<list<float, int>, set::unite<list<int, float>, list<float, int>>>);

}  // namespace util::list::unit

// TEST(PropagateConstTest, Correctness) {
//   static_assert(std::is_same_v<void, PropagateConst<int, void>>);
//   static_assert(std::is_same_v<const int, PropagateConst<const void, int>>);
//   static_assert(std::is_same_v<const int, PropagateConst<const int&, int>>);
//   static_assert(std::is_same_v<const int, PropagateConst<const int&&, int>>);
//   static_assert(std::is_same_v<int* const, PropagateConst<const void, int*>>);
// }

// TEST(IndexToTypeTest, Correctness) {
//   static_assert(std::is_same_v<int, IndexToType<0, int>>);
//   static_assert(std::is_same_v<int, IndexToType<0, int, float, char>>);
//   static_assert(std::is_same_v<float, IndexToType<1, int, float, char>>);
//   static_assert(std::is_same_v<char, IndexToType<2, int, float, char>>);
// }

// TEST(TypeToIndexTest, Correctness) {
//   static_assert(0 == TypeToIndex<int, int>);
//   static_assert(0 == TypeToIndex<int, int, float, char>);
//   static_assert(1 == TypeToIndex<float, int, float, char>);
//   static_assert(2 == TypeToIndex<char, int, float, char>);
//   static_assert(size_t(-1) == TypeToIndex<char, int, float>);
// }

// isaSet
// TEST(AllUniqueTest, Correctness) {
//   static_assert(AllUnique<>);
//   static_assert(AllUnique<int, float*, void>);
//   static_assert(!AllUnique<int, int>);
//   static_assert(!AllUnique<int, float, int>);
//   static_assert(!AllUnique<int, float, float>);
//   static_assert(!AllUnique<float, float, int>);
//   static_assert(!AllUnique<float, int, float, int, float>);
// }

// set::subtract
// TEST(RemoveTypesTest, Correctness) {
//   static_assert(std::is_same_v<list<>, RemoveTypes<VariadicHolder<>>>);
//   static_assert(std::is_same_v<list<int>, RemoveTypes<VariadicHolder<int>, bool>>);
//   static_assert(std::is_same_v<list<>, RemoveTypes<VariadicHolder<int>, int>>);
//   static_assert(std::is_same_v<list<float, char>, RemoveTypes<VariadicHolder<int, float, short, char>, int, short>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, char, int, void*>, int>, VariadicHolder<bool, char, void*>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, int, char, void*>, int>, VariadicHolder<bool, char, void*>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, char, void*, int>, int>, VariadicHolder<bool, char, void*>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, char, int, void*>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, char, void*, int>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, int, char, void*>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, int, void*, char>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, void*, char, int>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, void*, int, char>, int, void*>, VariadicHolder<bool, char>>);
//   static_assert(std::is_same_v<RemoveTypes<list<bool, void*, int, char>, int, void*, char>, VariadicHolder<bool>>);
// }

// concat
// TEST(ConcatTest, Correctness) {
//   static_assert(std::is_same_v<Concat<>, list<>>);
//   static_assert(std::is_same_v<Concat<list<>>, VariadicHolder<>>);
//   static_assert(std::is_same_v<Concat<list<>, VariadicHolder<>>, VariadicHolder<>>);
//   static_assert(std::is_same_v<Concat<list<int>, VariadicHolder<>>, VariadicHolder<int>>);
//   static_assert(
//       std::is_same_v<Concat<list<int, float>, VariadicHolder<float, char>>, VariadicHolder<int, float, float,
//       char>>);
//   static_assert(
//       std::is_same_v<
//           Concat<list<int, float>, VariadicHolder<>, VariadicHolder<float, char>>, list<int, float, float, char>>);
// }

// unique
// TEST(RemoveDuplicatesTest, Correctness) {
//   static_assert(std::is_same_v<RemoveDuplicates<list<>>, VariadicHolder<>>);
//   static_assert(std::is_same_v<RemoveDuplicates<list<int, float, char>>, VariadicHolder<int, float, char>>);
//   static_assert(std::is_same_v<RemoveDuplicates<list<int, int>>, VariadicHolder<int>>);
//   static_assert(std::is_same_v<RemoveDuplicates<list<int, float, int>>, VariadicHolder<float, int>>);
//   static_assert(std::is_same_v<RemoveDuplicates<list<float, int, int>>, VariadicHolder<float, int>>);
//   static_assert(
//       std::is_same_v<RemoveDuplicates<list<float, int, char, int, char, float>>, VariadicHolder<int, char, float>>);
// }

// unite
// TEST(UnionTest, Correctness) {
//   static_assert(std::is_same_v<Union<>, list<>>);
//   static_assert(std::is_same_v<Union<list<>>, VariadicHolder<>>);
//   static_assert(std::is_same_v<Union<list<>, VariadicHolder<>>, VariadicHolder<>>);
//   static_assert(std::is_same_v<Union<list<int>, VariadicHolder<>>, VariadicHolder<int>>);
//   static_assert(std::is_same_v<Union<list<>, VariadicHolder<int>>, VariadicHolder<int>>);
//   static_assert(std::is_same_v<Union<list<float>, VariadicHolder<int>>, VariadicHolder<float, int>>);
//   static_assert(std::is_same_v<Union<list<float>, VariadicHolder<float>>, VariadicHolder<float>>);
//   static_assert(
//       std::is_same_v<Union<list<float, int>, VariadicHolder<int, float, char>>, VariadicHolder<int, float, char>>);
//   static_assert(std::is_same_v<
//                 Union<
//                     list<float, int>, VariadicHolder<>, VariadicHolder<int, float, char>, VariadicHolder<>,
//                     list<short, char>, VariadicHolder<std::string, float>>,
//                 list<int, short, char, std::string, float>>);
// }

// set::subset
// TEST(SubsetOfTest, Correctness) {
//   static_assert(SubsetOf<list<>, VariadicHolder<>>::value);
//   static_assert(SubsetOf<list<>, VariadicHolder<int>>::value);
//   static_assert(SubsetOf<list<>, VariadicHolder<int, float, std::string>>::value);
//   static_assert(SubsetOf<list<int>, VariadicHolder<int>>::value);
//   static_assert(SubsetOf<list<int>, VariadicHolder<int, float>>::value);
//   static_assert(SubsetOf<list<int>, VariadicHolder<float, int>>::value);
//   static_assert(SubsetOf<list<int>, VariadicHolder<float, int, float>>::value);
//   static_assert(SubsetOf<list<int, float>, VariadicHolder<float, int, float>>::value);
//   static_assert(SubsetOf<list<int, float>, VariadicHolder<char, int, float>>::value);
//   static_assert(!SubsetOf<list<int>, VariadicHolder<>>::value);
//   static_assert(!SubsetOf<list<int>, VariadicHolder<float>>::value);
//   static_assert(!SubsetOf<list<int>, VariadicHolder<float, char>>::value);
//   static_assert(!SubsetOf<list<int>, VariadicHolder<std::string, float, char>>::value);
//   static_assert(!SubsetOf<list<int>, VariadicHolder<std::string, float, char>>::value);
//   static_assert(!SubsetOf<list<int, float, char>, VariadicHolder<>>::value);
//   static_assert(!SubsetOf<list<int, float, char>, VariadicHolder<float, int>>::value);
//   static_assert(!SubsetOf<list<int, float, char>, VariadicHolder<std::string>>::value);
// }