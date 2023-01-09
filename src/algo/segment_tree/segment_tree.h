#ifndef ALGO_SEGMENT_TREE_H
#define ALGO_SEGMENT_TREE_H

#include <concepts>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <numeric>

#include <iostream>

namespace algo::sgt {

template <typename T, size_t NDims>
struct multivector_holder {
  using type = std::vector<typename multivector_holder<T, NDims - 1>::type>;
};

template <typename T>
struct multivector_holder<T, 1> { using type = std::vector<T>; };

template <typename T, size_t NDims>
using multivector = typename multivector_holder<T, NDims>::type;

template <typename T, size_t NDims, size_t Dim>
void fill_sizes(const multivector<T, Dim + 1>& a, size_t (&sizes)[NDims]) {
  sizes[NDims - Dim - 1] = a.size();
  if constexpr (Dim > 0) {
    fill_sizes<T, NDims, Dim - 1>(a.front(), sizes);
  }
}

template <typename T, size_t NDims, size_t Dim = 0>
const T& multi_get(const multivector<T, NDims - Dim>& a, size_t (&idxs)[NDims]) {
  if constexpr (Dim == NDims - 1) {
    return a[idxs[Dim]];
  } else {
    return multi_get<T, NDims, Dim + 1>(a[idxs[Dim]], idxs);
  }
}

template <typename Node, typename Op, size_t NDims = 1>
class segment_tree {
 public:
  using SelfType = segment_tree<Node, Op, NDims>;
  using T = typename Node::value_type;

  explicit segment_tree(size_t (&sizes)[NDims]) { init_dims(sizes); }

  explicit segment_tree(const multivector<T, NDims>& a) {
    fill_sizes<T, NDims, NDims - 1>(a, dims);
    init_dims();
    build<0>(a);
  }

  Node query(size_t (&ql)[NDims], size_t (&qr)[NDims]) { return query<0>(ql, qr); }

  template <typename F>
  void update(size_t (&cs)[NDims], const F& func) { update<0>(cs, func); }

  void update(size_t (&cs)[NDims], const T& value) {
    update<0>(cs, [&value] (auto& x) { x = value; });
  }

  void update_range(size_t (&ql)[NDims], size_t (&qr)[NDims], const T& value) {
    update_range<0>(ql, qr, value);
  }

 private:
  template <size_t I>
  void build(const multivector<T, NDims>& a) {
    size_t l = ls[I];
    size_t r = rs[I];

    if (l != r) {
      size_t m = l + (r - l) / 2;
      size_t v = vs[I];

      vs[I] = 2 * v;
      ls[I] = l;
      rs[I] = m;
      build<I>(a);

      vs[I] = 2 * v + 1;
      ls[I] = m + 1;
      rs[I] = r;
      build<I>(a);

      vs[I] = v;
      ls[I] = l;
      rs[I] = r;
    }

    if constexpr (I < NDims - 1) {
      build<I + 1>(a);
      return;
    }

    size_t t_idx = indices_to_linear_index(vs);

    if (l != r) {
      size_t t_idx_l = t_idx + vs[I] * strides[I];
      size_t t_idx_r = t_idx_l + strides[I];
      t[t_idx] = op.combine(t[t_idx_l], t[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i]) continue;
        size_t t_idx_l = t_idx + vs[i] * strides[i];
        size_t t_idx_r = t_idx_l + strides[i];
        t[t_idx] = op.combine(t[t_idx_l], t[t_idx_r]);
        return;
      }
      op.update_leaf(t[t_idx], multi_get<T, NDims>(a, ls));
    }
  }

  template <size_t I>
  Node query(size_t (&qls)[NDims], size_t (&qrs)[NDims]) {
    if (qls[I] > qrs[I]) {
      return op.neutral();
    }

    size_t t_idx = indices_to_linear_index(vs);

    if (ls[I] == qls[I] && rs[I] == qrs[I]) {
      if constexpr (I == NDims - 1) {
        return t[t_idx];
      } else {
        return query<I + 1>(qls, qrs);
      }
    }

    size_t v = vs[I];
    size_t l = ls[I];
    size_t r = rs[I];
    size_t ql = qls[I];
    size_t qr = qrs[I];
    size_t m = l + (r - l) / 2;

    if constexpr (I == NDims - 1) {
      op.push(t[t_idx], t[t_idx + v], t[t_idx + v + 1]);
    }

    vs[I] = 2 * v;
    rs[I] = m;
    qrs[I] = std::min(qr, m);
    Node l_query = query<I>(qls, qrs);

    vs[I] = 2 * v + 1;
    ls[I] = m + 1;
    rs[I] = r;
    qls[I] = std::max(ql, m + 1);
    qrs[I] = qr;
    Node r_query = query<I>(qls, qrs);

    vs[I] = v;
    ls[I] = l;
    rs[I] = r; 
    qls[I] = ql;
    qrs[I] = qr;

    return op.combine(l_query, r_query);
  }

