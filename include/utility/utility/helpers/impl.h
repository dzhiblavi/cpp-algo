#pragma once

#include "utility/helpers/front.h"

#include <type_traits>

namespace util::hlp::impl {

template <typename From, typename To>
struct PropagateConst : public std::remove_const<To> {};

template <typename From, typename To>
struct PropagateConst<const From, To> {
  using type = const To;
};

template <typename From, typename To>
struct PropagateConst<const From&, To> {
  using type = const To;
};

template <typename From, typename To>
struct PropagateConst<const From&&, To> {
  using type = const To;
};

}  // namespace util::hlp::impl