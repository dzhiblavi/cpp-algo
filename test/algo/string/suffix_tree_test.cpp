#include "algo/string/suffix_tree.h"

#include "test/algo/string/emaxx_suffix_tree.h"

#include <gtest/gtest.h>

namespace test::string::unit {

using Node = algo::string::Node<>;
using SuffixTree = algo::string::SuffixTree<>;

namespace {

void collectEdgesImpl(Node* node, const std::string& s, std::vector<std::string>& out) {
    for (size_t i = 0; i < node->edges.size(); ++i) {
        auto* e = node->edges[i];
        if (e == nullptr) {
            continue;
        }
        out.emplace_back(s.substr(e->p_begin, e->size()));
        collectEdgesImpl(e->to, s, out);
        out.emplace_back("-");
    }
}

std::vector<std::string> collectEdges(const std::string& s, SuffixTree& tree) {
    std::vector<std::string> edges;
    collectEdgesImpl(tree.getRoot(), s, edges);
    return edges;
}

struct TestCase {
    std::string s;
    std::vector<std::string> expected_edges;
};

struct SuffixTreeTest : testing::TestWithParam<TestCase> {};

const std::vector<TestCase> smallTestCases{
    {
        .s = "{",
        .expected_edges = {"{", "-"},
    },
    {
        .s = "a{",
        .expected_edges = {"a{", "-", "{", "-"},
    },
    {
        .s = "aa{",
        .expected_edges = {"a", "a{", "-", "{", "-", "-", "{", "-"},
    },
    {
        .s = "ab{",
        .expected_edges = {"ab{", "-", "b{", "-", "{", "-"},
    },
    {
        .s = "ababba{",
        .expected_edges =
            {
                "a", "b",    "abba{", "-", "ba{", "-", "-",   "{", "-", "-", "b",
                "a", "bba{", "-",     "{", "-",   "-", "ba{", "-", "-", "{", "-",
            },
    },
    {
        .s = "abacaba{",
        .expected_edges =
            {
                "a", "ba", "caba{", "-", "{", "-", "-", "caba{", "-", "{", "-",
                "-", "ba", "caba{", "-", "{", "-", "-", "caba{", "-", "{", "-",
            },
    },
    {
        .s = "aaaaaaaa{",
        .expected_edges =
            {
                "a", "a", "a", "a", "a", "a", "a", "a{", "-", "{", "-", "-", "{", "-", "-", "{",
                "-", "-", "{", "-", "-", "{", "-", "-",  "{", "-", "-", "{", "-", "-", "{", "-",
            },
    },
    {
        .s = "aaaabbbb{",
        .expected_edges =
            {
                "a", "a",     "a", "abbbb{", "-", "bbbb{", "-", "-",  "bbbb{", "-",
                "-", "bbbb{", "-", "-",      "b", "b",     "b", "b{", "-",     "{",
                "-", "-",     "{", "-",      "-", "{",     "-", "-",  "{",     "-",
            },
    },
};

}  // namespace

TEST_P(SuffixTreeTest, Correctness) {
    auto& [s, expected_edges] = GetParam();
    auto tree = SuffixTree(s);
    auto edges = collectEdges(s, tree);
    EXPECT_EQ(edges, expected_edges);
}

INSTANTIATE_TEST_SUITE_P(Small, SuffixTreeTest, testing::ValuesIn(smallTestCases));

}  // namespace test::string::unit