  template <size_t I, typename F>
  void update(size_t (&cs)[NDims], const F& func) { 
    size_t l = ls[I];
    size_t r = rs[I];
    size_t c = cs[I];
    size_t v = vs[I];
    size_t t_idx = indices_to_linear_index(vs);

    if constexpr (I == NDims - 1) {
      op.push(t[t_idx], t[t_idx + v], t[t_idx + v + 1]);
    }

    if (l != r) {
      size_t m = l + (r - l) / 2;

      if (c <= m) {
        vs[I] = 2 * v;
        ls[I] = l;
        rs[I] = m;
      } else {
        vs[I] = 2 * v + 1;
        ls[I] = m + 1;
        rs[I] = r;
      }

      update<I>(cs, func);
      vs[I] = v;
      ls[I] = l;
      rs[I] = r;
    }

    if constexpr (I < NDims - 1) {
      update<I + 1>(cs, func);
      return;
    }

    if (l != r) {
      size_t t_idx_l = t_idx + vs[I] * strides[I];
      size_t t_idx_r = t_idx_l + strides[I];
      t[t_idx] = op.combine(t[t_idx_l], t[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i]) continue;
        size_t t_idx_l = t_idx + vs[i] * strides[i];;
        size_t t_idx_r = t_idx_l + strides[i];
        t[t_idx] = op.combine(t[t_idx_l], t[t_idx_r]);
        return;
      }
      func(t[t_idx]);
    }
  }

  template <size_t I>
  void update_range(size_t (&qls)[NDims], size_t (&qrs)[NDims], const T& value) {
    if (qls[I] > qrs[I]) {
      return;
    }

    size_t t_idx = indices_to_linear_index(vs);

    if (ls[I] == qls[I] && rs[I] == qrs[I]) {
      if constexpr (I == NDims - 1) {
        op.update_range(t[t_idx], value);
      } else {
        update_range<I + 1>(qls, qrs, value);
      }
      return;
    }

    size_t v = vs[I];
    size_t l = ls[I];
    size_t r = rs[I];
    size_t ql = qls[I];
    size_t qr = qrs[I];
    size_t m = l + (r - l) / 2;

    if constexpr (I == NDims - 1) {
      op.push(t[t_idx], t[t_idx + v], t[t_idx + v + 1]);
    }

    vs[I] = 2 * v;
    rs[I] = m;
    qrs[I] = std::min(qr, m);
    update_range<I>(qls, qrs, value);

    vs[I] = 2 * v + 1;
    ls[I] = m + 1;
    rs[I] = r;
    qls[I] = std::max(ql, m + 1);
    qrs[I] = qr;
    update_range<I>(qls, qrs, value);

    vs[I] = v;
    ls[I] = l;
    rs[I] = r; 
    qls[I] = ql;
    qrs[I] = qr;

    if constexpr (I == NDims - 1) {
      t[t_idx] = op.combine(t[t_idx + v], t[t_idx + v + 1]);
    }
  }

  void init_dims() {
    size_t stride = 1;
    for (size_t dim = NDims; dim-- > 0;) {
      strides[dim] = stride;
      stride *= 4 * dims[dim];
    }
    t.assign(stride, op.init());
    std::fill(vs, vs + NDims, 1);
    std::fill(ls, ls + NDims, 0);
    for (int i = 0; i < NDims; ++i) rs[i] = dims[i] - 1;
  }

  size_t indices_to_linear_index(size_t (&idxs)[NDims]) {
    return std::inner_product(strides, strides + NDims, idxs, 0);
  }

  Op op;
  size_t dims[NDims];
  size_t strides[NDims];
  size_t vs[NDims];
  size_t ls[NDims];
  size_t rs[NDims];
  std::vector<Node> t;
};

/**
 * @brief Segment tree's basic node class.
 *
 * Requirements for nodes (UserNode):
 *  - typename UserNode::value_type is valid and represents values' type.
 *
 * This particular implementation simply stores the value in the node.
 */
template <typename T>
struct simple_node {
  using value_type = T;

  template <typename... Args> requires std::constructible_from<T, Args...>
  simple_node(Args&&... args) : value_(std::forward<Args>(args)...) {}

  value_type& value() noexcept { return value_; }
  const value_type& value() const noexcept { return value_; }
  operator const value_type&() const noexcept { return value(); }

  void set_value(const value_type& value) { value_ = value; }

 private:
  value_type value_;
};

/**
 * @brief Segment tree's basic operation class.
 *
 * Requirements for segment tree's operation (UserOp):
 *  - UserOp::init returns initial node's value,
 *  - UserOp::neutral returns such node N so that for any node M combine(N, M) = combine(M, N) = M,
 *  - UserOp::combine merges two nodes into a single one,
 *  - UserOp::update_leaf updates the node using the new value (called for leaves only),
 *
 * Optional operations (required for range updates):
 *  - UserOp::push propagates segment-wise changes to its children,
 *  - UserOp::update_range updates the node's accumulators for range updates.
 *
 * This particular implementation is meant for Node = simple_node<typename Node::value_type>
 * and simply applies Operation::operator() whenever needed.
 *
 * This op does not support range udpates since simple_node does not store accumulators.
 */
template <typename Node, typename Op>
struct base_op {
  // At least one of these methods must be defined
  Node init() { return self().neutral(); }
  Node neutral() { return self().init(); }

  // By default simply do nothing
  void push(Node&, Node&, Node&) {}
  void update_range(Node&, const typename Node::value_type&) {}

 private:
  Op& self() noexcept { return *static_cast<Op*>(this); }
  const Op& self() const noexcept { return *static_cast<const Op*>(this); }
};

/**
 * This op does not support range udpates since simple_node does not store accumulators.
 */
template <typename Node, typename Operation>
struct simple_op : public base_op<Node, simple_op<Node, Operation>> {
  Node neutral() const noexcept { return op.neutral(); }
  Node combine(const Node& left, const Node& right) { return op(left.value(), right.value()); }
  void update_leaf(Node& node, const typename Node::value_type& value) { node.set_value(value); }

 private:
  Operation op;
};

template <typename T, typename Op, size_t NDims = 1>
using simple_segment_tree = segment_tree<simple_node<T>, simple_op<simple_node<T>, Op>, NDims>;

}  // namespace algo::sgt


#endif  // ALGO_SEGMENT_TREE_H

