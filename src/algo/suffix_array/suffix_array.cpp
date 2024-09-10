#include "algo/suffix_array/suffix_array.h"

#include <cassert>
#include <ranges>

namespace algo::sfa {

namespace {

size_t closestPower(size_t n) {
  size_t k = 0;
  size_t tmp = 1;

  while (tmp < n) {
    tmp <<= 1;
    ++k;
  }

  return k;
}

inline const char kMinChar = 'a' - 1;
const size_t kCharsCount = 27;

struct CyclicStringView {
  CyclicStringView(std::string_view s, size_t k)
      : k(k), mask((1ULL << k) - 1), size_(1ULL << k), n(s.length()), s(s) {}

  size_t k;
  size_t mask;
  size_t size_;
  size_t n;
  std::string_view s;

  size_t size() const {
    return size_;
  }

  char operator[](size_t i) const {
    assert(i < size_);
    size_t n = s.length();

    if (i < n) {
      return s[i];
    }
    if (i == n) {
      return kMinChar;
    }

    i -= n + 1;
    assert(i < n);
    return s[i];
  }
};

template <typename Range, typename DigitExtractor>
auto digitSort(Range&& r, size_t size, DigitExtractor e, size_t space_size) {
  std::vector<size_t> counters(space_size + 1, 0);
  std::fill(counters.begin(), counters.end(), 0);

  for (auto i : r) {
    ++counters[e(i) + 1];
  }
  for (size_t i = 2; i < counters.size(); ++i) {
    counters[i] += counters[i - 1];
  }

  std::vector<size_t> pos(size);
  for (auto i : r) {
    pos[counters[e(i)]++] = i;
  }

  return pos;
}

template <typename IndexComparator>
auto classes(const std::vector<size_t>& pos, IndexComparator comp) {
  const size_t n = pos.size();
  std::vector<size_t> cx(n);
  size_t cls = 0;
  cx[pos[0]] = cls;

  for (size_t i = 1; i < n; ++i) {
    if (!comp(pos[i], pos[i - 1])) {
      ++cls;
    }
    cx[pos[i]] = cls;
  }

  return std::make_pair(cx, cls);
}

auto sortPairs(std::vector<size_t>& c, size_t max_cls, size_t k) {
  size_t n = c.size();
  size_t half = 1ULL << (k - 1);
  size_t num_classes = max_cls + 1;
  size_t mask = n - 1;

  auto pos1 = digitSort(
      std::views::iota(0ULL, n),
      n,
      [&c, half, mask](size_t i) { return c[(i + half) & mask]; },
      num_classes);

  auto pos = digitSort(pos1, n, [&c](size_t i) { return c[i]; }, num_classes);

  auto [cx, mcls] = classes(pos, [&c, half, mask](size_t i, size_t j) {
    return c[i] == c[j] && c[(i + half) & mask] == c[(j + half) & mask];
  });

  return std::make_tuple(pos, cx, mcls);
}

}  // namespace

std::vector<size_t> buildSuffixArray(std::string_view ss) {
  const size_t n = ss.length();
  const size_t k = closestPower(n + 1);
  auto s = CyclicStringView(ss, k);

  auto pos = digitSort(                            //
      std::views::iota(0ULL, s.size()),            //
      s.size(),                                    //
      [&s](size_t i) { return s[i] - kMinChar; },  //
      kCharsCount);

  auto [cx, mc] = classes(pos, [&s](size_t i, size_t j) { return s[i] == s[j]; });
  size_t mcc = mc;

  for (size_t i = 1; i <= k; ++i) {
    auto [pn, cn, mc] = sortPairs(cx, mcc, i);
    mcc = mc;
    pos = std::move(pn);
    cx = std::move(cn);
  }

  return pos;
}

}  // namespace algo::sfa
