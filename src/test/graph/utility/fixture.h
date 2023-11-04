#pragma once

#include "algo/graph/repr/concepts.h"
#include "test/graph/utility/graph_types.h"

#include <gtest/gtest.h>

namespace algo::graph::util {

template <
    GraphKind::UnderlyingType Kind,     //
    DirectKind::UnderlyingType Direct,  //
    std::size_t StorageReserve>
using GTestGraphTypes = list::apply<::testing::Types, GraphTypes<Kind, Direct, StorageReserve>>;

template <typename Graph>
class BaseGraphTest : public ::testing::Test {};

}  // namespace algo::graph::util