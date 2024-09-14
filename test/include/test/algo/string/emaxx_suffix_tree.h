#pragma once

#include <string>

namespace test::string {

// http://e-maxx.ru/algo/ukkonen
class ReferenceSuffixTree {
public:
    explicit ReferenceSuffixTree(const std::string& s)
        : N(2 * s.size()), a(s) {
        t.resize(N);
        l.resize(N);
        r.resize(N);
        p.resize(N);
        this->s.resize(N);
        build();
    }

private:
    void ukkadd(int c) {
    suff:;
        if (r[tv] < tp) {
            if (t[tv][c] == -1) {
                t[tv][c] = ts;
                l[ts] = la;
                p[ts++] = tv;
                tv = s[tv];
                tp = r[tv] + 1;
                goto suff;
            }
            tv = t[tv][c];
            tp = l[tv];
        }
        if (tp == -1 || c == a[tp] - 'a')
            tp++;
        else {
            l[ts + 1] = la;
            p[ts + 1] = ts;
            l[ts] = l[tv];
            r[ts] = tp - 1;
            p[ts] = p[tv];
            t[ts][c] = ts + 1;
            t[ts][a[tp] - 'a'] = tv;
            l[tv] = tp;
            p[tv] = ts;
            t[p[ts]][a[l[ts]] - 'a'] = ts;
            ts += 2;
            tv = s[p[ts - 2]];
            tp = l[ts - 2];
            while (tp <= r[ts - 2]) {
                tv = t[tv][a[tp] - 'a'];
                tp += r[tv] - l[tv] + 1;
            }
            if (tp == r[ts - 2] + 1)
                s[ts - 2] = tv;
            else
                s[ts - 2] = ts;
            tp = r[tv] - (tp - r[ts - 2]) + 2;
            goto suff;
        }
    }

    void build() {
        ts = 2;
        tv = 0;
        tp = 0;
        std::fill(r.begin(), r.end(), (int)a.size() - 1);
        s[0] = 1;
        l[0] = -1;
        r[0] = -1;
        l[1] = -1;
        r[1] = -1;

        for (auto& a : t) {
            a.fill(-1);
        }

        t[1].fill(0);

        for (la = 0; la < (int)a.size(); ++la)
            ukkadd(a[la] - 'a');
    }

    size_t N;
    std::string_view a;
    std::vector<std::array<int, 26>> t;
    std::vector<int> l, r, p, s;
    int tv, tp, ts, la;
};

}  // namespace test::string
