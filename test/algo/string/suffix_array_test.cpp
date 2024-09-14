#include "algo/string/suffix_array.h"

#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <numeric>

namespace test::string::unit {

namespace {

struct CyclicStringView {
    CyclicStringView(std::string_view s, size_t k)
        : k(k), s(s) {}

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

struct SuffixTreeTest : testing::TestWithParam<std::string> {};

const std::vector<std::string> smallTestCases{
    "",
    "a",
    "ab",
    "abab",
    "aaaa",
    "abaaba",
    "abacabadabacaba",
    "abababbababaabab",
    "hwlloeowrld",
    "correctalgorithm",
    "xxxxxxaaaaaaaaxxxxxxxx",
};

}  // namespace

TEST_P(SuffixTreeTest, Correctness) {
    const auto& s = GetParam();
    auto naive = buildSuffixArrayNaive(s);
    auto arr = algo::string::buildSuffixArray(s);
    EXPECT_EQ(naive, arr);
}

INSTANTIATE_TEST_SUITE_P(Small, SuffixTreeTest, testing::ValuesIn(smallTestCases));

}  // namespace test::string::unit
