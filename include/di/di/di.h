#pragma once

#include "utility/type_list.h"

#include <cstddef>
#include <memory>
#include <utility>

namespace di {

namespace detail {

template <typename Exclude, size_t Tag>
struct AnyType {
  template <typename T>
  requires(!std::is_same_v<std::decay_t<T>, Exclude>)
  operator T();
};

template <typename T, size_t N>
consteval bool isNArgsConstructible() {
  return []<size_t... Indices>(std::index_sequence<Indices...>) {
    return std::is_constructible_v<T, AnyType<T, Indices>...>;
  }(std::make_index_sequence<N>());
}

template <typename T, size_t N = 10>
consteval size_t getNumConstructorArguments() {
  if constexpr (N == 0) {
    static_assert(std::is_constructible_v<T>, "Object is not constructible from under 10 arguments");
    return 0;
  } else if constexpr (isNArgsConstructible<T, N>()) {
    return N;
  } else {
    return getNumConstructorArguments<T, N - 1>();
  }
}

template <typename T>
class ValueBinding {
 public:
  using value_types = util::list::list<T>;

  explicit ValueBinding(T value) : value_{std::move(value)} {}

  template <typename>
  const T& get([[maybe_unused]] const auto& injector) const {
    return value_;
  }

 private:
  T value_;
};

template <typename Interface, typename Implementation>
class InterfaceBinding {
 public:
  using value_types = util::list::list<std::unique_ptr<Interface>, std::shared_ptr<Interface>>;

  template <typename ValueType>
  auto get(const auto& injector) const {
    if constexpr (std::is_same_v<ValueType, std::unique_ptr<Interface>>) {
      return injector.template createUnique<Implementation>();
    } else {
      return injector.template createShared<Implementation>();
    }
  }
};

template <typename T>
struct BindLhs {
  ValueBinding<T> to(T value) const {
    return ValueBinding<T>(std::move(value));
  }

  template <std::derived_from<T> Implementation>
  InterfaceBinding<T, Implementation> to() const {
    return {};
  }
};

template <typename... Bindings>
class Injector;

template <typename Except, size_t Tag, typename... Bindings>
class Converter {
  template <typename T>
  struct BindingSupports {
    template <typename Binding>
    struct Predicate : std::integral_constant<bool, util::list::contains<typename Binding::value_types, T>> {};
  };

 public:
  explicit Converter(const Injector<Bindings...>* injector) : injector_{injector} {}

  template <typename T>
  requires(!std::is_same_v<std::decay_t<T>, Except>)
  operator T() {
    namespace list = util::list;

    using BindingsList = list::list<Bindings...>;
    using AcceptedBindings = list::filter<BindingsList, BindingSupports<T>::template Predicate>;

    static_assert(list::size<AcceptedBindings> != 0, "Binding not provided");
    static_assert(list::size<AcceptedBindings> == 1, "Multiple bindings provided");

    using BindingType = list::get<AcceptedBindings, 0>;
    static constexpr auto binding_index = list::indexOf<BindingsList, BindingType>;

    return std::get<binding_index>(injector_->bindings_).template get<T>(*injector_);
  }

 private:
  const Injector<Bindings...>* injector_;
};

template <typename... Bindings>
class Injector {
 public:
  template <typename T>
  T create() const {
    return create<T>([]<typename... Args>(Args&&... args) { return T{std::forward<Args>(args)...}; });
  }

  template <typename T>
  std::unique_ptr<T> createUnique() const {
    return create<T>([]<typename... Args>(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); });
  }

  template <typename T>
  std::shared_ptr<T> createShared() const {
    return create<T>([]<typename... Args>(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); });
  }

 private:
  explicit Injector(Bindings... bindings) : bindings_{std::move(bindings)...} {}

  template <typename T, typename Constructor>
  auto create(Constructor constructor) const {
    return [this, constructor = std::move(constructor)]<size_t... Indices>(std::index_sequence<Indices...>) mutable {
      return constructor(this->makeConverter<T, Indices>()...);
    }(std::make_index_sequence<detail::getNumConstructorArguments<T>()>());
  }

  template <typename Except, size_t Index>
  auto makeConverter() const {
    return Converter<Except, Index, Bindings...>(this);
  }

  std::tuple<Bindings...> bindings_;

  template <typename... Bs>
  friend auto makeInjector(Bs...);

  template <typename, size_t, typename...>
  friend class Converter;
};

template <typename... Bindings>
auto makeInjector(Bindings... bindings) {
  return detail::Injector(std::move(bindings)...);
}

}  // namespace detail

template <typename T>
static constexpr detail::BindLhs<T> bind;

template <typename... Bindings>
auto makeInjector(Bindings... bindings) {
  return detail::makeInjector(std::move(bindings)...);
}

}  // namespace di
