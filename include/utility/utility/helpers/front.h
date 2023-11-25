#pragma once

#include "utility/helpers/preface.h"

namespace util::hlp {

template <typename From, typename To>
using propagateConst = typename impl::PropagateConst<From, To>::type;

}  // namespace util::hlp