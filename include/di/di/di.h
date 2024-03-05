#pragma once

#include "constructor_nargs.h"
#include "utility/templates.h"
#include "utility/type_list.h"

#include <cstddef>
#include <memory>
#include <utility>

namespace di {

namespace detail {

template <typename... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <typename T>
auto valueConstructor() {
  return overloaded{
      []<typename... Args>(Args&&... args) { return T(std::forward<Args>(args)...); },
      [](T&& value) { return std::move(value); },
  };
}

template <typename T>
auto rawConstructor() {
  return []<typename... Args>(Args&&... args) { return new T(std::forward<Args>(args)...); };
}

template <typename T>
auto uniqueConstructor() {
  return []<typename... Args>(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); };
}

template <typename T>
auto sharedConstructor() {
  return []<typename... Args>(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); };
}

template <typename From, typename To>
struct Binding {
  using bound_type = From;
  using actual_type = To;
};

template <typename T>
struct ValueBinding : Binding<T, T> {
  explicit ValueBinding(T value) : value_{std::move(value)} {}

  const T& get(const auto&) const {
    return value_;
  }

 private:
  T value_;
};

template <typename From, typename To>
struct TypeBinding : Binding<From, To> {
  auto get(const auto& injector) const {
    return injector.template create<To>();
  }
};

template <typename From>
struct Bind {
  ValueBinding<From> to(From value) const {
    return ValueBinding<From>(std::move(value));
  }

  template <std::derived_from<From> Implementation>
  TypeBinding<From, Implementation> to() const {
    return {};
  }
};

template <typename... Bindings>
class Injector;

template <typename Except, size_t Tag, typename... Bindings>
class Converter {
 public:
  explicit Converter(const Injector<Bindings...>* injector) : injector_{injector} {}

  template <typename T>
  requires(!std::is_same_v<std::decay_t<T>, Except>)
  operator T() const {
    return injector_->template create<T>();
  }

 private:
  const Injector<Bindings...>* injector_;
};

template <typename... Bindings>
class Injector {
  using BindingsList = util::list::list<Bindings...>;

 public:
  template <typename T>
  T create() const {
    return create<T>(valueConstructor<T>());
  }

 private:
  explicit Injector(Bindings... bindings) : bindings_{std::move(bindings)...} {}

  template <typename T>
  auto create(auto constructor) const {
    if constexpr (isBound<T>()) {
      return constructor(getBindingFor<T>().get(*this));
    } else {
      return createUnbound<T>(std::move(constructor));
    }
  }

  template <typename T, typename Constructor>
  auto createUnbound(Constructor constructor) const {
    if constexpr (util::tpl::isInstanceOfTemplate<T, std::shared_ptr>) {
      using type = actual_type_t<typename T::element_type>;
      return constructor(create<type>(sharedConstructor<type>()));
    } else if constexpr (util::tpl::isInstanceOfTemplate<T, std::unique_ptr>) {
      using type = actual_type_t<typename T::element_type>;
      return constructor(create<type>(uniqueConstructor<type>()));
    } else if constexpr (std::is_pointer_v<T>) {
      using type = actual_type_t<std::remove_pointer_t<T>>;
      return constructor(create<type>(rawConstructor<type>()));
    } else {
      return callConstructor<T>(std::move(constructor));
    }
  }

  template <typename T, typename Constructor>
  auto callConstructor(Constructor constructor) const {
    return [this, constructor = std::move(constructor)]<size_t... Indices>(std::index_sequence<Indices...>) mutable {
      return constructor(Converter<T, Indices, Bindings...>(this)...);
    }(std::make_index_sequence<detail::getNumConstructorArguments<T>()>());
  }

  template <typename T>
  decltype(auto) getBindingFor() const {
    static_assert(isBound<T>(), "No binding provided for the type");
    return std::get<getBindingIndex<T>()>(bindings_);
  }

  template <typename T>
  static constexpr size_t getBindingIndex() {
    using BindingTypesList = util::list::list<typename Bindings::bound_type...>;
    return util::list::indexOf<BindingTypesList, T>;
  }

  template <typename T>
  static constexpr bool isBound() {
    return getBindingIndex<T>() != util::list::npos;
  }

  template <typename T, bool isBound = isBound<T>()>
  struct ActualType {
    using type = T;
  };

  template <typename T>
  struct ActualType<T, true> {
    using type = typename util::list::get<BindingsList, getBindingIndex<T>()>::actual_type;
  };

  template <typename T>
  using actual_type_t = typename ActualType<T>::type;

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
static constexpr detail::Bind<T> bind;

template <typename... Bindings>
auto makeInjector(Bindings... bindings) {
  return detail::makeInjector(std::move(bindings)...);
}

}  // namespace di
