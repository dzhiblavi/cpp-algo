#ifndef ALGO_SPARSE_TREE_H
#define ALGO_SPARSE_TREE_H

#include <cstddef>
#include <vector>
#include <numeric>
#include <cmath>
#include <cassert>
#include <iostream>
#include <functional>

namespace algo::spt {

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
class sparse_tree {
 public:
  explicit sparse_tree(const multivector<T, NDims>& a) {
    size_t a_dims[NDims];
    fill_sizes<T, NDims, NDims - 1>(a, a_dims);
    std::copy(a_dims, a_dims + NDims, dims);
    init_dims();

    {
      size_t idxs[NDims];
      std::fill(idxs, idxs + NDims, 0);
      build_0<0>(a, idxs, 0);
    }

    size_t idxs[2 * NDims];
    size_t offsets[1ULL << NDims];
    std::fill(idxs, idxs + 2 * NDims, 0);
    std::fill(offsets, offsets + (1ULL << NDims), 0);
    build_q<0>(offsets, 0);
  }

  T get(size_t (&ql)[NDims], size_t (&qr)[NDims]) {
    size_t offsets[1ULL << NDims];
    std::fill(offsets, offsets + (1ULL << NDims), 0);
    return query<0>(ql, qr, offsets);
  }

 private:
  template <size_t I>
  T query(size_t (&ql)[NDims], size_t (&qr)[NDims], size_t (&offsets)[1ULL << NDims]) {
    static constexpr size_t num_offsets = 1ULL << NDims;
    static constexpr size_t step = 1ULL << I;

    size_t l = qr[I] - ql[I] + 1;
    size_t k = std::log2(l);
    size_t pk = 1ULL << k;

    {
      size_t j = 0;
      while (j < num_offsets) {
        for (size_t z = 0; z < step; ++z, ++j) {
          offsets[j] += strides[I] * ql[I] + strides[I + NDims] * k;
        }
        for (size_t z = 0; z < step; ++z, ++j) {
          offsets[j] += strides[I] * (1 + qr[I] - pk) + strides[I + NDims] * k;
        }
      }
    }

    if constexpr (I == NDims - 1) {
      return std::transform_reduce(
          offsets, offsets + num_offsets, op.neutral(),
          op, [this] (size_t offset) { return s[offset]; });
    } else {
      return query<I + 1>(ql, qr, offsets);
    }
  }

  template <size_t I>
  void build_0(const multivector<T, NDims>& a, size_t (&idxs)[NDims], size_t offset) {
    for (size_t i = 0; i < dims[I]; ++i) {
      idxs[I] = i;
      size_t new_offset = offset + i * strides[I];
      if constexpr (I == NDims - 1) {
        s[new_offset] = multi_get<T>(a, idxs);
      } else {
        build_0<I + 1>(a, idxs, new_offset);
      }
    }
  }

  template <size_t I>
  void build_q(const size_t (&offsets)[1ULL << NDims], size_t offset) {
    static constexpr size_t num_offsets = 1ULL << NDims;
    static constexpr size_t step = 1ULL << I;
    size_t s_offsets[num_offsets];

    for (size_t k = 0; k < dims[I + NDims]; ++k) {
      for (size_t i = 0; i + (1ULL << k) <= dims[I]; ++i) {
        std::copy(offsets, offsets + num_offsets, s_offsets);

        if (k == 0) {
          for (size_t z = 0; z < num_offsets; ++z) {
            s_offsets[z] += strides[I] * i;
          }
        } else {
          size_t j = 0;
          while (j < num_offsets) {
            for (size_t z = 0; z < step; ++z, ++j) {
              s_offsets[j] += strides[I] * i + strides[I + NDims] * (k - 1);
            }
            for (size_t z = 0; z < step; ++z, ++j) {
              s_offsets[j] += strides[I] * (i + (1ULL << (k - 1))) + strides[I + NDims] * (k - 1);
            }
          }
        }

        size_t new_offset = offset + i * strides[I] + k * strides[I + NDims];
        if constexpr (I == NDims - 1) {
          s[new_offset] = std::transform_reduce(
              s_offsets, s_offsets + num_offsets, op.neutral(),
              op, [this] (size_t offset) { return s[offset]; });
        } else {
          build_q<I + 1>(s_offsets, new_offset);
        }
      }
    }
  }

  void init_dims() {
    for (size_t i = NDims; i < 2 * NDims; ++i) {
      dims[i] = 1 + std::log2(dims[i - NDims]);
    }
    size_t stride = 1;
    for (size_t dim = 2 * NDims; dim-- > 0;) {
      strides[dim] = stride;
      stride *= dims[dim];
    }
    s.assign(stride, op.neutral());
  }

  Op op;
  size_t dims[2 * NDims];
  size_t strides[2 * NDims];
  std::vector<T> s;
};

}  // namespace algo::spt

#endif  // ALGO_SPARSE_TREE_H

