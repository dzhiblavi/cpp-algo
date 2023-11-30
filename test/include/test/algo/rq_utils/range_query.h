#pragma once

#include "algo/common/types.h"
#include "algo/utility/nd_container.h"
#include "generate.h"

#include <cstddef>
#include <functional>
#include <limits>
#include <vector>

namespace test::rq_utils {

template <typename T, typename Op, size_t NDims>
class NaiveRangeQueryEngine;

namespace detail {

template <size_t NDims>
struct QueryHandle {
  algo::types::Index<NDims>& getIndex() noexcept {
    return index_;
  }

 private:
  algo::types::Index<NDims> index_;

  template <typename, typename, size_t>
  friend class test::rq_utils::NaiveRangeQueryEngine;
};

template <size_t NDims>
struct RangeQueryHandle {
  algo::types::Range<NDims>& getRange() noexcept {
    return range_;
  }

 private:
  algo::types::Range<NDims> range_;

  template <typename, typename, size_t>
  friend class test::rq_utils::NaiveRangeQueryEngine;
};

}  // namespace detail

template <typename T, typename Op, size_t NDims>
class NaiveRangeQueryEngine {
 public:
  using QueryHandle = detail::QueryHandle<NDims>;
  using RangeQueryHandle = detail::RangeQueryHandle<NDims>;

  template <algo::utility::NDView<T, NDims> NDView>
  explicit NaiveRangeQueryEngine(NDView source)
      : container_(shapeAs<T, NDims>(source)), view_(algo::utility::asView(container_)) {
    copyView<T, NDims>(source, view_);
  }

  [[nodiscard]] QueryHandle getQueryHandle() const noexcept {
    return {};
  }

  [[nodiscard]] RangeQueryHandle getRangeQueryHandle() const noexcept {
    return {};
  }

  auto query(RangeQueryHandle& handle) {
    auto& [ql, qr] = handle.getRange();
    std::array<size_t, NDims> idxs;
    return query<0>(ql, qr, idxs);
  }

  void update(QueryHandle& handle, const T& value) {
    view_.at(handle.getIndex()) = value;
  }

  void updateRange(RangeQueryHandle& handle, const T& value) {
    auto& [ul, ur] = handle.getRange();
    std::array<size_t, NDims> idxs;
    updateRange<0>(ul, ur, idxs, value);
  }

 private:
  template <size_t I>
  void updateRange(
      std::array<size_t, NDims>& ul, std::array<size_t, NDims>& ur, std::array<size_t, NDims>& idxs, const T& value) {
    for (size_t i = ul[I]; i <= ur[I]; ++i) {
      idxs[I] = i;
      if constexpr (I == NDims - 1) {
        view_.at(idxs) += value;
      } else {
        updateRange<I + 1>(ul, ur, idxs, value);
      }
    }
  }

  template <size_t I>
  auto query(std::array<size_t, NDims>& ql, std::array<size_t, NDims>& qr, std::array<size_t, NDims>& idxs) {
    auto result = op_.neutral();
    for (size_t i = ql[I]; i <= qr[I]; ++i) {
      idxs[I] = i;
      if constexpr (I == NDims - 1) {
        result = op_(result, view_.at(idxs));
      } else {
        result = op_(result, query<I + 1>(ql, qr, idxs));
      }
    }
    return result;
  }

  [[no_unique_address]] Op op_;
  algo::utility::NDVector<T, NDims> container_;
  algo::utility::NDContainerView<decltype(container_)> view_;
};

template <typename T>
struct MinOp {
  T operator()(const T& a, const T& b) const noexcept {
    return std::min(a, b);
  }
  T neutral() const noexcept {
    return std::numeric_limits<T>::max();
  }
  T init() const noexcept {
    return neutral();
  }
};

template <typename T>
struct MaxOp {
  T operator()(const T& a, const T& b) const noexcept {
    return std::max(a, b);
  }
  T neutral() const noexcept {
    return std::numeric_limits<T>::min();
  }
  T init() const noexcept {
    return neutral();
  }
};

template <typename T>
struct SumOp : public std::plus<T> {
  T neutral() const noexcept {
    return T(0);
  }
  T init() const noexcept {
    return neutral();
  }
};

template <typename T>
struct MinAndCountOp {
  using value = std::pair<T, int>;

  value neutral() const noexcept {
    return {std::numeric_limits<T>::max(), 1};
  }
  value init() const noexcept {
    return neutral();
  }

  value operator()(const value& a, const value& b) const noexcept {
    if (a.first < b.first) {
      return a;
    } else if (b.first < a.first) {
      return b;
    } else {
      return {a.first, a.second + b.second};
    }
  }

  value operator()(const value& a, const T& b) const noexcept {
    return (*this)(a, {b, 1});
  }
};

}  // namespace test::rq_utils
