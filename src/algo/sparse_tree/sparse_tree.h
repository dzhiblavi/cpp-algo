#pragma once

#include "algo/common/types.h"
#include "algo/utility/nd_container.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <functional>
#include <numeric>
#include <vector>

namespace algo::spt {

template <typename T, typename Op, size_t NDims = 1>
class SparseTree;

namespace detail {

template <size_t NDims>
struct QueryHandle {
  types::Index<NDims>& getIndex() noexcept { return index_; }

 private:
  types::Index<NDims> index_;
  types::Index<1ULL << NDims> offsets_{0};

  template <typename T, typename Op, size_t Dims>
  friend class spt::SparseTree;
};

template <size_t NDims>
struct RangeQueryHandle {
  types::Range<NDims>& getRange() noexcept { return range_; }

 private:
  types::Range<NDims> range_;
  types::Index<1ULL << NDims> offsets_{0};

  template <typename T, typename Op, size_t Dims>
  friend class spt::SparseTree;
};

}  // namespace detail

template <typename T, typename Op, size_t NDims>
class SparseTree {
 public:
  using QueryHandle = detail::QueryHandle<NDims>;
  using RangeQueryHandle = detail::RangeQueryHandle<NDims>;

  explicit SparseTree(utility::NDView<T, NDims> auto view) {
    types::Index<NDims> a_dims;
    view.getDimensions(a_dims);
    std::copy(a_dims.begin(), a_dims.end(), dims_.begin());
    initDims();

    {
      types::Index<NDims> idxs{0};
      build_0<0>(view, idxs, 0);
    }

    types::Index<1ULL << NDims> offsets{0};
    build_q<0>(offsets, 0);
  }

  [[nodiscard]] QueryHandle getQueryHandle() const noexcept { return {}; }

  [[nodiscard]] RangeQueryHandle getRangeQueryHandle() const noexcept { return {}; }

  T query(RangeQueryHandle& handle) {
    std::fill(handle.offsets_.begin(), handle.offsets_.end(), 0);
    return query<0>(handle);
  }

 private:
  template <size_t I>
  T query(RangeQueryHandle& handle) {
    static constexpr size_t num_offsets = 1ULL << NDims;
    static constexpr size_t step = 1ULL << I;

    auto& [range, offsets] = handle;
    auto& [ql, qr] = range;

    size_t l = qr[I] - ql[I] + 1;
    size_t k = std::log2(l);
    size_t pk = 1ULL << k;

    {
      size_t j = 0;
      while (j < num_offsets) {
        for (size_t z = 0; z < step; ++z, ++j) {
          offsets[j] += strides_[I] * ql[I] + strides_[I + NDims] * k;
        }
        for (size_t z = 0; z < step; ++z, ++j) {
          offsets[j] += strides_[I] * (1 + qr[I] - pk) + strides_[I + NDims] * k;
        }
      }
    }

    if constexpr (I == NDims - 1) {
      return std::transform_reduce(
          offsets.begin(), offsets.begin() + num_offsets, op_.neutral(), op_,
          [this](size_t offset) { return storage_[offset]; });
    } else {
      return query<I + 1>(handle);
    }
  }

  template <size_t I>
  void build_0(utility::NDView<T, NDims> auto view, types::Index<NDims>& idxs, size_t offset) {
    for (size_t i = 0; i < dims_[I]; ++i) {
      idxs[I] = i;
      size_t new_offset = offset + i * strides_[I];
      if constexpr (I == NDims - 1) {
        storage_[new_offset] = view.at(idxs);
      } else {
        build_0<I + 1>(view, idxs, new_offset);
      }
    }
  }

  template <size_t I>
  void build_q(const types::Index<1ULL << NDims>& offsets, size_t offset) {
    static constexpr size_t kNumOffsets = 1ULL << NDims;
    static constexpr size_t step = 1ULL << I;
    types::Index<kNumOffsets> s_offsets;

    for (size_t k = 0; k < dims_[I + NDims]; ++k) {
      for (size_t i = 0; i + (1ULL << k) <= dims_[I]; ++i) {
        std::copy(offsets.begin(), offsets.end(), s_offsets.begin());

        if (k == 0) {
          for (auto& offset : s_offsets) {
            offset += strides_[I] * i;
          }
        } else {
          size_t j = 0;
          while (j < kNumOffsets) {
            for (size_t z = 0; z < step; ++z, ++j) {
              s_offsets[j] += strides_[I] * i + strides_[I + NDims] * (k - 1);
            }
            for (size_t z = 0; z < step; ++z, ++j) {
              s_offsets[j] += strides_[I] * (i + (1ULL << (k - 1))) + strides_[I + NDims] * (k - 1);
            }
          }
        }

        size_t new_offset = offset + i * strides_[I] + k * strides_[I + NDims];
        if constexpr (I == NDims - 1) {
          storage_[new_offset] = std::transform_reduce(
              s_offsets.begin(), s_offsets.end(), op_.neutral(), op_,
              [this](size_t offset) { return storage_[offset]; });
        } else {
          build_q<I + 1>(s_offsets, new_offset);
        }
      }
    }
  }

  void initDims() {
    for (size_t i = NDims; i < 2 * NDims; ++i) {
      dims_[i] = 1 + std::log2(dims_[i - NDims]);
    }
    size_t stride = 1;
    for (size_t dim = 2 * NDims; dim-- > 0;) {
      strides_[dim] = stride;
      stride *= dims_[dim];
    }
    storage_.assign(stride, op_.neutral());
  }

  [[no_unique_address]] Op op_;
  types::Index<2 * NDims> dims_;
  types::Index<2 * NDims> strides_;
  std::vector<T> storage_;
};

}  // namespace algo::spt
