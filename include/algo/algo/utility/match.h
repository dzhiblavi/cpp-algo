#pragma once

#include "algo/utility/visit.h"

namespace algo::utility {

template <typename T>
struct is_variant : std::false_type {};

template <typename... Args>
struct is_variant<std::variant<Args...>> : std::true_type {};

template <typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

template <typename... Underlying>
struct MatchImpl;

template <typename T>
requires is_variant_v<std::decay_t<T>>
struct MatchImpl<T> {
    explicit MatchImpl(T v)
        : var(v) {}

    template <typename... Funcs>
    decltype(auto) operator()(overloaded<Funcs...> f) {
        return visit(var, std::move(f));
    }

    T var;
};

template <>
struct MatchImpl<bool> {
    explicit MatchImpl(bool value)
        : value(value) {}

    template <typename... Funcs>
    decltype(auto) operator()(overloaded<Funcs...> f) {
        if (value) {
            return f(std::true_type());
        } else {
            return f(std::false_type());
        }
    }

    bool value;
};

template <typename T, typename... Funcs>
decltype(auto) operator+(MatchImpl<T> v, overloaded<Funcs...> f) {
    return v(std::move(f));
}

}  // namespace algo::utility

#define match(v) ::algo::utility::MatchImpl<decltype(v)>(v) + ::algo::utility::overloaded

#define if_true [&](std::true_type)
#define if_false [&](std::false_type)
