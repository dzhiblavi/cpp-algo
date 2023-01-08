#ifndef ALGO_TEST_RANGE_QUERY_H
#define ALGO_TEST_RANGE_QUERY_H

#include <cstddef>
#include <vector>
#include <limits>
#include <functional>

namespace algo::test {

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
T& multi_get(multivector<T, NDims - Dim>& a, size_t (&idxs)[NDims]) {
  if constexpr (Dim == NDims - 1) {
    return a[idxs[Dim]];
  } else {
    return multi_get<T, NDims, Dim + 1>(a[idxs[Dim]], idxs);
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

template <typename T, typename Op, size_t NDims>
class naive_range_query {
 public:
  explicit naive_range_query(multivector<T, NDims>& x) : a(x) {}

  T get(size_t (&ql)[NDims], size_t (&qr)[NDims]) {
    size_t idxs[NDims];
    std::fill(idxs, idxs + NDims, 0);
    return query<0>(ql, qr, idxs);
  }

  void set(size_t (&cs)[NDims], const T& value) { multi_get<T>(a, cs) = value; }

 private:
  template <size_t I>
  T query(size_t (&ql)[NDims], size_t (&qr)[NDims], size_t (&idxs)[NDims]) {
    T result = op.neutral();
    for (size_t i = ql[I]; i <= qr[I]; ++i) {
      idxs[I] = i;
      if constexpr (I == NDims - 1) {
        result = op(result, multi_get<T>(a, idxs));
      } else {
        result = op(result, query<I + 1>(ql, qr, idxs));
      }
    }
    return result;
  }

  Op op;
  multivector<T, NDims>& a;  
};

template <typename T>
struct min_op {
  T operator()(const T& a, const T& b) const noexcept { return std::min(a, b); }
  T neutral() const noexcept { return std::numeric_limits<T>::max(); }
};

template <typename T>
struct max_op {
  T operator()(const T& a, const T& b) const noexcept { return std::max(a, b); }
  T neutral() const noexcept { return std::numeric_limits<T>::min(); }
};

template <typename T>
struct sum_op : public std::plus<T> {
  T neutral() const noexcept { return T(0); }
};

}  // namespace algo::test


#endif  // ALGO_TEST_RANGE_QUERY_H

