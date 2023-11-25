#pragma once

#include "algo/common/types.h"
#include "algo/utility/nd_container.h"

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <vector>

namespace algo::sgt {

template <typename Node, typename Op, size_t NDims>
class SegmentTree;

namespace detail {

template <size_t NDims>
struct Position {
  explicit Position(const types::Index<NDims>& dims) noexcept {
    std::fill(vs.begin(), vs.end(), 1);
    std::fill(ls.begin(), ls.end(), 0);
    for (size_t i = 0; i < NDims; ++i) {
      rs[i] = dims[i] - 1;
    }
  }

  types::Index<NDims> vs;
  types::Index<NDims> ls;
  types::Index<NDims> rs;
};

template <size_t NDims>
struct QueryHandle {
  types::Index<NDims>& getIndex() noexcept { return index_; }

 private:
  explicit QueryHandle(const types::Index<NDims>& dims) noexcept : position_{dims} {}

  types::Index<NDims> index_;
  Position<NDims> position_;

  template <typename T, typename Op, size_t Dims>
  friend class sgt::SegmentTree;
};

template <size_t NDims>
struct RangeQueryHandle {
  types::Range<NDims>& getRange() noexcept { return range_; }

 private:
  explicit RangeQueryHandle(const types::Index<NDims>& dims) noexcept : position_{dims} {}

  types::Range<NDims> range_;
  Position<NDims> position_;

  template <typename T, typename Op, size_t Dims>
  friend class sgt::SegmentTree;
};

}  // namespace detail

template <typename Node, typename Op, size_t NDims = 1>
class SegmentTree {
  using T = typename Node::value_type;

 public:
  using QueryHandle = detail::QueryHandle<NDims>;
  using RangeQueryHandle = detail::RangeQueryHandle<NDims>;

  explicit SegmentTree(types::Index<NDims>& sizes) { initDims(sizes); }

  explicit SegmentTree(utility::NDView<T, NDims> auto view) {
    view.getDimensions(dims_);
    initDims();

    detail::Position<NDims> position{dims_};
    build<0>(view, position);
  }

  [[nodiscard]] QueryHandle getQueryHandle() const noexcept { return QueryHandle(dims_); }

  [[nodiscard]] RangeQueryHandle getRangeQueryHandle() const noexcept { return RangeQueryHandle(dims_); }

  Node query(RangeQueryHandle& handle) { return query<0>(handle); }

  template <typename F>
  void update(QueryHandle& handle, const F& func) {
    update<0>(handle, func);
  }

  void update(QueryHandle& handle, const T& value) {
    update<0>(handle, [&value](auto& x) { x = value; });
  }

  void updateRange(RangeQueryHandle& handle, const T& value) { updateRange<0>(handle, value); }

 private:
  template <size_t I>
  void build(utility::NDView<T, NDims> auto view, detail::Position<NDims>& position) {
    auto& [vs, ls, rs] = position;

    size_t l = ls[I];
    size_t r = rs[I];

    if (l != r) {
      size_t m = l + (r - l) / 2;
      size_t v = vs[I];

      vs[I] = 2 * v;
      ls[I] = l;
      rs[I] = m;
      build<I>(view, position);

      vs[I] = 2 * v + 1;
      ls[I] = m + 1;
      rs[I] = r;
      build<I>(view, position);

      vs[I] = v;
      ls[I] = l;
      rs[I] = r;
    }
    if constexpr (I < NDims - 1) {
      build<I + 1>(view, position);
      return;
    }

    size_t t_idx = indicesToLinearIndex(vs);

    if (l != r) {
      size_t t_idx_l = t_idx + vs[I] * strides_[I];
      size_t t_idx_r = t_idx_l + strides_[I];
      storage_[t_idx] = op_.combine(storage_[t_idx_l], storage_[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i]) {
          continue;
        }

        size_t t_idx_l = t_idx + vs[i] * strides_[i];
        size_t t_idx_r = t_idx_l + strides_[i];
        storage_[t_idx] = op_.combine(storage_[t_idx_l], storage_[t_idx_r]);
        return;
      }
      op_.updateLeaf(storage_[t_idx], view.at(ls));
    }
  }

  template <size_t I>
  Node query(RangeQueryHandle& handle) {
    auto& [range, position] = handle;
    auto& [qls, qrs] = range;
    auto& [vs, ls, rs] = position;

    if (qls[I] > qrs[I]) {
      return op_.neutral();
    }

    size_t t_idx = indicesToLinearIndex(vs);

    if (ls[I] == qls[I] && rs[I] == qrs[I]) {
      if constexpr (I == NDims - 1) {
        return storage_[t_idx];
      } else {
        return query<I + 1>(handle);
      }
    }

    size_t v = vs[I];
    size_t l = ls[I];
    size_t r = rs[I];
    size_t ql = qls[I];
    size_t qr = qrs[I];
    size_t m = l + (r - l) / 2;

    if constexpr (I == NDims - 1) {
      op_.push(storage_[t_idx], storage_[t_idx + v], storage_[t_idx + v + 1]);
    }

    vs[I] = 2 * v;
    rs[I] = m;
    qrs[I] = std::min(qr, m);
    Node l_query = query<I>(handle);

    vs[I] = 2 * v + 1;
    ls[I] = m + 1;
    rs[I] = r;
    qls[I] = std::max(ql, m + 1);
    qrs[I] = qr;
    Node r_query = query<I>(handle);

    vs[I] = v;
    ls[I] = l;
    rs[I] = r;
    qls[I] = ql;
    qrs[I] = qr;

    return op_.combine(l_query, r_query);
  }

