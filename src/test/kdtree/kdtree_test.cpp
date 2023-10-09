#include "algo/kdtree/kdtree.h"

#include "test/utils/generate.h"

#include <gtest/gtest.h>
#include <random>

namespace algo::kdtree::unit {

template <typename PointType, size_t Dims, typename DistanceBinaryFunctor = kdtree::EuclideanDistance<PointType>>
class NaiveKDTree {
 public:
  explicit NaiveKDTree(std::vector<PointType> points) noexcept : points_{std::move(points)} {}

  const PointType& nnSearch(const PointType& q) {
    return *std::min_element(points_.begin(), points_.end(), [&q, this](const auto& a, const auto& b) {
      return distance_(a, q) < distance_(b, q);
    });
  }

  std::vector<PointType> knnSearch(const PointType& q, size_t max_count) {
    std::sort(points_.begin(), points_.end(), [&q, this](const auto& a, const auto& b) {
      return distance_(a, q) < distance_(b, q);
    });
    return {points_.begin(), points_.begin() + std::min(max_count, points_.size())};
  }

  std::vector<PointType> radiusSearch(const PointType& q, uint64_t radius) {
    radius *= radius;
    std::vector<PointType> output;
    std::copy_if(points_.begin(), points_.end(), std::back_inserter(output), [&q, radius, this](const PointType& p) {
      return distance_(q, p) < radius;
    });
    return output;
  }

 private:
  [[no_unique_address]] DistanceBinaryFunctor distance_;
  std::vector<PointType> points_;
};

template <typename PointType, typename DistanceBinaryFunctor>
void verifySameDistances(const PointType& to, const PointType& a, const PointType& b) {
  DistanceBinaryFunctor distance;
  EXPECT_EQ(distance(to, a), distance(to, b));
}

template <typename PointType, typename DistanceBinaryFunctor>
void verifySameDistances(const PointType& to, const std::vector<PointType>& p1, const std::vector<PointType>& p2) {
  std::vector<uint64_t> p1d;
  std::vector<uint64_t> p2d;
  p1d.reserve(p1.size());
  p2d.reserve(p2.size());

  DistanceBinaryFunctor distance;
  std::transform(p1.begin(), p1.end(), std::back_inserter(p1d), [&](const auto& a) { return distance(a, to); });
  std::transform(p2.begin(), p2.end(), std::back_inserter(p2d), [&](const auto& a) { return distance(a, to); });

  std::sort(p1d.begin(), p1d.end());
  std::sort(p2d.begin(), p2d.end());
  EXPECT_EQ(p1d, p2d);
}

template <size_t Dims, typename TestOperation, typename ArgsGenerator>
void kdtreeGenericTest(std::vector<std::array<int, Dims>> points, TestOperation operation, ArgsGenerator generator) {
  using PointType = std::array<int, Dims>;
  using DistanceFunctor = EuclideanDistance<PointType>;
  static constexpr int kNumTrials = 10000;

  kdtree::KDTree<PointType, Dims, DistanceFunctor> kdtree(points);
  NaiveKDTree<PointType, Dims, DistanceFunctor> naive_tree(points);

  PointType q;
  for (int _ = 0; _ < kNumTrials; ++_) {
    test::generatePoint(q);
    std::apply(
        [&](auto... args) {
          verifySameDistances<PointType, DistanceFunctor>(
              q, operation(kdtree, q, args...), operation(kdtree, q, args...));
        },
        generator());
  }
}

template <size_t Dims>
void nnSearchTest(std::vector<std::array<int, Dims>> points) {
  kdtreeGenericTest(
      std::move(points),                                           //
      [](auto& tree, const auto& q) { return tree.nnSearch(q); },  //
      [] { return std::tuple<>{}; });
}

template <size_t Dims>
void knnSearchTest(std::vector<std::array<int, Dims>> points) {
  kdtreeGenericTest(
      std::move(points),  //
      [](auto& tree, const auto& q, size_t max_count) { return tree.knnSearch(q, max_count); },
      [point_count = points.size()] { return std::tuple<size_t>{test::random(size_t{0}, point_count)}; });
}

template <size_t Dims>
void radiusSearchTest(std::vector<std::array<int, Dims>> points) {
  kdtreeGenericTest(
      std::move(points),  //
      [](auto& tree, const auto& q, uint64_t radius) { return tree.radiusSearch(q, radius); },
      [] { return std::tuple<uint64_t>{test::random(0, 1000)}; });
}

template <size_t Dims>
void nnSearchTest(size_t point_count) {
  nnSearchTest(test::generatePoints<Dims>(point_count));
}

template <size_t Dims>
void knnSearchTest(size_t point_count) {
  knnSearchTest(test::generatePoints<Dims>(point_count));
}

template <size_t Dims>
void radiusSearchTest(size_t point_count) {
  radiusSearchTest(test::generatePoints<Dims>(point_count));
}

TEST(KDTreeTest, nnSearchSmall2D) {
  nnSearchTest(std::vector<std::array<int, 2>>{{0, 0}, {1, 0}, {0, 1}, {1, 1}, {-1, 0}, {0, -1}, {-1, -1}});
}

TEST(KDTreeTest, knnSearchSmall2D) {
  knnSearchTest(std::vector<std::array<int, 2>>{{0, 0}, {1, 0}, {0, 1}, {1, 1}, {-1, 0}, {0, -1}, {-1, -1}});
}

TEST(KDTreeTest, radiusSearchSmall2D) {
  radiusSearchTest(std::vector<std::array<int, 2>>{{0, 0}, {1, 0}, {0, 1}, {1, 1}, {-1, 0}, {0, -1}, {-1, -1}});
}

TEST(KDTreeTest, nnSearchLarge2D) { nnSearchTest<2>(1000); }
TEST(KDTreeTest, nnSearchLarge3D) { nnSearchTest<3>(1000); }
TEST(KDTreeTest, nnSearchLarge5D) { nnSearchTest<5>(1000); }

TEST(KDTreeTest, knnSearchLarge2D) { knnSearchTest<2>(1000); }
TEST(KDTreeTest, knnSearchLarge3D) { knnSearchTest<3>(1000); }
TEST(KDTreeTest, knnSearchLarge5D) { knnSearchTest<5>(1000); }

TEST(KDTreeTest, radiusSearchLarge2D) { radiusSearchTest<2>(1000); }
TEST(KDTreeTest, radiusSearchLarge3D) { radiusSearchTest<3>(1000); }
TEST(KDTreeTest, radiusSearchLarge5D) { radiusSearchTest<5>(1000); }

}  // namespace algo::kdtree::unit