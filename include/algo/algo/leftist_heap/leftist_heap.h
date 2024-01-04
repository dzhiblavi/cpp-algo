#pragma once

#include <concepts>
#include <memory>

namespace algo::leftist_heap {

/**
 * @brief Leftist heap is a binary tree with heap properties
 * @note that it is not necessarily balanced
 */
template <typename T, std::integral S = size_t, typename Comparator = std::less<T>>
class IntrusiveLeftistHeap;

template <typename T, std::integral S = size_t>
struct LeftistHeapHook {
  using SizeType = S;

  S distance{1};
  T* left{nullptr};
  T* right{nullptr};
  T* parent{nullptr};
};

namespace impl {

template <typename T, std::integral S, typename Comparator>
T* merge(T* x, T* y, Comparator& less_comparator) noexcept;

template <typename T, std::integral SizeType>
std::pair<T*, T*> extract(T* heap) noexcept;

namespace debug {

template <typename T, std::integral S, std::invocable<LeftistHeapHook<S>*> UnaryFunction>
void traverse(LeftistHeapHook<T, S>* heap, UnaryFunction function);

template <typename T, std::integral S, typename Comparator>
bool checkInvariants(const T* heap, Comparator& comparator) noexcept;

}  // namespace debug

}  // namespace impl

template <typename T, std::integral S, typename Comparator>
class IntrusiveLeftistHeap {
  static_assert(std::derived_from<T, LeftistHeapHook<T, S>>, "T must be publicly derived from heap hook");

 public:
  struct debug {
    /**
     * @brief Traverses all elements of a given heap. Complexity O(N * T(function)).
     */
    template <std::invocable<const T&> UnaryFunction>
    static void traverse(const IntrusiveLeftistHeap& heap, UnaryFunction&& function) {
      impl::debug::traverse<T, S, UnaryFunction>(heap.root_, std::forward<UnaryFunction>(function));
    }

    /**
     * @brief Completely verifies all leftist heap's invariants.
     */
    static bool checkInvariants(const IntrusiveLeftistHeap& heap) noexcept {
      return impl::debug::checkInvariants<T, S>(heap.root_, heap.comparator_);
    }
  };

  /**
   * @brief Constructs an empty heap.
   */
  IntrusiveLeftistHeap() noexcept = default;

  IntrusiveLeftistHeap(const IntrusiveLeftistHeap&) = default;
  IntrusiveLeftistHeap& operator=(const IntrusiveLeftistHeap&) = default;

  /**
   * @brief Removes the minimal element from the heap. Complexity O(log(N)).
   */
  void extractTop() noexcept {
    impl::extract<T, S>(root_, comparator_);
  }

  /**
   * @brief Inserts a new element into the heap. Complexity O(log(N)).
   */
  void insert(T& value) noexcept {
    *this = IntrusiveLeftistHeap<T, S>::merge(*this, singleton(value), comparator_);
  }

  /**
   * @brief Check whether the heap is empty.
   */
  [[nodiscard]] bool isEmpty() const noexcept {
    return root_ == nullptr;
  }

  /**
   * @brief Get the minimal element of this heap.
   */
  [[nodiscard]] T& top() noexcept {
    return *root_;
  }
  [[nodiscard]] const T& top() const noexcept {
    return *root_;
  }

  /**
   * @brief Merges two leftist heaps. Complexity O(log(N)).
   * @note No allocations are made, result is always either of two parameters.
   */
  static IntrusiveLeftistHeap merge(
      IntrusiveLeftistHeap x, IntrusiveLeftistHeap y, Comparator comparator = {}) noexcept {
    return IntrusiveLeftistHeap<T, S>(impl::merge<T, S, Comparator>(x.root_, y.root_, comparator));
  }

  /**
   * @brief Empty heap "singleton".
   */
  static const IntrusiveLeftistHeap& empty() noexcept {
    static const IntrusiveLeftistHeap instance{};
    return instance;
  }

