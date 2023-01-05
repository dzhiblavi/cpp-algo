#include "src/algo/treap/node.h"

namespace algo::treap {

std::mt19937& DefaultReproducibleGenerator() {
  static std::mt19937 gen(0);
  return gen;
}

}  // namespace algo::treap

