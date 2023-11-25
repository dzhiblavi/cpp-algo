#pragma once

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#define mutable_method_impl(return_type, name, class_name, ...)                                                       \
  using name##_fn_t = return_type (*)(p_this, std::tuple<__VA_ARGS__>);                                               \
  struct name##Thunk {                                                                                                \
    template <typename Impl>                                                                                          \
    auto set() noexcept {                                                                                             \
      fun_ptr = [](p_this self, std::tuple<__VA_ARGS__> tuple) {                                                      \
        return std::apply(                                                                                            \
            [self](auto&&... args) { return static_cast<Impl*>(self)->name(std::forward<decltype(args)>(args)...); }, \
            std::move(tuple));                                                                                        \
      };                                                                                                              \
    }                                                                                                                 \
    decltype(auto) operator()(void* self, auto&&... args) {                                                           \
      return fun_ptr(self, std::forward_as_tuple(args...));                                                           \
    }                                                                                                                 \
    name##_fn_t fun_ptr;                                                                                              \
  }

#define vtable_mutable_method(return_type, name, ...) mutable_method_impl(return_type, name, name, ##__VA_ARGS__)
#define vtable_declare_thunks(...) std::tuple<__VA_ARGS__> thunks;

#define vtable_boilerplate                                                                     \
  using p_this = void*;                                                                        \
  using destroy_fn_t = void (*)(p_this) noexcept;                                              \
  template <typename FnT>                                                                      \
  auto& thunk() {                                                                              \
    return std::get<FnT>(thunks);                                                              \
  }                                                                                            \
  template <typename Impl>                                                                     \
  static auto* instance() {                                                                    \
    static VTable value = [] {                                                                 \
      VTable v{};                                                                              \
      std::apply([](auto&... thunks) { (thunks.template set<Impl>(), ...); }, v.thunks);       \
      return v;                                                                                \
    }();                                                                                       \
    return &value;                                                                             \
  }                                                                                            \
  struct destroyThunk {                                                                        \
    template <typename Impl>                                                                   \
    void set() noexcept {                                                                      \
      this->fun_ptr = +[](p_this self) noexcept { return static_cast<Impl*>(self)->~Impl(); }; \
    }                                                                                          \
    decltype(auto) operator()(void* self) {                                                    \
      return fun_ptr(self);                                                                    \
    }                                                                                          \
    destroy_fn_t fun_ptr;                                                                      \
  }

#define wptr_boilerplate(wptr_class, vtable_class)     \
  vtable_class* vtable;                                \
  void* self;                                          \
  ~wptr_class() noexcept {                             \
    if (!self) {                                       \
      return;                                          \
    }                                                  \
    vtable->thunk<vtable_class::destroyThunk>()(self); \
    ::operator delete(self);                           \
  }                                                    \
  template <typename Impl, typename... Args>           \
  static wptr_class make(Args&&... args) {             \
    auto x = wptr_class{                               \
        .vtable = vtable_class::instance<Impl>(),      \
        .self = ::operator new(sizeof(Impl)),          \
    };                                                 \
    new (x.self) Impl(std::forward<Args>(args)...);    \
    return x;                                          \
  }

#define wptr_mutable_method(return_type, name, ...)                                                 \
  return_type name(auto&&... args) {                                                                \
    using vtable_class = std::remove_reference_t<decltype(*vtable)>;                                \
    return vtable->thunk<vtable_class::name##Thunk>()(self, std::forward<decltype(args)>(args)...); \
  }
