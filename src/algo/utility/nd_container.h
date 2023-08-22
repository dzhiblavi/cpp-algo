#pragma once

#include "nd_helpers.h"

namespace algo::utility {

template <typename T, size_t NDims>
struct NDViewTag {};

template <typename Container>
class NDContainerView;

template <typename Container>
NDContainerView<Container> asView(Container& container) noexcept;

template <typename Container>
class NDContainerView
    : public NDViewTag<detail::getValueType<Container>, detail::countDimensions<Container>> {
 public:
  static constexpr size_t NDims = detail::countDimensions<Container>;
  using ValueType = detail::getValueType<Container>;

  [[nodiscard]] decltype(auto) at(std::same_as<size_t> auto... indices) noexcept {
    static_assert(sizeof...(indices) > 0);
    return atImpl<sizeof...(indices)>(*this, std::make_tuple(indices...));
  }

  [[nodiscard]] decltype(auto) at(std::same_as<size_t> auto... indices) const noexcept {
    static_assert(sizeof...(indices) > 0);
    return atImpl<sizeof...(indices)>(*this, std::make_tuple(indices...));
  }

  template <size_t NI>
  [[nodiscard]] decltype(auto) at(const std::array<size_t, NI>& indices) noexcept {
    static_assert(NI <= NDims && NI > 0);
    return atImpl<NI>(*this, indices);
  }

  template <size_t NI>
  [[nodiscard]] decltype(auto) at(const std::array<size_t, NI>& indices) const noexcept {
    static_assert(NI <= NDims && NI > 0);
    return atImpl<NI>(*this, indices);
  }

  [[nodiscard]] size_t frontDimension() const noexcept {
    return container_->size();
  }

  template <typename It>
  void getDimensions(It first, It last) const noexcept {
    assert(std::distance(first, last) == NDims);
    assert(!container_->empty());

    *first = container_->size();
    if constexpr (NDims > 1) {
      asView(container_->front()).getDimensions(std::next(first), last);
    }
  }

  void getDimensions(std::array<size_t, NDims>& dims) const noexcept {
    getDimensions(dims.begin(), dims.end());
  }

 private:
  template <size_t IndicesCount>
  using AtResultType = std::conditional_t<
      IndicesCount == NDims, ValueType&,
      NDContainerView<NDVector<ValueType, NDims - IndicesCount>>>;

  explicit NDContainerView(Container& container) noexcept : container_{&container} {}

  template <size_t IndicesCount, typename Self, typename Indices>
  static AtResultType<IndicesCount> atImpl(Self&& self, Indices indices) noexcept {
    static const size_t kTupleSize = std::tuple_size_v<Indices>;
    const size_t index = std::get<0>(indices);
    assert(0 <= index && index < self.container_->size());
    auto& item = (*self.container_)[index];

    if constexpr (kTupleSize == 1) {
      if constexpr (IndicesCount < NDims) {
        return asView(item);
      } else {
        return item;
      }
    } else {
      return std::apply(
          [&item](size_t skip, auto... indices) -> AtResultType<IndicesCount> {
            return asView(item).at(indices...);
          },
          indices);
    }
  }

  template <typename C>
  friend NDContainerView<C> asView(C& container) noexcept;

  Container* container_;
};

template <typename Container>
NDContainerView<Container> asView(Container& container) noexcept {
  return NDContainerView<Container>(container);
}

template <typename C, typename T, size_t NDims>
static constexpr bool IsNDView = std::is_base_of_v<NDViewTag<T, NDims>, C>;

template <typename C, typename T, size_t NDims>
concept NDView = IsNDView<C, T, NDims>;

}  // namespace algo::utility