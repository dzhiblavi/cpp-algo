#include <gtest/gtest.h>
#include <random>
#include <optional>

#include "algo/segment_tree/segment_tree.h"
#include "utils/range_query.h"
#include "utils/generate.h"
#include "utils/compare_range_engines.h"

namespace algo::sgt {

template <size_t NDims>
void TestSGTvsNaiveMin(const size_t (&dims)[NDims]) {
  test::CompareRangeEnginesMutable<
    sgt::simple_segment_tree<int, test::min_op<int>, NDims>,
    test::naive_range_query<int, test::min_op<int>, NDims>
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

template <size_t NDims>
void TestSGTvsNaiveMinAndCount(const size_t (&dims)[NDims]) {
  /**
   * @brief For the underlying sement:
   *  - stores minimal value,
   *  - stores the count of values equal to minimal value.
   */
  struct MinAndCountNode {
    using value_type = int;
    MinAndCountNode(value_type x) : min_value(x), min_count(1) {}

    operator std::pair<int, int>() const { return { min_value, min_count }; }

    value_type min_value;
    int min_count;
  };

  /**
   * @brief Combines minimal values and their respective counts appropriately
   */
  struct MinAndCountOp : public base_op<MinAndCountNode, MinAndCountOp> {
    using Node = MinAndCountNode;

    Node neutral() const noexcept { return Node(std::numeric_limits<int>::max()); }

    void update_leaf(Node& node, int value) { node.min_value = value; }

    Node combine(const Node& left, const Node& right) {
      Node node(std::min(left.min_value, right.min_value));
      node.min_count = 0;
      node.min_count += left.min_value == node.min_value ? left.min_count : 0;
      node.min_count += right.min_value == node.min_value ? right.min_count : 0;
      return node;
    }
  };

  test::CompareRangeEnginesMutable<
    sgt::segment_tree<MinAndCountNode, MinAndCountOp, NDims>,
    test::naive_range_query<int, test::min_and_count_op<int>, NDims>,
    int, std::pair<int, int>
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

template <size_t NDims>
void TestSGTvsNaiveSumRangeUpdate(const size_t (&dims)[NDims]) {
  /**
   * @brief For the underlying sement:
   *  - stores sum of segment's values,
   *  - optionally stores the value added to all its elements.
   */
  struct SumAddNode {
    using value_type = int64_t;
    SumAddNode(value_type sum, int64_t size) : sum(sum), size(size) {}

    operator int64_t() const noexcept { return get_sum(); }
    int64_t get_sum() const noexcept { return sum + size * added; }
    void operator=(int64_t x) noexcept { sum = x; }

    int64_t sum;
    int64_t size;
    int64_t added = 0;
  };

  /**
   * @brief Operation for SumAssignNode.
   */
  struct SumAddOp : public base_op<SumAddNode, SumAddOp> {
    using Node = SumAddNode;

    Node neutral() const noexcept { return Node(0, 0); }
    Node init() const noexcept { return Node(0, 1); }

    Node combine(const Node& left, const Node& right) {
      return Node(left.get_sum() + right.get_sum(), left.size + right.size);
    }

    void push(Node& root, Node& left, Node& right) {
      if (root.added == 0) return;

      left.added += root.added;
      right.added += root.added;

      root.sum = left.get_sum() + right.get_sum();
      root.added = 0;
    }

    void update_leaf(Node& node, int64_t value) { node.added += value; }
    void update_range(Node& node, int64_t value) { node.added += value; }
  };

  // This will not work for NDims > 1 because push strategy with sum works for 1D only
  test::CompareRangeEnginesMutableRange<
    sgt::segment_tree<SumAddNode, SumAddOp, NDims>,
    test::naive_range_query<int64_t, test::sum_op<int64_t>, NDims>,
    int64_t, int64_t
  >(dims, 2 * std::accumulate(dims, dims + NDims, 1, std::multiplies<size_t>()));
}

TEST(sgt_min, correctness_dims_1) { TestSGTvsNaiveMin({ 10000 }); }

TEST(sgt_min, correctness_dims_2) { TestSGTvsNaiveMin({ 73, 237 }); }

TEST(sgt_min, correctness_dims_3) { TestSGTvsNaiveMin({ 13, 27, 49 }); }

TEST(sgt_min, correctness_dims_4) { TestSGTvsNaiveMin({ 9, 13, 21, 17 }); }

TEST(sgt_min_and_count, correctness_dims_1) { TestSGTvsNaiveMinAndCount({ 10000 }); }

TEST(sgt_min_and_count, correctness_dims_2) { TestSGTvsNaiveMinAndCount({ 73, 237 }); }

TEST(sgt_min_and_count, correctness_dims_3) { TestSGTvsNaiveMinAndCount({ 13, 27, 49 }); }

TEST(sgt_min_and_count, correctness_dims_4) { TestSGTvsNaiveMinAndCount({ 9, 13, 21, 17 }); }

TEST(sgt_sum_range_update, correctness_dims_1) { TestSGTvsNaiveSumRangeUpdate({ 10000 }); }

}  // namespace algo::sgt

