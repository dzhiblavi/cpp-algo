#include "algo/utility/soa_storage.h"

#include <gtest/gtest.h>

namespace algo::utility {

SOA(NodeSOA, (int, x), (float, y), (Descriptor, self));

TEST(A, B) {
    NodeSOA<int> s;

    auto x = s.create();

    x.x() = 1;
    x.y() = 2.f;

    EXPECT_FALSE(x.isNull());
    EXPECT_EQ(x.x(), 1);
    EXPECT_FLOAT_EQ(x.y(), 2.f);
}

}  // namespace algo::utility
