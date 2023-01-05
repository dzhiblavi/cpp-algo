#ifndef ALGO_TREAP_H
#define ALGO_TREAP_H

#include "src/algo/treap/node.h"

#include <optional>

namespace algo::treap {

namespace impl {

template <typename Node>
Node* find(Node* root, const typename Node::KeyType& key) noexcept {
  if (root->Key() == key) {
    return root; 
  }
  if (root->Key() > key) {
    return root->left ? find(root->left, key) : nullptr;
  } else {
    return root->right ? find(root->right, key) : nullptr;
  }
}

template <typename Node>
std::pair<Node*, Node*> split(Node* node, const typename Node::KeyType& key) noexcept {
  if (node == nullptr) {
    return { nullptr, nullptr };
  }
  if (node->Key() == key) {
    auto* r = node->right;
    node->right = nullptr; 
    node->Update();
    return { node, r };
  } else if (node->Key() > key) {
    auto [l, r] = split(node->left, key);
    node->left = r;
    node->Update();
    return { l, node };
  } else {
    auto [l, r] = split(node->right, node->KeyLeftToRight(key));
    node->right = l;
    node->Update();
    return { node, r };
  }
}

template <typename Node>
Node* merge(Node* u, Node* v) noexcept {
  if (u == nullptr) {
    return v;
  }
  if (v == nullptr) {
    return u;
  }
  if (u->Priority() >= v->Priority()) {
    u->right = merge(u->right, v);
    u->Update();
    return u;
  } else {
    v->left = merge(u, v->left);
    v->Update();
    return v;
  }
}

template <typename Node>
Node* insert(Node* root, Node* node) noexcept {
  if (root == nullptr) {
    return node;
  }
  assert(find(root, node->Key()) == nullptr);
  auto [l, r] = split(root, node->Key());
  return merge(l, merge(node, r));
}

template <typename Node>
std::pair<Node*, Node*> erase(Node* root, const typename Node::KeyType& key) noexcept {
  if (root == nullptr || find(root, key) == nullptr) {
    return { root, nullptr };
  }
  auto [l, r] = split(root, key);
  auto [u, v] = split(l, key - 1);
  return { merge(u, r), v };
}

template <typename Node, typename F>
void traverse(Node* root, const F& func) {
  if (root == nullptr) return;
  traverse(root->left, func);
  func(*root);
  traverse(root->right, func);
}

template <typename Node, typename F>
void traverse_depth(Node* root, const F& func, int depth = 0) {
  if (root == nullptr) return;
  traverse_depth(root->left, func, depth + 1);
  func(*root, depth);
  traverse_depth(root->right, func, depth + 1);
}

}  // namespace impl

template <typename Node>
class treap {
 public:
  treap() noexcept = default;

  void insert(Node& node) noexcept { root_ = impl::insert(root_, &node); }

  std::pair<treap<Node>, treap<Node>> split(const typename Node::KeyType& key) && noexcept {
    auto [u, v] = impl::split(root_, key);
    return { treap<Node>(u), treap<Node>(v) };
  }

  Node* erase(const typename Node::KeyType& key) noexcept {
    auto [u, v] = impl::erase(root_, key);
    root_ = u;
    return v;
  }

  Node* find(const typename Node::KeyType& key) noexcept {
    return impl::find(root_, key);
  }

  Node* root() noexcept { return root_; }

 private:
  template <typename Treap>
  friend Treap merge(Treap&&, Treap&&) noexcept;

  explicit treap(Node* root) noexcept : root_(root) {}

  Node* root_ = nullptr;
};

template <typename Node>
class implicit_key_treap {
 public: 
  using SizeType = typename Node::SizeType;

  implicit_key_treap() noexcept = default;

  void push_back(Node& node) noexcept { root_ = impl::merge(root_, &node); }

  void pop_back() noexcept {
    auto size = root_->Size();   
    root_ = impl::split(root_, size - 1).first;
  }

  std::pair<implicit_key_treap<Node>, implicit_key_treap<Node>>
  slice(SizeType end_inclusive) && noexcept {
    auto [u, v] = impl::split(root_, end_inclusive);
    return { implicit_key_treap<Node>(u), implicit_key_treap<Node>(v) };
  }

  Node* root() noexcept { return root_; }

 private:
  template <typename Treap>
  friend Treap merge(Treap&&, Treap&&) noexcept;

  explicit implicit_key_treap(Node* root) noexcept : root_(root) {}

  Node* root_ = nullptr;
};

template <typename Treap>
Treap merge(Treap&& a, Treap&& b) noexcept { return Treap(impl::merge(a.root(), b.root())); }

}  // namespace algo::treap

#endif  // ALGO_TREAP_H

