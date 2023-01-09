#ifndef ALGO_FENWICK_TREE_H
#define ALGO_FENWICK_TREE_H

#include <cstddef>
#include <limits>
#include <vector>
#include <functional>
#include <iostream>

namespace algo::fwt {

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

template <typename T>
struct fenwick_min_op {
  T neutral() const noexcept { return std::numeric_limits<T>::max(); }
  T operator()(const T& a, const T& b) const noexcept { return std::min(a, b); }

  /* Set value to x */
  void update(T& t, const T& x) const noexcept { t = std::min(t, x); }
};

template <typename T>
struct fenwick_max_op {
  T neutral() const noexcept { return std::numeric_limits<T>::min(); }
  T operator()(const T& a, const T& b) const noexcept { return std::max(a, b); }

  /* Set value to x */
  void update(T& t, const T& x) const noexcept { t = std::max(t, x); }
};

template <typename T>
struct fenwick_sum_op : public std::plus<T> {
  T neutral() const noexcept { return T(0); }
  T inv(const T& x) { return -x; }

  /* Increase value by x */
  void update(T& t, const T& x) const noexcept { t += x; }
};

template <typename T, typename Op, size_t NDims = 1>
class fenwick_tree {
 public:
  explicit fenwick_tree(const size_t (&dims)[NDims]) { init_dims(dims); }

  explicit fenwick_tree(const multivector<T, NDims>& a) {
    size_t d[NDims];
    fill_sizes<T, NDims, NDims - 1>(a, d);
    init_dims(d);
    init<0>(a, d);
  }

  void update(const size_t (&cs)[NDims], const T& value) { update<0>(cs, 0, value); }

  T query(const size_t (&qr)[NDims]) {
    T result = op.neutral();
    query<0>(result, qr, 0);
    return result;
  }

  T query(const size_t (&ql)[NDims], const size_t (&qr)[NDims]) {
    size_t q[NDims];
    T result = op.neutral();
    query<0, 1>(result, q, ql, qr);
    return result;
  }

 private:
  template <size_t I>
  void query(T& result, const size_t (&qr)[NDims], size_t index) {
    for (int64_t i = qr[I]; i >= 0; i = (i & (i + 1)) - 1) {
      if constexpr (I == NDims - 1) {
        result = op(result, t[index + i]);
      } else {
        query<I + 1>(result, qr, index + i * strides[I]);
      }
    }
  }

  template <size_t I, int Sign>
  void query(T& result, size_t (&q)[NDims], const size_t (&ql)[NDims], const size_t (&qr)[NDims]) {
    q[I] = ql[I] - 1;
    if constexpr (I == NDims - 1) {
      T x = query(q);
      result = op(result, Sign == -1 ? x : op.inv(x));
    } else {
      query<I + 1, -Sign>(result, q, ql, qr);
    }
    q[I] = qr[I];
    if constexpr (I == NDims - 1) {
      T x = query(q);
      result = op(result, Sign == 1 ? x : op.inv(x));
    } else {
      query<I + 1, Sign>(result, q, ql, qr);
    }
  }

  template <size_t I>
  void update(const size_t (&cs)[NDims], size_t index, const T& value) {
    for (int64_t i = cs[I]; i < dims[I]; i = (i | (i + 1))) {
      if constexpr (I == NDims - 1) {
        op.update(t[index + i], value);
      } else {
        update<I + 1>(cs, index + i * strides[I], value);
      }
    }
  }

  template <size_t I>
  void init(const multivector<T, NDims>& a, size_t (&idxs)[NDims]) {
    for (size_t i = 0; i < dims[I]; ++i) {
      idxs[I] = i;
      if constexpr (I == NDims - 1) {
        update(idxs, multi_get<T>(a, idxs)); 
      } else {
        init<I + 1>(a, idxs);
      }
    }
  }

  void init_dims(const size_t (&d)[NDims]) {
    std::copy(d, d + NDims, dims);
    size_t stride = 1;
    for (size_t dim = NDims; dim-- > 0;) {
      strides[dim] = stride;
      stride *= dims[dim];
    }
    t.assign(stride, op.neutral());
  }

  Op op;
  size_t dims[NDims];
  size_t strides[NDims];
  std::vector<T> t;
};

}  // namespace algo::fwt

#endif  // ALGO_FENWICK_TREE_H

