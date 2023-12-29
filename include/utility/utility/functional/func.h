#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace util::functional {

/**
 * @brief Functional composition.
 * Given g: a -> b and f : b -> c computes f*g: a -> c such that (f*g)(a) = f(g(a))
 */
template <typename F, typename G>
[[nodiscard]] auto compose(F&& f, G&& g) {
  return [f = std::forward<F>(f), g = std::forward<G>(g)](auto&& arg) mutable {
    return f(g(std::forward<decltype(arg)>(arg)));
  };
}

/**
 * @brief Alternative bind_front (with a single operator() overload).
 */
template <typename F, typename... Args>
[[nodiscard]] auto bind_front(F&& f, Args&&... args) {
  return [f = std::forward<F>(f), ... args = std::forward<Args>(args)]<typename... Tail>(Tail&&... tail) mutable {
    return f(std::forward<Args>(args)..., std::forward<Tail>(tail)...);
  };
}

template <typename F>
struct CallableTraits;

template <typename T, typename R, typename... Args>
struct CallableTraits<R (T::*)(Args...)> {
  using result_type = T;
  using args_tuple = std::tuple<Args...>;
};

namespace detail {

template <size_t Offset, size_t... Idxs, typename... Args>
static auto sub_tuple(std::tuple<Args...>& src, std::index_sequence<Idxs...>) {
  return std::make_tuple(std::move(std::get<Offset + Idxs>(src))...);
}

template <typename F, typename... Args, typename Traits = CallableTraits<decltype(&F::operator())>>
decltype(auto) fn(F&& f, Args&&... bound) {
  using Tuple = typename Traits::args_tuple;
  using First = std::tuple_element_t<0, Tuple>;

  if constexpr (sizeof...(bound) == std::tuple_size_v<Tuple>) {
    return f(std::forward<Args>(bound)...);
  } else {
    return [f = std::forward<F>(f), ... bound = std::forward<Args>(bound)](First arg) mutable {
      return fn(std::forward<F>(f), std::forward<Args>(bound)..., std::forward<First>(arg));
    };
  }
}

}  // namespace detail

template <typename F>
class Function {
 public:
  explicit Function(F f) : fun_{std::move(f)} {}

  decltype(auto) operator()(auto&& arg) {
    return fun_(std::forward<decltype(arg)>(arg));
  }

  decltype(auto) operator()(auto&& arg) const {
    return fun_(std::forward<decltype(arg)>(arg));
  }

  template <typename Fu, typename G>
  requires std::is_same_v<std::decay_t<Fu>, Function<F>> || std::is_same_v<std::decay_t<G>, Function<F>>
  friend auto operator*(Fu&& f, G&& g) {
    return compose(std::forward<Fu>(f), std::forward<G>(g));
  }

 private:
  F fun_;
};

template <typename F>
decltype(auto) fn(F&& f) {
  return Function(detail::fn(std::forward<F>(f)));
}

}  // namespace util::functional
