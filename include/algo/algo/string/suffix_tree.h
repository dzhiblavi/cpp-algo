#pragma once

#include "algo/utility/flat_alloc.h"
#include "algo/utility/match.h"

#include <array>
#include <cassert>
#include <string>

namespace algo::string {

static constexpr auto kAlphSize = 27;  // a-z + {

template <typename SizeType = std::size_t>
struct Node;

template <typename SizeType = std::size_t>
struct Edge {
    SizeType p_begin;
    SizeType p_end;
    Node<SizeType>* to;

    [[nodiscard]] SizeType size() const noexcept {
        return p_end - p_begin;
    }
};

template <typename SizeType = std::size_t>
struct Parent {
    Node<SizeType>* ptr;
    char by_char;

    [[nodiscard]] bool is_valid() const noexcept {
        return ptr != nullptr;
    }

    [[nodiscard]] bool edge() const noexcept {
        assert(is_valid());
        return ptr->edge(by_char);
    }
};

template <typename SizeType>
struct Node {
    std::array<Edge<SizeType>*, kAlphSize> edges = {nullptr};
    Node<SizeType>* sufflink;
    Parent<SizeType> parent;

    [[nodiscard]] auto*& edge(char c) noexcept {
        return edges[c - 'a'];
    }
};

template <typename SizeType = std::size_t>
struct EdgePos {
    Edge<SizeType>* edge;
    SizeType pos;
};

template <typename SizeType = std::size_t>
using Pos = std::variant<Node<SizeType>*, EdgePos<SizeType>>;

template <
    typename SizeType = std::size_t,  //
    typename NodeAllocator = std::allocator<Node<SizeType>>,
    typename EdgeAllocator = std::allocator<Edge<SizeType>>>
class SuffixTree {
protected:
    using Node = Node<SizeType>;
    using Edge = Edge<SizeType>;
    using EdgePos = EdgePos<SizeType>;
    using Pos = Pos<SizeType>;
    using Parent = Parent<SizeType>;

public:
    explicit SuffixTree(const std::string& s)
        : s(s)
        , node_alloc(std::max(2 * this->s.size() - 1, size_t(2)))
        , edge_alloc(std::max(2 * this->s.size() - 1, size_t(2))) {
        buildTree();
    }

    Node* getRoot() {
        return root;
    }

protected:
    SuffixTree(const std::string& s, char)
        : s(s), node_alloc(std::max(2 * this->s.size() - 1, size_t(2))) {}

    void buildTree() {
        Pos curr = root = make_node();

        for (size_t i = 0; i < s.size(); ++i) {
            curr = addChar(curr, i);
        }
    }

    Pos addChar(Pos p, size_t i) {
        char c = s[i];

        for (;;) {
            bool has_transition = hasTransition(p, c);

            if (isRoot(p)) {
                return has_transition ? transition(p, c) : addLeaf(p, i);
            }

            if (has_transition) {
                return transition(p, c);
            }

            p = getSuffLink(addLeaf(p, i));
        }
    }

    bool isRoot(Pos p) {
        return std::holds_alternative<Node*>(p) && std::get<Node*>(p) == root;
    }

    bool hasTransition(Pos& p, char c) {
        return match(p){
            [c](Node* v) { return v->edge(c) != nullptr; },
            [c, this](EdgePos& ep) {
                auto& [e, p] = ep;
                assert(e->p_begin < p && p < e->p_end);
                return s[p] == c;
            },
        };
    }

    Pos transition(Pos p, char c) {
        assert(hasTransition(p, c));

        return match(p){
            [c](Node* v) -> Pos {
                auto* e = v->edge(c);

                if (e->size() == 1) {
                    return e->to;
                }

                return EdgePos{
                    .edge = e,
                    .pos = e->p_begin + 1,
                };
            },
            [](EdgePos ep) -> Pos {
                auto& [e, i] = ep;
                ++i;

                if (i == e->p_end) {
                    return e->to;
                }

                return ep;
            },
        };
    }

    Node* addLeaf(Pos& p, size_t i) {
        return match(p){
            [i, this](Node* v) {
                attach(v, make_node(), i, s.size());
                return v;
            },
            [i, this](EdgePos ep) {
                auto m = split(ep);
                attach(m, make_node(), i, s.size());
                return m;
            },
        };
    }

    Node* getSuffLink(Node* v) {
        if (v->sufflink == nullptr) {
            auto pos = resolveSuffLink(v);

            v->sufflink = match(pos){
                [](Node* v) { return v; },
                [this](EdgePos ep) -> Node* { return split(ep); },
            };
        }
        return v->sufflink;
    }

    Pos resolveSuffLink(Node* v) {
        if (!v->parent.is_valid()) {
            return v;
        }

        auto [p, c] = v->parent;
        auto& e = p->edge(c);

        if (!p->parent.is_valid()) {
            return transition(p, e->p_begin + 1, e->p_end);
        } else {
            return transition(getSuffLink(p), e->p_begin, e->p_end);
        }
    }

    Pos transition(Pos p, size_t begin, size_t end) {
        while (begin < end) {
            size_t len = end - begin;
            char c = s[begin];

            p = match(p){
                [&](Node* v) -> Pos {
                    auto* e = v->edge(c);
                    assert(e);

                    if (e->size() == 1) {
                        ++begin;
                        return e->to;
                    }

                    if (len < e->size()) {
                        begin += len;
                        return EdgePos{
                            .edge = e,
                            .pos = e->p_begin + len,
                        };
                    }

                    begin += e->size();
                    return e->to;
                },
                [&](EdgePos ep) -> Pos {
                    auto& [e, i] = ep;

                    if (i + len < e->size()) {
                        i += len;
                        begin += len;
                        return ep;
                    }

                    begin += e->size() - i;
                    return e->to;
                },
            };
        }

        return p;
    }

    Node* split(EdgePos& ep) {
        auto& [e, i] = ep;
        auto u = make_node();
        auto p = e->to->parent.ptr;
        attach(u, e->to, i, e->p_end);
        attach(p, u, e->p_begin, i);
        return u;
    }

    void attach(Node* p, Node* u, size_t begin, size_t end) {
        const char c = s[begin];

        u->parent = Parent{
            .ptr = p,
            .by_char = c,
        };

        if (p->edge(c) == nullptr) {
            p->edge(c) = edge_alloc.allocate();
        }

        *p->edge(c) = Edge{
            .p_begin = begin,
            .p_end = end,
            .to = u,
        };
    }

    Node* make_node() {
        return node_alloc.allocate();
    }

    Edge* make_edge() {
        return edge_alloc.allocate();
    }

    Node* root;
    std::string_view s;
    utility::CappedFlatAllocator<Node, NodeAllocator> node_alloc;
    utility::CappedFlatAllocator<Edge, EdgeAllocator> edge_alloc;
};

}  // namespace algo::string
