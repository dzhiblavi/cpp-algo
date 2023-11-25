#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

namespace algo::kdtree {

namespace detail {

template <typename T, typename Compare = std::less<T>>
class BoundedPriorityQueue {
 public:
  explicit BoundedPriorityQueue(size_t max_size) noexcept : max_size_{max_size} { container_.reserve(max_size + 1); }

  template <typename... Args>
  void emplace(Args&&... args) {
    container_.emplace_back(std::forward<Args>(args)...);
    std::push_heap(container_.begin(), container_.end(), comp_);

    if (container_.size() > max_size_) {
      std::pop_heap(container_.begin(), container_.end(), comp_);
      container_.pop_back();
    }
  }

  std::vector<T> collect() && { return std::move(container_); }
  const T& top() { return container_.front(); }
  [[nodiscard]] size_t size() const noexcept { return container_.size(); }
  [[nodiscard]] size_t maxSize() const noexcept { return max_size_; }

 private:
  const size_t max_size_;
  [[no_unique_address]] Compare comp_;
  std::vector<T> container_;
};

}  // namespace detail

template <typename PointType>
struct EuclideanDistance {
  using DistanceType = uint64_t;
  static constexpr DistanceType kMaxValue = std::numeric_limits<DistanceType>::max();

  uint64_t operator()(const PointType& a, const PointType& b) const {
    return std::transform_reduce(
        a.begin(), a.end(), b.begin(), uint64_t{0}, std::plus<uint64_t>{},
        [](auto x, auto y) -> uint64_t { return (x - y) * (x - y); });
  }

  uint64_t axisDistance(const PointType& a, const PointType& b, size_t dim) const {
    const uint64_t dist = a[dim] <= b[dim] ? b[dim] - a[dim] : a[dim] - b[dim];
    return dist * dist;
  }
};

template <typename PointType, size_t Dims, typename DistanceBinaryFunctor = EuclideanDistance<PointType>>
class KDTree {
  struct Node {
    PointType point;
    std::array<std::unique_ptr<Node>, 2> children{};
  };

  using DistanceType = typename DistanceBinaryFunctor::DistanceType;
  using KNNSearchQueue = detail::BoundedPriorityQueue<std::pair<DistanceType, const Node*>>;

 public:
  using DistanceFunctor = DistanceBinaryFunctor;

  explicit KDTree(std::vector<PointType>& points)  //
      : root_{buildTree(points.begin(), points.end(), 0)} {}

  const PointType& nnSearch(const PointType& q) const {
    assert(root_ && !!"kdtree is empty");
    DistanceType dist = DistanceBinaryFunctor::kMaxValue;
    const PointType* point = nullptr;

    nnSearchRecursive(root_.get(), 0, q, dist, point);
    return *point;
  }

  std::vector<PointType> knnSearch(const PointType& q, size_t max_count) const {
    if (max_count == 0) {
      return {};
    }
    assert(root_ && !!"kdtree is empty");

    KNNSearchQueue queue(max_count);
    knnSearchRecursive(root_.get(), 0, q, queue);

    std::vector<PointType> result;
    result.reserve(queue.size());

    for (auto [_, node] : std::move(queue).collect()) {
      result.push_back(node->point);
    }

    return result;
  }

  std::vector<PointType> radiusSearch(const PointType& q, uint64_t radius) const {
    if (!root_) {
      return {};
    }

    radius *= radius;
    std::vector<PointType> result;
    radiusSearchRecursive(root_.get(), 0, q, radius, result);
    return result;
  }

 private:
  static constexpr size_t kLeftChild = 0;
  static constexpr size_t kRightChild = 1;

  void nnSearchRecursive(
      const Node* v, size_t dim, const PointType& q, DistanceType& min_dist, const PointType*& point) const {
    if (v == nullptr) {
      return;
    }

    const auto& curr = v->point;
    const auto distance = distance_(q, curr);

    if (distance < min_dist) {
      min_dist = distance;
      point = &curr;
    }

    const size_t direction = closestDirection(q, curr, dim);
    const size_t next_dim = nextDimension(dim);
    nnSearchRecursive(v->children[direction].get(), next_dim, q, min_dist, point);

    if (distance_.axisDistance(q, curr, dim) < min_dist) {
      nnSearchRecursive(v->children[1 - direction].get(), next_dim, q, min_dist, point);
    }
  }

  void knnSearchRecursive(const Node* v, size_t dim, const PointType& q, KNNSearchQueue& queue) const {
    if (v == nullptr) {
      return;
    }

    const auto& curr = v->point;
    const auto distance = distance_(q, curr);
    queue.emplace(distance, v);

    const size_t direction = closestDirection(q, curr, dim);
    const size_t next_dim = nextDimension(dim);
    knnSearchRecursive(v->children[direction].get(), next_dim, q, queue);

    if (queue.size() < queue.maxSize() || distance_.axisDistance(q, curr, dim) < queue.top().first) {
      knnSearchRecursive(v->children[1 - direction].get(), next_dim, q, queue);
    }
  }

  void radiusSearchRecursive(
      const Node* v, size_t dim, const PointType& q, uint64_t radius, std::vector<PointType>& output) const {
    if (!v) {
      return;
    }

    const auto& curr = v->point;
    const auto distance = distance_(q, curr);

    if (distance < radius) {
      output.push_back(curr);
    }

    const size_t direction = closestDirection(q, curr, dim);
    const size_t next_dim = nextDimension(dim);
    radiusSearchRecursive(v->children[direction].get(), next_dim, q, radius, output);

    if (distance_.axisDistance(q, curr, dim) < radius) {
      radiusSearchRecursive(v->children[1 - direction].get(), next_dim, q, radius, output);
    }
  }

  template <std::random_access_iterator It>
  static std::unique_ptr<Node> buildTree(It begin, It end, size_t dim) {
    const size_t size = std::distance(begin, end);

    if (size == 0) {
      return {};
    }
    if (size == 1) {
      return std::unique_ptr<Node>(new Node{.point = *begin});
    }

    auto it = begin + size / 2;
    std::nth_element(begin, it, end, [dim](const auto& p1, const auto& p2) { return p1[dim] < p2[dim]; });

    const size_t next_dim = nextDimension(dim);
    return std::unique_ptr<Node>(new Node{
        .point = *it,
        .children =
            {
                buildTree(begin, it, next_dim),
                buildTree(it + 1, end, next_dim),
            },
    });
  }

  static size_t nextDimension(size_t dim) noexcept { return dim == Dims - 1 ? 0 : dim + 1; }

  static size_t closestDirection(const PointType& query, const PointType& current, size_t dim) noexcept {
    return query[dim] <= current[dim] ? kLeftChild : kRightChild;
  }

  [[no_unique_address]] DistanceBinaryFunctor distance_;
  std::unique_ptr<Node> root_{nullptr};
};

}  // namespace algo::kdtree