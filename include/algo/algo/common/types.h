#pragma once

#include <array>
#include <cstddef>

namespace algo::types {

template <size_t NDims>
class Index : public std::array<size_t, NDims> {
 public:
  Index& getIndex() noexcept {
    return *this;
  }
};

template <size_t NDims>
class Range {
 public:
  Range& getRange() noexcept {
    return *this;
  }

  Index<NDims> left;
  Index<NDims> right;
};

template <size_t NDims>
class StatelessEngineBase {
 public:
  using QueryHandle = Index<NDims>;
  using RangeQueryHandle = Range<NDims>;

  [[nodiscard]] QueryHandle getQueryHandle() const noexcept {
    return {};
  }

  [[nodiscard]] RangeQueryHandle getRangeQueryHandle() const noexcept {
    return {};
  }
};

}  // namespace algo::types