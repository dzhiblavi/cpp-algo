#pragma once

#include "algo/common/types.h"
#include "algo/utility/nd_container.h"

#include <cstddef>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

namespace algo::fwt {

template <typename T, typename Op, size_t NDims = 1>
class FenwickTree : public types::StatelessEngineBase<NDims> {
 public:
  using QueryHandle = typename types::StatelessEngineBase<NDims>::QueryHandle;
  using RangeQueryHandle = typename types::StatelessEngineBase<NDims>::RangeQueryHandle;

  explicit FenwickTree(const types::Index<NDims>& dims) {
    initDims(dims);
  }

  explicit FenwickTree(utility::NDView<T, NDims> auto view) {
    types::Index<NDims> d;
    view.getDimensions(d);
    initDims(d);
    init<0>(view, d);
  }

  void update(QueryHandle& handle, const T& value) {
    update<0>(handle.getIndex(), 0, value);
  }

  T query(QueryHandle& handle) {
    T result = op_.neutral();
    query<0>(result, handle.getIndex(), 0);
    return result;
  }

  T query(RangeQueryHandle& handle) {
    types::Index<NDims> q;
    T result = op_.neutral();
    query<0, 1>(result, q, handle.getRange().left, handle.getRange().right);
    return result;
  }

 private:
  template <size_t I>
  void query(T& result, const types::Index<NDims>& qr, size_t index) {
    for (int64_t i = qr[I]; i >= 0; i = (i & (i + 1)) - 1) {
      if constexpr (I == NDims - 1) {
        result = op_(result, storage_[index + i]);
      } else {
        query<I + 1>(result, qr, index + i * strides_[I]);
      }
    }
  }

  template <size_t I, int Sign>
  void query(T& result, types::Index<NDims>& q, const types::Index<NDims>& ql, const types::Index<NDims>& qr) {
    q[I] = ql[I] - 1;
    if constexpr (I == NDims - 1) {
      T x = query(q);
      result = op_(result, Sign == -1 ? x : op_.inv(x));
    } else {
      query<I + 1, -Sign>(result, q, ql, qr);
    }
    q[I] = qr[I];
    if constexpr (I == NDims - 1) {
      T x = query(q);
      result = op_(result, Sign == 1 ? x : op_.inv(x));
    } else {
      query<I + 1, Sign>(result, q, ql, qr);
    }
  }

  template <size_t I>
  void update(const types::Index<NDims>& cs, size_t index, const T& value) {
    for (int64_t i = cs[I]; i < dims_[I]; i = (i | (i + 1))) {
      if constexpr (I == NDims - 1) {
        op_.update(storage_[index + i], value);
      } else {
        update<I + 1>(cs, index + i * strides_[I], value);
      }
    }
  }

  template <size_t I, utility::NDView<T, NDims> NDView>
  void init(const NDView& a, types::Index<NDims>& idxs) {
    for (size_t i = 0; i < dims_[I]; ++i) {
      idxs[I] = i;
      if constexpr (I == NDims - 1) {
        update(idxs, a.at(idxs));
      } else {
        init<I + 1>(a, idxs);
      }
    }
  }

  void initDims(const types::Index<NDims>& d) {
    std::copy(d.begin(), d.end(), dims_.begin());
    size_t stride = 1;
    for (size_t dim = NDims; dim-- > 0;) {
      strides_[dim] = stride;
      stride *= dims_[dim];
    }
    storage_.assign(stride, op_.neutral());
  }

  [[no_unique_address]] Op op_;
  types::Index<NDims> dims_;
  types::Index<NDims> strides_;
  std::vector<T> storage_;
};

template <typename T>
struct MinOp {
  T neutral() const noexcept {
    return std::numeric_limits<T>::max();
  }
  T operator()(const T& a, const T& b) const noexcept {
    return std::min(a, b);
  }

  /* Set value to x */
  void update(T& t, const T& x) const noexcept {
    t = std::min(t, x);
  }
};

template <typename T>
struct MaxOp {
  T neutral() const noexcept {
    return std::numeric_limits<T>::min();
  }
  T operator()(const T& a, const T& b) const noexcept {
    return std::max(a, b);
  }

  /* Set value to x */
  void update(T& t, const T& x) const noexcept {
    t = std::max(t, x);
  }
};

template <typename T>
struct SumOp : public std::plus<T> {
  T neutral() const noexcept {
    return T(0);
  }
  T inv(const T& x) {
    return -x;
  }

  /* Increase value by x */
  void update(T& t, const T& x) const noexcept {
    t += x;
  }
};

}  // namespace algo::fwt
