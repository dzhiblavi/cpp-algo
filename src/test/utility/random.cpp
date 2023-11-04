#include "test/utility/random.h"

namespace test::utility::random {

std::mt19937& generator() {
  static std::mt19937 gen(0);
  return gen;
}

}  // namespace test::utility::random
