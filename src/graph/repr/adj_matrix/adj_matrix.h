#pragma once

#include "graph/repr/adj_matrix/repr.h"
#include "graph/repr/adj_matrix/transposed.h"
#include "graph/repr/concepts.h"

namespace algo::graph::repr {

template <typename NodeIdType, typename EdgePayloadType, typename StorageType, bool Direct, std::integral SizeType>
class AdjMatrixGraph
    : public detail::AdjMatrixGraph<
          NodeIdType, EdgePayloadType, AdjMatrixGraph<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>>,
      public ContinuousNodeIndexTag<true>,
      public DirectTag<Direct> {
  using Transposed = AdjMatrixGraphTransposedView<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>;
  friend class AdjMatrixGraphTransposedView<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>;
  friend class detail::AdjMatrixGraph<
      NodeIdType, EdgePayloadType, AdjMatrixGraph<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>>;

 public:
  using NodeId = NodeIdType;
  using EdgePayload = EdgePayloadType;

  AdjMatrixGraph() noexcept(!StorageType::kMaxSize || StorageType::kIsStatic) {
    if (StorageType::kMaxSize > 0 && !StorageType::kIsStatic) {
      reserve(StorageType::kMaxSize);
    }
  }

  explicit AdjMatrixGraph(size_t node_count) noexcept(StorageType::kIsStatic) {  //
    resize(node_count);
  }

  auto transposedView() noexcept { return Transposed(this); }
  const SizeType& nodeCount() const noexcept { return node_count_; }
  const SizeType& edgeCount() const noexcept { return edge_count_; }

 private:
  using StorageContainer = std::conditional_t<
      StorageType::kIsStatic,
      std::array<std::array<std::optional<EdgePayload>, StorageType::kMaxSize>, StorageType::kMaxSize>,
      std::vector<std::vector<std::optional<EdgePayload>>>>;

  SizeType& nodeCountMutable() noexcept { return node_count_; }
  SizeType& edgeCountMutable() noexcept { return edge_count_; }

  auto& get(size_t i, size_t j) noexcept {
    normalize(i, j);
    return c_[i][j];
  }

  const auto& get(size_t i, size_t j) const noexcept {
    normalize(i, j);
    return c_[i][j];
  }

  void resize(size_t node_count) noexcept(StorageType::kIsStatic) {
    if constexpr (StorageType::kIsStatic) {
      assert(node_count <= StorageType::kMaxSize);
    } else {
      c_.resize(node_count);
      for (auto& row : c_) {
        row.resize(node_count);
      }
    }

    node_count_ = node_count;
  }

  void reserve(size_t capacity) {
    if constexpr (!StorageType::kIsStatic) {
      c_.reserve(capacity);
      for (auto& row : c_) {
        row.reserve(capacity);
      }
    }
  }

  static void normalize(auto& from, auto& to) {
    if constexpr (!Direct) {
      if (from > to) {
        std::swap(from, to);
      }
    }
  }

  SizeType node_count_{0};
  SizeType edge_count_{0};
  StorageContainer c_;
};

}  // namespace algo::graph::repr