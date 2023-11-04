#pragma once

#include "algo/graph/repr/adj_matrix/repr.h"
#include "algo/graph/repr/concepts.h"

namespace algo::graph::repr {

template <
    typename NodeIdType = uint32_t,              //
    typename EdgePayloadType = empty_payload_t,  //
    typename StorageType = DynamicStorage<>,     //
    bool Direct = false,                         //
    std::integral SizeType = std::size_t>
class AdjMatrixGraph;

template <typename NodeIdType, typename EdgePayloadType, typename StorageType, bool Direct, std::integral SizeType>
class AdjMatrixGraphTransposedView
    : public detail::AdjMatrixGraph<
          NodeIdType, EdgePayloadType,
          AdjMatrixGraphTransposedView<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>>,
      public ContinuousNodeIndexTag<true>,
      public DirectTag<Direct> {
  using Wrapped = AdjMatrixGraph<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>;

  friend class detail::AdjMatrixGraph<
      NodeIdType, EdgePayloadType,
      AdjMatrixGraphTransposedView<NodeIdType, EdgePayloadType, StorageType, Direct, SizeType>>;

 public:
  using NodeId = typename Wrapped::NodeId;
  using EdgePayload = typename Wrapped::EdgePayload;

  explicit AdjMatrixGraphTransposedView(Wrapped* graph) noexcept : graph_{graph} {}

  auto& originalGraph() noexcept { return *graph_; }
  const SizeType& nodeCount() const noexcept { return graph_->nodeCount(); }
  const SizeType& edgeCount() const noexcept { return graph_->edgeCount(); }

 private:
  SizeType& nodeCountMutable() noexcept { return graph_->nodeCountMutable(); }
  SizeType& edgeCountMutable() noexcept { return graph_->edgeCountMutable(); }

  auto& get(size_t i, size_t j) noexcept { return graph_->get(j, i); }
  const auto& get(size_t i, size_t j) const noexcept { return graph_->get(j, i); }

  void resize(size_t capacity) { graph_->resize(capacity); }
  void reserve(size_t capacity) { graph_->reserve(capacity); }

  Wrapped* graph_;
};

}  // namespace algo::graph::repr