  /**
   * @brief Constructs a heap of one element.
   */
  static IntrusiveLeftistHeap singleton(T& value, Comparator comparator = {}) noexcept {
    return IntrusiveLeftistHeap<T, S, Comparator>(&value, std::move(comparator));
  }

 private:
  explicit IntrusiveLeftistHeap(T* root, Comparator comparator = {}) noexcept
      : comparator_{std::move(comparator)}, root_{root} {}

  [[no_unique_address]] Comparator comparator_{};
  T* root_{nullptr};
};

namespace impl {

template <typename T, std::integral S>
S getDistance(LeftistHeapHook<T, S>* node) noexcept {
  return node == nullptr ? S{0} : node->distance;
}

template <typename T, std::integral S, typename Comparator>
T* merge(T* x, T* y, Comparator& less_comparator) noexcept {
  if (x == nullptr) [[unlikely]] {
    return y;
  }
  if (y == nullptr) [[unlikely]] {
    return x;
  }
  if (!less_comparator(*x, *y)) {
    std::swap(x, y);
  }
  x->right = merge<T, S, Comparator>(x->right, y, less_comparator);
  x->right->parent = x;

  if (getDistance(x->right) > getDistance(x->left)) {
    std::swap(x->left, x->right);
  }

  x->distance = getDistance(x->right) + 1;
  return x;
}

template <typename T, std::integral SizeType>
std::pair<T*, T*> extract(T*) noexcept {}

template <typename T, std::integral S, std::invocable<LeftistHeapHook<S>*> UnaryFunction>
requires std::derived_from<T, LeftistHeapHook<T, S>>
void traverse(LeftistHeapHook<T, S>* heap, UnaryFunction function) {
  if (heap == nullptr) {
    return;
  }

  traverse(heap->left, function);
  function(heap);
  traverse(heap->right, function);
}

namespace debug {

template <typename T, std::integral S, typename Comparator>
bool checkInvariants(const T* heap, Comparator& less_comparator) noexcept {
  if (heap == nullptr) {
    return true;
  }
  if (heap->left != nullptr && less_comparator(*heap->left, *heap)) {
    return false;
  }
  if (heap->right != nullptr && less_comparator(*heap->right, *heap)) {
    return false;
  }
  if (heap->left != nullptr && heap->left->parent != heap) {
    return false;
  }
  if (heap->right != nullptr && heap->right->parent != heap) {
    return false;
  }
  if (getDistance(heap->left) < getDistance(heap->right)) {
    return false;
  }
  if (heap->distance != getDistance(heap->right) + 1) {
    return false;
  }

  return checkInvariants<T, S>(heap->left, less_comparator) && checkInvariants<T, S>(heap->right, less_comparator);
}

}  // namespace debug

// template <typename T, std::integral SizeType,
// std::invocable<const T&> UnaryFunction> LeftistHeap<T, SizeType> modifyKey(
//     LeftistHeap<T, SizeType> heap, UnaryFunction&& modifier) noexcept {
//   auto* root = heap.root_;
//   auto* parent = root->parent;

//   // perform modification
//   std::forward<UnaryFunction>(modifier)(root->value);

//   if (parent == nullptr) {
//     return heap;
//   }

//   // disconnect heap from the rest
//   if (parent->left == root) {
//     parent->left = nullptr;
//   } else if (parent->right == root) {
//     parent->right = nullptr;
//   } else [[unlikely]] {
//     assert(false);
//   }

//   auto fix_distances = [](auto* node) {
//     if (node->right->distance > node->left->distance) [[unlikely]] {
//       std::swap(node->right, node->left);
//     }
//     node->distance = node->right->distance + 1;
//   };

//   while (parent->parent != nullptr) {
//     fix_distances(parent);
//     parent = parent->parent;
//   }

//   fix_distances(parent);
//   return merge(LeftistHeap{.root = parent}, heap);
// }

}  // namespace impl

}  // namespace algo::leftist_heap
