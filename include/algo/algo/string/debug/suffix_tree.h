#pragma once

#include "algo/string/suffix_tree.h"

namespace algo::string::debug {

class DebugSuffixTree : public SuffixTree<> {
public:
    explicit DebugSuffixTree(std::string s)
        : SuffixTree(s, 'x') {
        debugBuildTree();
    }

private:
    void traverse(Pos p = {}) {
        traverse_impl(root, p, 0);
        std::cout << std::endl;
    }

    void debugBuildTree() {
        root = make_node();
        Pos curr = root;

        for (size_t i = 0; i < s.size(); ++i) {
            writeln(0, "adding char s[", i, "] = ", s[i]);
            curr = addChar(curr, i);
            traverse(curr);
        }
    }

    template <typename... Args>
    void writeln(int indent, Args... args) {
        std::cout << std::string(indent, ' ');
        ((std::cout << args), ...);
        std::cout << std::endl;
    }

    void traverse_impl(Node* v, Pos cur, int indent) {
        assert(v);
        bool curr = std::holds_alternative<Node*>(cur) && std::get<Node*>(cur) == v;
        if (curr) {
            writeln(indent, "*");
        }

        for (size_t i = 0; i < 27; ++i) {
            auto& e = v->edge('a' + i);
            if (!e.is_valid()) {
                continue;
            }

            bool curr = std::holds_alternative<EdgePos>(cur) && std::get<EdgePos>(cur).edge == &e;

            if (curr) {
                auto p = std::get<EdgePos>(cur).pos;
                assert(e.p_begin < p && p < e.p_end);
                writeln(
                    indent + 1, s.substr(e.p_begin, p - e.p_begin), "*", s.substr(p, e.p_end - p));
            } else {
                writeln(indent + 1, s.substr(e.p_begin, e.size()));
            }

            traverse_impl(e.to, cur, indent + 2);
        }
    }
};

}  // namespace algo::string::debug
