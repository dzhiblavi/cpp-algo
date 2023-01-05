#ifndef ALGO_SEGMENT_TREE_H
#define ALGO_SEGMENT_TREE_H

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

template <typename T, typename Op, size_t NDims = 1>
class segment_tree {
 public:
  explicit segment_tree(size_t (&sizes)[NDims]) { init_dims(sizes); }

  explicit segment_tree(const multivector<T, NDims>& a) {
    fill_sizes<T, NDims, NDims - 1>(a, dims);
    init_dims();
    build<0>(a);
  }

  T get(size_t (&ql)[NDims], size_t (&qr)[NDims]) { return query<0>(ql, qr); }

  template <typename F>
  void transform(size_t (&cs)[NDims], const F& func) { update<0>(cs, func); }

  void set(size_t (&cs)[NDims], const T& value) { update<0>(cs, [&value] (T& x) { x = value; }); }

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
      t[t_idx] = op(t[t_idx_l], t[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i]) continue;
        size_t t_idx_l = t_idx + vs[i] * strides[i];
        size_t t_idx_r = t_idx_l + strides[i];
        t[t_idx] = op(t[t_idx_l], t[t_idx_r]);
        return;
      }
      t[t_idx] = multi_get<T, NDims>(a, ls);
    }
  }

  template <size_t I>
  T query(size_t (&qls)[NDims], size_t (&qrs)[NDims]) {
    if (qls[I] > qrs[I]) {
      return op.neutral();
    }

    if (ls[I] == qls[I] && rs[I] == qrs[I]) {
      if constexpr (I == NDims - 1) {
        return t[indices_to_linear_index(vs)];
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

    vs[I] = 2 * v;
    rs[I] = m;
    qrs[I] = std::min(qr, m);
    T l_query = query<I>(qls, qrs);

    vs[I] = 2 * v + 1;
    ls[I] = m + 1;
    rs[I] = r;
    qls[I] = std::max(ql, m + 1);
    qrs[I] = qr;
    T r_query = query<I>(qls, qrs);

    vs[I] = v;
    ls[I] = l;
    rs[I] = r; 
    qls[I] = ql;
    qrs[I] = qr;

    return op(l_query, r_query);
  }

  template <size_t I, typename F>
  void update(size_t (&cs)[NDims], const F& func) { 
    size_t l = ls[I];
    size_t r = rs[I];
    size_t c = cs[I];

    if (l != r) {
      size_t m = l + (r - l) / 2;
      size_t v = vs[I];

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

    size_t t_idx = indices_to_linear_index(vs);

    if (l != r) {
      size_t t_idx_l = t_idx + vs[I] * strides[I];
      size_t t_idx_r = t_idx_l + strides[I];
      t[t_idx] = op(t[t_idx_l], t[t_idx_r]);
    } else {
      for (size_t i = 0; i < NDims - 1; ++i) {
        if (ls[i] == rs[i]) continue;
        size_t t_idx_l = t_idx + vs[i] * strides[i];
        size_t t_idx_r = t_idx_l + strides[i];
        t[t_idx] = op(t[t_idx_l], t[t_idx_r]);
        return;
      }
      func(t[t_idx]);
    }
  }

  void init_dims() {
    size_t stride = 1;
    for (size_t dim = NDims; dim-- > 0;) {
      strides[dim] = stride;
      stride *= 4 * dims[dim];
    }
    t.resize(stride);
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
  std::vector<T> t;
};

}  // namespace algo::sgt


#endif  // ALGO_SEGMENT_TREE_H

