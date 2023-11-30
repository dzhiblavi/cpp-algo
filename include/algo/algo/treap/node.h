#pragma once

#include "algo/treap/adapter.h"

#include <concepts>
#include <cstdint>
#include <functional>
#include <limits>
#include <random>
#include <tuple>
#include <type_traits>

namespace algo::treap {

template <typename T>
struct DistributionHolder;

template <std::integral T>
struct DistributionHolder<T> {
  using type = std::uniform_int_distribution<T>;
};

template <std::floating_point T>
struct DistributionHolder<T> {
  using type = std::uniform_real_distribution<T>;
};

inline std::mt19937& defaultReproducibleGenerator() {
  static std::mt19937 gen(0);
  return gen;
}

template <typename T>
auto& DefaultDistribution() {
  static auto distribution =
      typename DistributionHolder<T>::type(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
  static auto generator = std::bind(distribution, std::ref(defaultReproducibleGenerator()));
  return generator;
}

template <typename SelfT, std::integral KeyType, std::integral PriorityType = uint64_t>
class node_base {
 public:
  using NodePtr = SelfT*;

  node_base() noexcept(noexcept(node_base(DefaultDistribution<PriorityType>())))
      : node_base(DefaultDistribution<PriorityType>()) {}

  template <std::invocable<PriorityType()> Generator>
  explicit node_base(Generator& g) noexcept(noexcept(g())) : priority_(g()) {}

  void Update() noexcept(noexcept(Self().UpdateNode())) {
    Self().UpdateNode();
  }

  void UpdateNode() {}

  const KeyType& Key() const noexcept {
    return Self().Key();
  }

  const PriorityType& Priority() const noexcept {
    return priority_;
  }

  const KeyType& KeyLeftToRight(const KeyType& key) const noexcept {
    return key;
  }

  NodePtr left = nullptr;
  NodePtr right = nullptr;

 private:
  SelfT& Self() noexcept {
    return *static_cast<SelfT*>(this);
  }
  const SelfT& Self() const noexcept {
    return *static_cast<const SelfT*>(this);
  }

  PriorityType priority_;
};

template <typename T>
class value_node : public value_adapter<T>, public node_base<value_node<T>, T> {
 public:
  using KeyType = T;

  using value_adapter<T>::value_adapter;

  const KeyType& Key() const noexcept {
    return value_adapter<T>::Value();
  };
};

template <typename T, typename SizeT = size_t>
class implicit_key_node
    : public value_adapter<T>,
      public size_adapter<SizeT>,
      public node_base<implicit_key_node<T, SizeT>, SizeT> {
 public:
  using SizeType = SizeT;
  using KeyType = SizeType;

  using value_adapter<T>::value_adapter;

  void UpdateNode() {
    size_adapter<SizeType>::UpdateAdapter(this->left, this->right);
  }

  KeyType Key() const noexcept {
    return this->left ? this->left->Size() : 0;
  }

  KeyType KeyLeftToRight(const KeyType& key) {
    return key - Key() - 1;
  }
};

}  // namespace algo::treap
