#include "algo/treap/treap.h"

#include <gtest/gtest.h>
#include <vector>

namespace test::treap::unit {

namespace treap = ::algo::treap;

struct traverse_print {
  template <typename Node>
  void operator()(const Node& node, int depth) const {
    while (depth--) {
      std::cout << "  ";
    }
    std::cout << "k = " << node.Key() << " v = " << node.Value() << " p = " << node.Priority() << std::endl;
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

TEST(TreapTest, Insert) {
  std::vector<treap::value_node<int>> nodes;
  nodes.reserve(5);
  treap::treap<treap::value_node<int>> t;

  for (int i = 0; i < 5; ++i) {
    nodes.emplace_back(i);
    t.insert(nodes.back());
    treap::impl::traverse(t.root(), traverse_le());
  }
}

TEST(TreapTest, Erase) {
  std::vector<treap::value_node<int>> nodes;
  nodes.reserve(5);
  treap::treap<treap::value_node<int>> t;
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
    if (i == 3) {
      continue;
    }
    ASSERT_NE(t.find(i), nullptr);
  }

  treap::impl::traverse(t.root(), traverse_le());
}

TEST(TreapTest, InsertImplicit) {
  std::vector<treap::implicit_key_node<int>> nodes;
  nodes.reserve(10);
  treap::implicit_key_treap<treap::implicit_key_node<int>> t;
  for (int i = 0; i < 10; ++i) {
    nodes.emplace_back(1 + i);
    t.push_back(nodes.back());
  }

  traverse_collect collector;
  treap::impl::traverse(t.root(), collector);
  ASSERT_EQ(collector.values, (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

  auto [u, v] = (std::move(t)).slice(3);
  auto [vu, vv] = (std::move(v)).slice(3);
  t = merge(std::move(vu), merge(std::move(u), std::move(vv)));
  collector.values.clear();
  treap::impl::traverse(t.root(), collector);
  ASSERT_EQ(collector.values, (std::vector<int>{5, 6, 7, 8, 1, 2, 3, 4, 9, 10}));
}

}  // namespace test::treap::unit
