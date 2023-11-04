#pragma once

#include <type_traits>

namespace algo::utility {

namespace detail {

template <typename T, template <typename> typename Template>
struct IsInstanceOfTemplate : std::false_type {};

template <typename V, template <typename> typename Template>
struct IsInstanceOfTemplate<Template<V>, Template> : std::true_type {};

} // namespace detail

template <typename T, template <typename> typename Template>
[[maybe_unused]] static constexpr bool isInstanceOfTemplate =
    detail::IsInstanceOfTemplate<T, Template>::value;

} // namespace algo::utility