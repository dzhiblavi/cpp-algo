#include <concepts>
#include <vector>

namespace algo::dsu {

struct TreeSizeRankStrategy {
  static size_t getNewRank(size_t rank_u, size_t rank_v) noexcept {
    return rank_u + rank_v;
  }
};

struct TreeDepthRankStrategy {
  static size_t getNewRank(size_t rank_u, size_t rank_v) noexcept {
    if (rank_u == rank_v) {
      return rank_u + 1;
    }

    // rank_u is guaranteed to be greater or equal than rank_v
    return rank_u;
  }
};

template <std::integral IdType = int, typename RankStrategy = TreeSizeRankStrategy>
class DisjointSetUnion {
 public:
  static_assert(sizeof(IdType) <= sizeof(size_t), "Integral ids larger than size_t are not allowed");

  explicit DisjointSetUnion(size_t capacity_hint = 0) {
    if (capacity_hint > 0) {
      sets_.reserve(capacity_hint);
    }
  }

  IdType makeSet() {
    const IdType result{sets_.size()};

    sets_.push_back(SetInfo{
        .parent = result,
        .rank = 1,
    });

    return result;
  }

  IdType getRepresentative(IdType id) noexcept {
    const auto& set = sets_[id];

    if (id != set.parent) {
      set.parent = getRepresentative(set.parent);
    }

    return set.parent;
  }

  bool unionSets(IdType u, IdType v) noexcept {
    u = getRepresentative(u);
    v = getRepresentative(v);

    if (u == v) {
      return false;
    }
    if (sets_[u].rank < sets_[v].rank) {
      std::swap(u, v);
    }

    sets_[v].parent = u;
    sets_[u].rank = RankStrategy::getNewRank(sets_[u].rank, sets_[v].rank);

    return true;
  }

 private:
  struct SetInfo {
    IdType parent;
    size_t rank;
  };

  std::vector<SetInfo> sets_;
};

}  // namespace algo::dsu