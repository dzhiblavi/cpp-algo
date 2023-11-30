#pragma once

#include <cstddef>
#include <limits>

namespace algo::aho {

template <typename SelfT, typename CharT>
struct node_base {
  using NodePtr = SelfT*;
  using CharType = CharT;

  static constexpr size_t NumChars = std::numeric_limits<CharT>::max() - std::numeric_limits<CharT>::min() + 1;

  NodePtr children[NumChars] = {nullptr};
  NodePtr jump[NumChars] = {nullptr};

  NodePtr suff_link = nullptr;
  NodePtr parent = nullptr;

  CharT parent_char{};
};

template <typename Node>
class AhoCorasick {
 public:
  using NodePtr = typename Node::NodePtr;
  using CharType = typename Node::CharType;

  AhoCorasick() noexcept = default;

  ~AhoCorasick() noexcept {
    destroy(&root_);
  }

  template <typename It>
  void add_string(It begin, It end) {
    NodePtr v = &root_;

    for (; begin != end; ++begin) {
      size_t index = *begin - std::numeric_limits<CharType>::min();

      if (v->children[index] == nullptr) {
        NodePtr u = new Node;
        u->parent = v;
        u->parent_char = *begin;
        v->children[index] = u;
      }

      v = v->children[index];
    }
  }

  NodePtr get_suff_link(NodePtr node) {
    if (node->suff_link == nullptr) {
      if (node == &root_ || node->parent == &root_) {
        node->suff_link = &root_;
      } else {
        node->suff_link = get_jump(get_suff_link(node->parent), node->parent_char);
      }
    }
    return node->suff_link;
  }

  NodePtr get_jump(NodePtr node, const CharType& c) {
    size_t index = c - std::numeric_limits<CharType>::min();
    if (node->jump[index] == nullptr) {
      if (node->children[index] != nullptr) {
        node->jump[index] = node->children[index];
      } else if (node == &root_) {
        node->jump[index] = &root_;
      } else {
        node->jump[index] = get_jump(get_suff_link(node), c);
      }
    }
    return node->jump[index];
  }

 private:
  static void destroy(NodePtr v) noexcept {
    for (NodePtr u : v->children) {
      if (u) {
        destroy(u);
        delete u;
      }
    }
  }

  Node root_;
};

}  // namespace algo::aho