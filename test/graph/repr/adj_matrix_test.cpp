#include "graph/repr/adj_matrix/adj_matrix.h"

#include <gtest/gtest.h>

namespace algo::graph::repr::unit {

struct WeightPayload {
  explicit WeightPayload(int l) noexcept : length(l) {}
  int length;
};

TEST(AdjMatrixGraphTest, Basic) {
  //
}

}  // namespace algo::graph::repr::unit