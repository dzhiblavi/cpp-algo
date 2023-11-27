#pragma once

#include "utility/constexpr_for.h"
#include "value_or_error/value_or_error.h"

#include <tuple>

namespace voe {

/**
 * @brief Combine two ValueOrError types
 *
 * For example, consider the following snippet:
 * @code
 * using A = ValueOrError<char, int, short>;
 * A Foo() { ... }
 *
 * using B = ValueOrError<void, long, int>;
 * B Bar() { ... }
 *
 * using C = Union<float, A, B>;
 * static_assert(std::is_same_v<C, ValueOrError<float, int, short, long>>);
 *
 * C Baz() {
 *   RETURN_IF_ERROR(Foo());
 *   RETURN_IF_ERROR(Bar());
 *   return 10.f;
 * }
 * @endcode
 */
template <typename ValueType, typename... VoEOrErrorTypes>
using Union =
    detail::TransferTemplate<detail::list::set::unite<detail::ErrorTypes<VoEOrErrorTypes>...>, ValueOrError, ValueType>;

template <typename VoE, typename... VoEs>
Union<void, std::decay_t<VoE>, std::decay_t<VoEs>...> firstError(VoE& voe, VoEs&... voes) {
  if (voe.HasAnyError()) {
    return std::move(voe).DiscardValue();
  }

  if constexpr (sizeof...(voes) == 0) {
    return {};
  } else {
    return firstError(voes...);
  }
}

template <typename... VoEs>
using InvertResultType = Union<
    util::tpl::applyToTemplate<
        std::tuple, util::list::filter<util::list::list<typename VoEs::value_type...>, detail::IsNotVoidPredicate>>,
    VoEs...>;

template <typename... VoEs>
InvertResultType<std::decay_t<VoEs>...> invert(std::tuple<VoEs...>&& voes_tpl) noexcept {
  using namespace util::list;
  using namespace util::tpl;

  if (auto error = std::apply([](auto&... refs) { return firstError(refs...); }, voes_tpl); error.HasAnyError())
      [[unlikely]] {
    return std::move(error);
  }

  using Voes = list<std::decay_t<VoEs>...>;
  using ValueVoes = filter<Voes, detail::IsNotVoidVoePredicate>;
  using ValueVoesTuple = applyToTemplate<std::tuple, ValueVoes>;

  // should have default "empty" state constructor
  ValueVoesTuple voe_tuple{};

  util::constexprFor<size_t{0}, size<Voes>, size_t{1}>([&, source = std::move(voes_tpl)](auto sourceIndex) mutable {
    if constexpr (!std::is_same_v<typename get<Voes, sourceIndex>::value_type, void>) {
      constexpr size_t voidCount =
          sourceIndex.value - size<filter<prefix<Voes, sourceIndex.value>, detail::IsNotVoidVoePredicate>>;
      constexpr size_t destIndex = sourceIndex - voidCount;
      std::get<destIndex>(voe_tuple) = std::move(std::get<sourceIndex>(source));
    }
  });

  // convert std::tuple<Voe<x>, ...> -> std::tuple<x...>;
  return std::apply(
      [](auto&&... voes) { return std::make_tuple(std::move(voes.GetValue())...); }, std::move(voe_tuple));
}

template <typename... VoEs>
auto invert(VoEs&&... voes) noexcept {
  return invert(std::forward_as_tuple(voes...));
}

}  // namespace voe

#define RETURN_IF_ERROR(expr)               \
  do {                                      \
    auto&& err = (expr);                    \
    if (err.HasAnyError()) [[unlikely]] {   \
      return std::move(err).DiscardValue(); \
    }                                       \
  } while (0)

#define ASSIGN_OR_RETURN_ERROR(var, expr)   \
  do {                                      \
    auto&& err = (expr);                    \
    if (err.HasAnyError()) [[unlikely]] {   \
      return std::move(err).DiscardValue(); \
    }                                       \
    assert(!err.IsEmpty());                 \
    var = std::move(err.GetValue());        \
  } while (0)