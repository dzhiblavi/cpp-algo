#pragma once

#include "algo/utility/visit.h"

#include <array>
#include <cassert>
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

template <typename SizeType = std::size_t>
struct NodeSOAStorage {
    using IndexType = SizeType;
    static constexpr IndexType kNull = -1;
    friend struct Descriptor;

    struct Descriptor {
        friend struct NodeSOAStorage<SizeType>;

        Descriptor() = default;
        Descriptor(const Descriptor&) = default;
        Descriptor& operator=(const Descriptor&) = default;

        Descriptor parent() {
            return {soa, soa->parent_[idx]};
        }

        void set_parent(const Descriptor& node) {
            soa->parent_[idx] = node.idx;
        }

        Descriptor sufflink() {
            return {soa, soa->sufflink_[idx]};
        }

        void set_sufflink(const Descriptor& node) {
            soa->sufflink_[idx] = node.idx;
        }

        Descriptor edge(char c) {
            return {soa, soa->edges_[idx][c - 'a']};
        }

        void set_edge(char c, const Descriptor& node) {
            soa->edges_[idx][c - 'a'] = node.idx;
        }

        Edge<SizeType>& p_edge_ref() {
            return soa->p_edge_[idx];
        }

        bool isRoot() {
            return parent().isNone();
        }

        bool isNone() {
            return idx == kNull;
        }

    private:
        Descriptor(NodeSOAStorage* soa, std::convertible_to<IndexType> auto idx)
            : soa(soa), idx(static_cast<IndexType>(idx)) {}

        NodeSOAStorage* soa = nullptr;
        IndexType idx = kNull;
    };

    explicit NodeSOAStorage(size_t capacity) {
        parent_.reserve(capacity);
        sufflink_.reserve(capacity);
        p_edge_.reserve(capacity);
        edges_.reserve(capacity);
    }

    Descriptor create_node() {
        size_t idx = parent_.size();
        parent_.push_back(-1);
        sufflink_.push_back(-1);
        p_edge_.emplace_back();
        edges_.emplace_back();
        edges_.back().fill(-1);
        return {this, idx};
    }

private:
    std::vector<int> parent_;
    std::vector<int> sufflink_;
    std::vector<Edge<SizeType>> p_edge_;
    std::vector<std::array<IndexType, kAlphSize>> edges_;
};

template <typename SizeType = std::size_t>
using Node = typename NodeSOAStorage<SizeType>::Descriptor;

struct NodeTag {};

template <typename SizeType = std::size_t>
struct Pos {
    Node<SizeType> node;
    SizeType pos;

    Pos(Node<SizeType> v, SizeType pos = 0)
        : node(v), pos(pos) {}

    template <typename... Funcs>
    decltype(auto) visit(Funcs&&... funcs) {
        auto call = utility::overloaded<Funcs...>{std::forward<Funcs>(funcs)...};
        using CallType = decltype(call);
        using Node = Node<SizeType>;

        if (pos == 0) {
            if constexpr (std::is_invocable_v<CallType, Node&>) {
                return call(node);
            }
        } else {
            if constexpr (std::is_invocable_v<CallType, Node&, SizeType>) {
                return call(node, pos);
            } else if constexpr (std::is_invocable_v<CallType, SizeType>) {
                return call(pos);
            }
        }
    }

    static Pos inNode(Node<SizeType> v) {
        return {v};
    }

    static Pos onEdge(Node<SizeType> to, SizeType pos) {
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
        : s(s), node_storage(std::max<size_t>(2 * this->s.size() - 1, 2)) {
        buildTree();
    }

    Node getRoot() {
        return root;
    }

protected:
    SuffixTree(const std::string& s, char)
        : s(s), node_storage(std::max<size_t>(2 * this->s.size() - 1, 2)) {}

    void buildTree() {
        Pos curr = root = node_storage.create_node();

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
            if (p.node.isRoot()) {
                return addLeaf(p, i);
            }

            p = getSuffLink(addLeaf(p, i));
        }
    }

    bool hasTransition(Pos& pos, char c) {
        return pos.visit(
            [&](Node& v) { return !v.edge(c).isNone(); },  //
            [&](SizeType i) { return s[i] == c; });
    }

    Pos transition(Pos pos, char c) {
        assert(hasTransition(pos, c));
        return pos.visit(
            [&](Node& v) {
                Node to = v.edge(c);
                Edge& e = to.p_edge_ref();
                return e.size() == 1 ? Pos::inNode(to) : Pos::onEdge(to, e.p_begin + 1);
            },
            [&](Node& to, SizeType pos) {
                Edge& e = to.p_edge_ref();
                return pos + 1 == e.p_end ? Pos::inNode(to) : Pos::onEdge(to, pos + 1);
            });
    }

    Node addLeaf(Pos pos, SizeType i) {
        auto m = split(pos);
        attach(m, node_storage.create_node(), i, s.size());
        return m;
    }

    Node getSuffLink(Node v) {
        if (v.sufflink().isNone()) {
            v.set_sufflink(resolveSuffLink(v));
        }
        return v.sufflink();
    }

    Node resolveSuffLink(Node v) {
        if (v.isRoot()) {
            return v;
        }

        Edge& e = v.p_edge_ref();
        Node p = v.parent();

        if (p.isRoot()) {
            return split(transition(p, e.p_begin + 1, e.p_end));
        } else {
            return split(transition(getSuffLink(p), e.p_begin, e.p_end));
        }
    }

    Pos transition(Pos pos, SizeType begin, SizeType end) {
        while (begin < end) {
            SizeType len = end - begin;

            pos = pos.visit(
                [&](Node& v) {
                    char c = s[begin];
                    Node to = v.edge(c);
                    Edge& e = to.p_edge_ref();

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
                [&](Node& to, SizeType i) {
                    Edge& e = to.p_edge_ref();

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

    Node split(Pos pos) {
        return pos.visit(
            [&](Node& v) { return v; },
            [&](Node& to, SizeType pos) {
                Node p = to.parent();
                Edge& e = to.p_edge_ref();
                auto u = node_storage.create_node();
                attach(p, u, e.p_begin, pos);
                attach(u, to, pos, e.p_end);
                return u;
            });
    }

    void attach(Node& p, Node u, SizeType begin, SizeType end) {
        p.set_edge(s[begin], u);
        u.set_parent(p);
        u.p_edge_ref() = Edge{
            .p_begin = begin,
            .p_end = end,
        };
    }

    Node root;
    std::string_view s;
    NodeSOAStorage<SizeType> node_storage;
};

}  // namespace algo::string