  template <size_t I, typename F>
  void update(QueryHandle& handle, const F& func) {
    auto& [cs, position] = handle;
    auto& [vs, ls, rs] = position;

    size_t l = ls[I];
    size_t r = rs[I];
    size_t c = cs[I];
    size_t v = vs[I];
    size_t t_idx = indicesToLinearIndex(vs);

    if constexpr (I == NDims - 1) {
      op_.push(storage_[t_idx], storage_[t_idx + v], storage_[t_idx + v + 1]);
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

      update<I>(handle, func);
      vs[I] = v;
      ls[I] = l;
      rs[I] = r;
    }

    if constexpr (I < NDims - 1) {
      update<I + 1>(handle, func);
      return;
    }

    if (l != r) {
      size_t t_idx_l = t_idx + vs[I] * strides_[I];
      size_t t_idx_r = t_idx_l + strides_[I];
      storage_[t_idx] = op_.combine(storage_[t_idx_l], storage_[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i])
          continue;
        size_t t_idx_l = t_idx + vs[i] * strides_[i];
        size_t t_idx_r = t_idx_l + strides_[i];
        storage_[t_idx] = op_.combine(storage_[t_idx_l], storage_[t_idx_r]);
        return;
      }
      func(storage_[t_idx]);
    }
  }

  template <size_t I>
  void updateRange(RangeQueryHandle& handle, const T& value) {
    auto& [range, position] = handle;
    auto& [qls, qrs] = range;
    auto& [vs, ls, rs] = position;

    if (qls[I] > qrs[I]) {
      return;
    }

    size_t t_idx = indicesToLinearIndex(vs);

    if (ls[I] == qls[I] && rs[I] == qrs[I]) {
      if constexpr (I == NDims - 1) {
        op_.updateRange(storage_[t_idx], value);
      } else {
        updateRange<I + 1>(handle, value);
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
      op_.push(storage_[t_idx], storage_[t_idx + v], storage_[t_idx + v + 1]);
    }

    vs[I] = 2 * v;
    rs[I] = m;
    qrs[I] = std::min(qr, m);
    updateRange<I>(handle, value);

    vs[I] = 2 * v + 1;
    ls[I] = m + 1;
    rs[I] = r;
    qls[I] = std::max(ql, m + 1);
    qrs[I] = qr;
    updateRange<I>(handle, value);

    vs[I] = v;
    ls[I] = l;
    rs[I] = r;
    qls[I] = ql;
    qrs[I] = qr;

    if constexpr (I == NDims - 1) {
      storage_[t_idx] = op_.combine(storage_[t_idx + v], storage_[t_idx + v + 1]);
    }
  }

  void initDims() {
    size_t stride = 1;
    for (size_t dim = NDims; dim-- > 0;) {
      strides_[dim] = stride;
      stride *= 4 * dims_[dim];
    }
    storage_.assign(stride, op_.init());
  }

  size_t indicesToLinearIndex(types::Index<NDims>& idxs) {
    return std::inner_product(strides_.begin(), strides_.end(), idxs.begin(), 0);
  }

  [[no_unique_address]] Op op_;
  types::Index<NDims> dims_;
  types::Index<NDims> strides_;
  std::vector<Node> storage_;
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

  template <typename... Args>
  requires std::constructible_from<T, Args...>
  simple_node(Args&&... args) : value_(std::forward<Args>(args)...) {}

  value_type& value() noexcept { return value_; }
  const value_type& value() const noexcept { return value_; }
  operator const value_type&() const noexcept { return value(); }

  void setValue(const value_type& value) { value_ = value; }

 private:
  [[no_unique_address]] value_type value_;
};

/**
 * @brief Segment tree's basic operation class.
 *
 * Requirements for segment tree's operation (UserOp):
 *  - UserOp::init returns initial node's value,
 *  - UserOp::neutral returns such node N so that for any node M combine(N, M) =
 * combine(M, N) = M,
 *  - UserOp::combine merges two nodes into a single one,
 *  - UserOp::updateLeaf updates the node using the new value (called for
 * leaves only),
 *
 * Optional operations (required for range updates):
 *  - UserOp::push propagates segment-wise changes to its children,
 *  - UserOp::updateRange updates the node's accumulators for range updates.
 *
 * This particular implementation is meant for Node = simple_node<typename
 * Node::value_type> and simply applies Operation::operator() whenever needed.
 *
 * This op does not support range updates since simple_node does not store
 * accumulators.
 */
template <typename Node, typename Op>
struct base_op {
  // At least one of these methods must be defined
  Node init() { return self().neutral(); }
  Node neutral() { return self().init(); }

  // By default simply do nothing
  void push(Node&, Node&, Node&) {}
  void updateRange(Node&, const typename Node::value_type&) {}

 private:
  Op& self() noexcept { return *static_cast<Op*>(this); }
  const Op& self() const noexcept { return *static_cast<const Op*>(this); }
};

/**
 * This op does not support range updates since simple_node does not store
 * accumulators.
 */
template <typename Node, typename Operation>
struct simple_op : public base_op<Node, simple_op<Node, Operation>> {
  Node neutral() const noexcept { return op.neutral(); }
  Node combine(const Node& left, const Node& right) { return op(left.value(), right.value()); }
  void updateLeaf(Node& node, const typename Node::value_type& value) { node.setValue(value); }

 private:
  [[no_unique_address]] Operation op;
};

template <typename T, typename Op, size_t NDims = 1>
using SimpleSegmentTree = SegmentTree<simple_node<T>, simple_op<simple_node<T>, Op>, NDims>;

}  // namespace algo::sgt
