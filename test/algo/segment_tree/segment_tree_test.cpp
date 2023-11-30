#include "algo/segment_tree/segment_tree.h"

#include "test/algo/rq_utils/compare_range_engines.h"
#include "test/algo/rq_utils/generate.h"
#include "test/algo/rq_utils/range_query.h"

#include <gtest/gtest.h>
#include <optional>
#include <random>

namespace test::sgt::unit {

namespace sgt = ::algo::sgt;

template <size_t NDims>
void TestSGTvsNaiveMin(const std::array<size_t, NDims>& dims) {
  rq_utils::compareRangeEnginesMutable<
      sgt::SimpleSegmentTree<int, rq_utils::MinOp<int>, NDims>,
      rq_utils::NaiveRangeQueryEngine<int, rq_utils::MinOp<int>, NDims> >(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

template <size_t NDims>
void TestSGTvsNaiveMinAndCount(const std::array<size_t, NDims>& dims) {
  /**
   * @brief For the underlying segment:
   *  - stores minimal value,
   *  - stores the count of values equal to minimal value.
   */
  struct MinAndCountNode {
    using value_type = int;
    MinAndCountNode(value_type x) : min_value(x), min_count(1) {}

    operator std::pair<int, int>() const {
      return {min_value, min_count};
    }

    value_type min_value;
    int min_count;
  };

  /**
   * @brief Combines minimal values and their respective counts appropriately
   */
  struct MinAndCountOp : public sgt::base_op<MinAndCountNode, MinAndCountOp> {
    using Node = MinAndCountNode;

    Node neutral() const noexcept {
      return Node(std::numeric_limits<int>::max());
    }

    void updateLeaf(Node& node, int value) {
      node.min_value = value;
    }

    Node combine(const Node& left, const Node& right) {
      Node node(std::min(left.min_value, right.min_value));
      node.min_count = 0;
      node.min_count += left.min_value == node.min_value ? left.min_count : 0;
      node.min_count += right.min_value == node.min_value ? right.min_count : 0;
      return node;
    }
  };

  rq_utils::compareRangeEnginesMutable<
      sgt::SegmentTree<MinAndCountNode, MinAndCountOp, NDims>,
      rq_utils::NaiveRangeQueryEngine<int, rq_utils::MinAndCountOp<int>, NDims>, int, std::pair<int, int> >(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

template <size_t NDims>
void TestSGTvsNaiveSumRangeUpdate(const std::array<size_t, NDims>& dims) {
  /**
   * @brief For the underlying segment:
   *  - stores sum of segment's values,
   *  - optionally stores the value added to all its elements.
   */
  struct SumAddNode {
    using value_type = int64_t;
    SumAddNode(value_type sum, int64_t size) : sum(sum), size(size) {}

    operator int64_t() const noexcept {
      return get_sum();
    }
    int64_t get_sum() const noexcept {
      return sum + size * added;
    }
    void operator=(int64_t x) noexcept {
      sum = x;
    }

    int64_t sum;
    int64_t size;
    int64_t added = 0;
  };

  /**
   * @brief Operation for SumAssignNode.
   */
  struct SumAddOp : public sgt::base_op<SumAddNode, SumAddOp> {
    using Node = SumAddNode;

    Node neutral() const noexcept {
      return Node(0, 0);
    }
    Node init() const noexcept {
      return Node(0, 1);
    }

    Node combine(const Node& left, const Node& right) {
      return Node(left.get_sum() + right.get_sum(), left.size + right.size);
    }

    void push(Node& root, Node& left, Node& right) {
      if (root.added == 0)
        return;

      left.added += root.added;
      right.added += root.added;

      root.sum = left.get_sum() + right.get_sum();
      root.added = 0;
    }

    void updateLeaf(Node& node, int64_t value) {
      node.added += value;
    }
    void updateRange(Node& node, int64_t value) {
      node.added += value;
    }
  };

  // This will not work for NDims > 1 because push strategy with sum works for 1D only
  rq_utils::compareRangeEnginesMutableRange<
      sgt::SegmentTree<SumAddNode, SumAddOp, NDims>,
      rq_utils::NaiveRangeQueryEngine<int64_t, rq_utils::SumOp<int64_t>, NDims>, int64_t, int64_t>(
      dims, 2 * std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<size_t>()));
}

TEST(SegmentTreeTest, MinDims1Correctness) {
  TestSGTvsNaiveMin<1>({10000});
}
TEST(SegmentTreeTest, MinDims2Correctness) {
  TestSGTvsNaiveMin<2>({73, 237});
}
TEST(SegmentTreeTest, MinDims3Correctness) {
  TestSGTvsNaiveMin<3>({13, 27, 49});
}
TEST(SegmentTreeTest, MinDims4Correctness) {
  TestSGTvsNaiveMin<4>({9, 13, 21, 17});
}

TEST(SegmentTreeTest, MinAndCountDims1Correctness) {
  TestSGTvsNaiveMinAndCount<1>({10000});
}
TEST(SegmentTreeTest, MinAndCountDims2Correctness) {
  TestSGTvsNaiveMinAndCount<2>({73, 237});
}
TEST(SegmentTreeTest, MinAndCountDims3Correctness) {
  TestSGTvsNaiveMinAndCount<3>({13, 27, 49});
}
TEST(SegmentTreeTest, MinAndCountDims4Correctness) {
  TestSGTvsNaiveMinAndCount<4>({9, 13, 21, 17});
}

TEST(SegmentTreeTest, SumRangeUpdateDims1Correctness) {
  TestSGTvsNaiveSumRangeUpdate<1>({10000});
}

}  // namespace test::sgt::unit
