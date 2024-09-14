#pragma once

#include <variant>

namespace algo::utility {

template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename Variant, typename... Funcs>
decltype(auto) visit(Variant&& v, Funcs&&... funcs) {
    return std::visit(
        overloaded<Funcs...>{std::forward<Funcs>(funcs)...}, std::forward<Variant>(v));
}

}  // namespace algo::utility
