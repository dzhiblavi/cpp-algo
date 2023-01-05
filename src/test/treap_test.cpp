#include <gtest/gtest.h>

#include <vector>

#include "algo/treap/treap.h"

namespace algo::treap {

struct traverse_print {
  template <typename Node>
  void operator()(const Node& node, int depth) const {
    while (depth--) std::cout << "  ";
    std::cout << "k = " << node.Key() 
              << " v = " << node.Value()
              << " p = " << node.Priority() << std::endl;
  }
};

struct traverse_collect {
  template <typename Node>
  void operator()(const Node& node) const {
    values.push_back(node.Value());
  }

  mutable std::vector<int> values;
};

struct traverse_le {
  template <typename Node>
  void operator()(const Node& node) const {
    ASSERT_LE(last_value, node.Value());
    last_value = node.Value();
  }
  mutable int last_value = 0;
};

TEST(key_node, insert) {
  std::vector<value_node<int>> nodes;
  nodes.reserve(5);
  treap<value_node<int>> t;

  for (int i = 0; i < 5; ++i) {
    nodes.emplace_back(i);
    t.insert(nodes.back());
    impl::traverse(t.root(), traverse_le());
  }
}

TEST(key_node, erase) {
  std::vector<value_node<int>> nodes;
  nodes.reserve(5);
  treap<value_node<int>> t;
  for (int i = 0; i < 5; ++i) {
    nodes.emplace_back(i);
    t.insert(nodes.back());
  }

  ASSERT_NE(t.find(3), nullptr);
  auto opt_erase = t.erase(3);
  ASSERT_NE(opt_erase, nullptr);
  ASSERT_EQ(opt_erase->Key(), 3);
  ASSERT_EQ(t.find(3), nullptr);

  for (int i = 0; i < 5; ++i) {
    if (i == 3) continue;
    ASSERT_NE(t.find(i), nullptr);
  }

  impl::traverse(t.root(), traverse_le());
}

TEST(implicit_key, insert) {
  std::vector<implicit_key_node<int>> nodes;
  nodes.reserve(10);
  implicit_key_treap<implicit_key_node<int>> t;
  for (int i = 0; i < 10; ++i) {
    nodes.emplace_back(1 + i);
    t.push_back(nodes.back());
  }

  traverse_collect collector;
  impl::traverse(t.root(), collector);
  ASSERT_EQ(collector.values, (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  auto [u, v] = (std::move(t)).slice(3);
  auto [vu, vv] = (std::move(v)).slice(3);
  t = merge(std::move(vu), merge(std::move(u), std::move(vv)));
  collector.values.clear();
  impl::traverse(t.root(), collector);
  ASSERT_EQ(collector.values, (std::vector<int>{5, 6, 7, 8, 1, 2, 3, 4, 9, 10}));
}

}  // namespace algo::treap

