#include "algo/suffix_array/suffix_array.h"

#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <numeric>

namespace test::sfa::unit {

namespace {

struct CyclicStringView {
  CyclicStringView(std::string_view s, size_t k) : k(k), s(s) {}

  size_t k;
  std::string_view s;

  size_t size() const {
    return 1ULL << k;
  }

  char operator[](size_t i) const {
    size_t n = s.length();
    if (i < n) {
      return s[i];
    }
    if (i == n) {
      return 'a' - 1;
    }
    return s[i - n - 1];
  }
};

bool cyclicComparator(const CyclicStringView& s, size_t i, size_t j) {
  for (;; ++i, ++j) {
    if (i >= s.size()) {
      i -= s.size();
    }
    if (j >= s.size()) {
      j -= s.size();
    }
    if (s[i] != s[j]) {
      return s[i] < s[j];
    }
  }
}

std::vector<size_t> buildSuffixArrayNaive(std::string_view src) {
  size_t k = 0;
  while ((1ULL << k) < src.size() + 1) {
    ++k;
  }
  CyclicStringView s(src, k);
  std::vector<size_t> pos(s.size());
  std::iota(pos.begin(), pos.end(), 0);
  std::stable_sort(pos.begin(), pos.end(), std::bind_front(cyclicComparator, s));
  return pos;
}

void testCorrectness(std::string_view s) {
  auto naive = buildSuffixArrayNaive(s);
  auto arr = algo::sfa::buildSuffixArray(s);
  EXPECT_EQ(naive, arr);
}

}  // namespace

TEST(SuffixArrayTest, Correctness) {
  testCorrectness("");
  testCorrectness("a");
  testCorrectness("ab");
  testCorrectness("abab");
  testCorrectness("aaaa");
  testCorrectness("abaaba");
  testCorrectness("abacabadabacaba");
  testCorrectness("abababbababaabab");
  testCorrectness("hwlloeowrld");
  testCorrectness("correctalgorithm");
  testCorrectness("xxxxxxaaaaaaaaxxxxxxxx");
}

}  // namespace test::sfa::unit
