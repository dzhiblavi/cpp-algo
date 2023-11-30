#pragma once

#include <concepts>

namespace algo::graph::repr {

namespace edge {

template <typename EdgePayload>
concept Weighted = requires(const EdgePayload& edge) {
  typename EdgePayload::WeightType;
  requires std::integral<typename EdgePayload::WeightType>;
  { edge.weight() } -> std::same_as<typename EdgePayload::WeightType>;
};

}  // namespace edge

template <bool Value>
struct DirectTag : public std::integral_constant<bool, Value> {};

template <bool Value>
struct ContinuousNodeIndexTag : public std::integral_constant<bool, Value> {};

template <typename G>
concept Graph =  //
    requires(G& g, const G& cg) {
      typename G::NodeId;
      typename G::EdgePayload;

      // { G::continuousNodeIndexing } -> std::convertible_to<bool>;
      // { G::isDirected } -> std::convertible_to<bool>;

      { cg.nodeCount() } -> std::convertible_to<std::size_t>;
      { cg.edgeCount() } -> std::convertible_to<std::size_t>;
      { cg.edgesFrom(std::declval<typename G::NodeId>()) };
      { g.addNodes(0) };

      // { g.addEdge(std::declval<typename G::NodeId>(), std::declval<typename
      // G::NodeId>()) };
    };

template <typename G>
concept ContinuousNodeIndexGraph = (Graph<G> && std::is_base_of_v<ContinuousNodeIndexTag<true>, G>);

template <typename G>
concept DirectGraph = (Graph<G> && std::is_base_of_v<DirectTag<true>, G>);

template <typename G>
concept IndirectGraph = (Graph<G> && std::is_base_of_v<DirectTag<false>, G>);

template <typename G>
concept WeightedGraph = (Graph<G> && edge::Weighted<typename G::EdgePayload>);

}  // namespace algo::graph::repr