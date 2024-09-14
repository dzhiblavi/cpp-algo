#pragma once

#include "algo/utility/visit.h"

namespace algo::utility {

template <typename Variant>
struct MatchImpl {
    Variant var;
};

template <typename Variant, typename... Funcs>
decltype(auto) operator+(MatchImpl<Variant> v, overloaded<Funcs...> f) {
    return visit(v.var, std::move(f));
}

}  // namespace algo::utility

#define match(v) ::algo::utility::MatchImpl<decltype(v)>{.var = v} + ::algo::utility::overloaded
