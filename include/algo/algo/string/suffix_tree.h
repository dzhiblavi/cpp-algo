#pragma once

#include "algo/utility/flat_alloc.h"
#include "algo/utility/visit.h"

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

    [[nodiscard]] Edge<SizeType>* edge() const noexcept {
        assert(ptr != nullptr);
        return ptr->edge(by_char);
    }
};

template <typename SizeType>
struct Node {
    std::array<Edge<SizeType>*, kAlphSize> edges = {nullptr};
    Node<SizeType>* sufflink;
    Parent<SizeType> parent;

    [[nodiscard]] bool isRoot() noexcept {
        return parent.ptr == nullptr;
    }

    [[nodiscard]] auto*& edge(char c) noexcept {
        return edges[c - 'a'];
    }
};

struct EdgeTag {};
struct NodeTag {};

template <typename SizeType = std::size_t>
struct Pos {
    SizeType pos;
    Node<SizeType>* v;

    Pos(Node<SizeType>* v)
        : pos(0), v(v) {}

    Pos(Edge<SizeType>* e, SizeType pos)
        : pos(pos), v(e->to) {
        assert(e->p_begin < pos && pos < e->p_end);
    }

    template <typename... Funcs>
    decltype(auto) visit(Funcs&&... funcs) {
        auto call = utility::overloaded<Funcs...>{std::forward<Funcs>(funcs)...};
        using CallType = decltype(call);
        using Edge = Edge<SizeType>;
        using Node = Node<SizeType>;

        if (pos == 0) {
            if constexpr (std::is_invocable_v<CallType, Node*>) {
                return call(v);
            } else if constexpr (std::is_invocable_v<CallType, NodeTag>) {
                return call(NodeTag{});
            }
        } else {
            if constexpr (std::is_invocable_v<CallType, Edge*, SizeType>) {
                return call(v->parent.edge(), pos);
            } else if constexpr (std::is_invocable_v<CallType, Edge*>) {
                return call(v->parent.edge());
            } else if constexpr (std::is_invocable_v<CallType, SizeType>) {
                return call(pos);
            } else if constexpr (std::is_invocable_v<CallType, EdgeTag>) {
                return call(EdgeTag{});
            }
        }
    }

    static Pos inNode(Node<SizeType>* v) {
        return {v};
    }

    static Pos onEdge(Edge<SizeType>* e, SizeType pos) {
        return {e, pos};
    }
};

template <
    typename SizeType = std::size_t,  //
    typename NodeAllocator = std::allocator<Node<SizeType>>,
    typename EdgeAllocator = std::allocator<Edge<SizeType>>>
class SuffixTree {
protected:
    using Node = Node<SizeType>;
    using Edge = Edge<SizeType>;
    using Pos = Pos<SizeType>;
    using Parent = Parent<SizeType>;

public:
    explicit SuffixTree(const std::string& s)
        : s(s)
        , node_alloc(std::max<size_t>(2 * this->s.size() - 1, 2))
        , edge_alloc(std::max<size_t>(2 * this->s.size() - 1, 2)) {
        buildTree();
    }

    Node* getRoot() {
        return root;
    }

protected:
    SuffixTree(const std::string& s, char)
        : s(s), node_alloc(std::max<size_t>(2 * this->s.size() - 1, 2)) {}

    void buildTree() {
        Pos curr = root = make_node();

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
                auto* e = v->edge(c);
                return e->size() == 1 ? Pos::inNode(e->to) : Pos::onEdge(e, e->p_begin + 1);
            },
            [&](Edge* e, SizeType i) {
                return i + 1 == e->p_end ? Pos::inNode(e->to) : Pos::onEdge(e, i + 1);
            });
    }

    Node* addLeaf(Pos pos, SizeType i) {
        auto m = split(pos);
        attach(m, make_node(), i, s.size());
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

        auto& [p, c] = v->parent;
        auto* e = p->edge(c);

        if (p->isRoot()) {
            return split(transition(p, e->p_begin + 1, e->p_end));
        } else {
            return split(transition(getSuffLink(p), e->p_begin, e->p_end));
        }
    }

    Pos transition(Pos pos, SizeType begin, SizeType end) {
        while (begin < end) {
            SizeType len = end - begin;

            pos.visit(
                [&, c = s[begin]](Node* v) {
                    auto* e = v->edge(c);

                    if (e->size() == 1) {
                        ++begin;
                        pos.v = e->to;
                    } else if (len < e->size()) {
                        begin += len;
                        pos = Pos::onEdge(e, e->p_begin + len);
                    } else {
                        begin += e->size();
                        pos.v = e->to;
                    }
                },
                [&](Edge* e, SizeType i) {
                    if (i + len < e->size()) {
                        begin += len;
                        pos = Pos::onEdge(e, i + len);
                    } else {
                        begin += e->size() - i;
                        pos = Pos::inNode(e->to);
                    }
                });
        }

        return pos;
    }

    Node* split(Pos pos) {
        return pos.visit(
            [&](Node* v) { return v; },
            [&](Edge* e, SizeType i) {
                auto* p = pos.v->parent.ptr;
                auto u = make_node();
                attach(u, e->to, i, e->p_end);
                attach(p, u, e->p_begin, i);
                return u;
            });
    }

    void attach(Node* p, Node* u, SizeType begin, SizeType end) {
        const char c = s[begin];

        u->parent = Parent{
            .ptr = p,
            .by_char = c,
        };

        auto& edge = p->edge(c);

        if (edge == nullptr) {
            edge = edge_alloc.allocate();
        }

        *edge = Edge{
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
