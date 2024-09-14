#pragma once

#include "algo/utility/flat_alloc.h"
#include "algo/utility/visit.h"

#include <array>
#include <cassert>
#include <string>

namespace algo::string {

static constexpr auto kAlphSize = 7;  // a-z + {

template <typename SizeType = std::size_t>
struct Node;

template <typename SizeType = std::size_t>
struct Edge {
    SizeType p_begin;
    SizeType p_end;

    SizeType size() const noexcept {
        return p_end - p_begin;
    }
};

template <typename SizeType>
struct Node {
    std::array<Node*, kAlphSize> edges = {nullptr};
    Node<SizeType>* sufflink;
    Node<SizeType>* parent;
    Edge<SizeType> p_edge;

    bool isRoot() noexcept {
        return parent == nullptr;
    }

    auto*& edge(char c) noexcept {
        return edges[c - 'a'];
    }
};

template <typename SizeType = std::size_t>
struct Pos {
    SizeType pos;
    Node<SizeType>* v;

    Pos(Node<SizeType>* v, SizeType pos = 0)
        : pos(pos), v(v) {}

    template <typename... Funcs>
    decltype(auto) visit(Funcs&&... funcs) {
        auto call = utility::overloaded<Funcs...>{std::forward<Funcs>(funcs)...};
        using CallType = decltype(call);
        using Edge = Edge<SizeType>;
        using Node = Node<SizeType>;

        if (pos == 0) {
            if constexpr (std::is_invocable_v<CallType, Node*>) {
                return call(v);
            }
        } else {
            if constexpr (std::is_invocable_v<CallType, Edge&, Node*, SizeType>) {
                return call(v->p_edge, v, pos);
            } else if constexpr (std::is_invocable_v<CallType, Node*, SizeType>) {
                return call(v, pos);
            } else if constexpr (std::is_invocable_v<CallType, SizeType>) {
                return call(pos);
            }
        }
    }

    static Pos inNode(Node<SizeType>* v) {
        return {v};
    }

    static Pos onEdge(Node<SizeType>* to, SizeType pos) {
        return {to, pos};
    }
};

template <
    typename SizeType = std::size_t,  //
    typename NodeAllocator = std::allocator<Node<SizeType>>>
class SuffixTree {
protected:
    using Node = Node<SizeType>;
    using Edge = Edge<SizeType>;
    using Pos = Pos<SizeType>;

public:
    explicit SuffixTree(const std::string& s)
        : s(s), node_alloc(std::max<size_t>(2 * this->s.size() - 1, 2)) {
        buildTree();
    }

    Node* getRoot() {
        return root;
    }

protected:
    SuffixTree(const std::string& s, char)
        : s(s), node_alloc(std::max<size_t>(2 * this->s.size() - 1, 2)) {}

    void buildTree() {
        Pos curr = root = node_alloc.allocate();

        for (SizeType i = 0; i < static_cast<SizeType>(s.size()); ++i) {
            curr = addChar(curr, i);
        }
    }

    Pos addChar(Pos p, SizeType i) {
        char c = s[i];

        for (;;) {
            if (hasTransition(p, c)) {
                return transition(p, c);
            }
            if (p.v == root) {
                return addLeaf(p, i);
            }

            p = getSuffLink(addLeaf(p, i));
        }
    }

    bool hasTransition(Pos& pos, char c) {
        return pos.visit(
            [&](Node* v) { return v->edge(c) != nullptr; },  //
            [&](SizeType i) { return s[i] == c; });
    }

    Pos transition(Pos pos, char c) {
        assert(hasTransition(pos, c));
        return pos.visit(
            [&](Node* v) {
                auto* to = v->edge(c);
                auto& e = to->p_edge;
                return e.size() == 1 ? Pos::inNode(to) : Pos::onEdge(to, e.p_begin + 1);
            },
            [&](Edge& e, Node* to, SizeType pos) {
                return pos + 1 == e.p_end ? Pos::inNode(to) : Pos::onEdge(to, pos + 1);
            });
    }

    Node* addLeaf(Pos pos, SizeType i) {
        auto m = split(pos);
        attach(m, node_alloc.allocate(), i, s.size());
        return m;
    }

    Node* getSuffLink(Node* v) {
        if (v->sufflink == nullptr) {
            v->sufflink = resolveSuffLink(v);
        }
        return v->sufflink;
    }

    Node* resolveSuffLink(Node* v) {
        if (v->isRoot()) {
            return v;
        }

        Edge& e = v->p_edge;
        Node* p = v->parent;

        if (p->isRoot()) {
            return split(transition(p, e.p_begin + 1, e.p_end));
        } else {
            return split(transition(getSuffLink(p), e.p_begin, e.p_end));
        }
    }

    Pos transition(Pos pos, SizeType begin, SizeType end) {
        while (begin < end) {
            SizeType len = end - begin;

            pos.visit(
                [&](Node* v) {
                    char c = s[begin];
                    Node* u = v->edge(c);
                    Edge& e = u->p_edge;

                    if (e.size() == 1) {
                        ++begin;
                        pos.v = u;
                    } else if (len < e.size()) {
                        begin += len;
                        pos = Pos::onEdge(u, e.p_begin + len);
                    } else {
                        begin += e.size();
                        pos.v = u;
                    }
                },
                [&](Edge& e, Node* to, SizeType i) {
                    if (i + len < e.size()) {
                        begin += len;
                        pos = Pos::onEdge(to, i + len);
                    } else {
                        begin += e.size() - i;
                        pos = Pos::inNode(to);
                    }
                });
        }

        return pos;
    }

    Node* split(Pos pos) {
        return pos.visit(
            [&](Node* v) { return v; },
            [&](Edge& e, Node* to, SizeType pos) {
                Node* p = to->parent;
                Node* u = node_alloc.allocate();
                attach(p, u, e.p_begin, pos);
                attach(u, to, pos, e.p_end);
                return u;
            });
    }

    void attach(Node* p, Node* u, SizeType begin, SizeType end) {
        p->edge(s[begin]) = u;
        u->parent = p;
        u->p_edge = Edge{
            .p_begin = begin,
            .p_end = end,
        };
    }

    Node* root;
    std::string_view s;
    utility::CappedFlatAllocator<Node, NodeAllocator> node_alloc;
};

}  // namespace algo::string
