#pragma once

#include "algo/utility/visit.h"

#include <array>
#include <cassert>
#include <ranges>
#include <string>

namespace algo::string {

static constexpr auto kAlphSize = 27;  // a-z + {

template <typename SizeType = std::size_t>
struct Edge {
    SizeType p_begin = 0;
    SizeType p_end = 0;

    SizeType size() const {
        return p_end - p_begin;
    }
};

struct NodeTag {};

template <typename SizeType = std::size_t>
struct Pos {
    SizeType node;
    SizeType pos;

    Pos(SizeType v, SizeType pos = 0)
        : node(v), pos(pos) {}

    template <typename... Funcs>
    decltype(auto) visit(Funcs&&... funcs) {
        auto call = utility::overloaded<Funcs...>{std::forward<Funcs>(funcs)...};
        using CallType = decltype(call);

        if (pos == 0) {
            if constexpr (std::is_invocable_v<CallType, NodeTag, SizeType>) {
                return call(NodeTag{}, node);
            }
        } else {
            if constexpr (std::is_invocable_v<CallType, SizeType, SizeType>) {
                return call(node, pos);
            } else if constexpr (std::is_invocable_v<CallType, SizeType>) {
                return call(pos);
            }
        }
    }

    static Pos inNode(SizeType v) {
        return {v};
    }

    static Pos onEdge(SizeType to, SizeType pos) {
        return {to, pos};
    }
};

template <typename SizeType = std::size_t>
class SuffixTree {
protected:
    using Node = SizeType;
    using Edge = Edge<SizeType>;
    using Pos = Pos<SizeType>;

public:
    static constexpr SizeType kNull = 0;

    explicit SuffixTree(const std::string& s)
        : s(s) {
        reserve(s.size());
        buildTree();
    }

    SizeType getRoot() {
        return root;
    }

    SizeType& parent(Node node) {
        return parent_[node];
    }

    SizeType& sufflink(Node node) {
        return sufflink_[node];
    }

    Edge& p_edge(Node node) {
        return p_edge_[node];
    }

    SizeType& edge(Node node, char c) {
        return edges_[node * kAlphSize + (c - 'a')];
    }

    bool isRoot(Node v) {
        return v == root;
    }

protected:
    SuffixTree(const std::string& s, char)
        : s(s) {
        reserve(s.size());
    }

    void buildTree() {
        Pos curr = root = makeNode();
        sufflink(root) = root;

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
            if (isRoot(p.node)) {
                return addLeaf(p, i);
            }

            p = getSuffLink(addLeaf(p, i));
        }
    }

    Node addLeaf(Pos pos, SizeType i) {
        auto m = split(pos);
        attach(m, makeNode(), i, s.size());
        return m;
    }

    Node getSuffLink(Node v) {
        if (sufflink(v) == kNull) [[unlikely]] {
            sufflink(v) = resolveSuffLink(v);
        }
        return sufflink(v);
    }

    Node resolveSuffLink(Node v) {
        Edge& e = p_edge(v);
        Node p = parent(v);

        if (isRoot(p)) {
            return split(transition(p, e.p_begin + 1, e.p_end));
        } else {
            return split(transition(getSuffLink(p), e.p_begin, e.p_end));
        }
    }

    bool hasTransition(Pos& pos, char c) {
        return pos.visit(
            [&](NodeTag, Node v) { return edge(v, c) != kNull; },  //
            [&](SizeType i) { return s[i] == c; });
    }

    Pos transition(Pos pos, char c) {
        assert(hasTransition(pos, c));
        return pos.visit(
            [&](NodeTag, Node v) {
                Node to = edge(v, c);
                Edge& e = p_edge(to);
                return e.size() == 1 ? Pos::inNode(to) : Pos::onEdge(to, e.p_begin + 1);
            },
            [&](Node to, SizeType pos) {
                Edge& e = p_edge(to);
                return pos + 1 == e.p_end ? Pos::inNode(to) : Pos::onEdge(to, pos + 1);
            });
    }

    Pos transition(Pos pos, SizeType begin, SizeType end) {
        while (begin < end) {
            SizeType len = end - begin;

            pos = pos.visit(
                [&](NodeTag, Node v) {
                    char c = s[begin];
                    Node to = edge(v, c);
                    Edge& e = p_edge(to);

                    if (e.size() == 1) {
                        ++begin;
                        return Pos::inNode(to);
                    } else if (len < e.size()) {
                        begin += len;
                        return Pos::onEdge(to, e.p_begin + len);
                    } else {
                        begin += e.size();
                        return Pos::inNode(to);
                    }
                },
                [&](Node to, SizeType i) {
                    Edge& e = p_edge(to);

                    if (i + len < e.size()) {
                        begin += len;
                        return Pos::onEdge(to, i + len);
                    } else {
                        begin += e.size() - i;
                        return Pos::inNode(to);
                    }
                });
        }

        return pos;
    }

    SizeType split(Pos pos) {
        return pos.visit(
            [&](NodeTag, Node v) { return v; },
            [&](Node to, SizeType pos) {
                Node p = parent(to);
                Edge& e = p_edge(to);
                auto u = makeNode();
                attach(p, u, e.p_begin, pos);
                attach(u, to, pos, e.p_end);
                return u;
            });
    }

    void attach(SizeType p, SizeType u, SizeType begin, SizeType end) {
        edge(p, s[begin]) = u;
        parent(u) = p;
        p_edge(u) = Edge{
            .p_begin = begin,
            .p_end = end,
        };
    }

    void reserve(size_t size) {
        size_t max_nodes = 1 + std::max<size_t>(2 * size, 2);
        size_t buf_size = max_nodes * (4 + kAlphSize);
        size_t buf_size_bytes = buf_size * sizeof(SizeType);

        phantom_.reset(static_cast<SizeType*>(::operator new(buf_size_bytes)));
        SizeType* buff = phantom_.get();
        std::memset(buff, kNull, buf_size_bytes);

        edges_ = buff;
        buff += kAlphSize * max_nodes;

        p_edge_ = reinterpret_cast<Edge*>(buff);
        buff += 2 * max_nodes;

        parent_ = buff;
        buff += max_nodes;

        sufflink_ = buff;
    }

    Node makeNode() {
        return size++;
    }

    SizeType root;
    SizeType size = 1;
    std::string_view s;

    SizeType* edges_;
    Edge* p_edge_;
    SizeType* parent_;
    SizeType* sufflink_;

    std::unique_ptr<SizeType> phantom_;
};

}  // namespace algo::string
