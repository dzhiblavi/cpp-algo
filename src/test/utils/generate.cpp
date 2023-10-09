#include "generate.h"

namespace test {

std::mt19937& generator() {
  static std::mt19937 gen(0);
  return gen;
}

}  // namespace test
