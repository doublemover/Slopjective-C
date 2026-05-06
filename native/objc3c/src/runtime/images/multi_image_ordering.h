#pragma once

#include <cstdint>

namespace objc3c::runtime {

int CompareRuntimeImageRegistrationOrder(
    std::uint64_t lhs_ordinal, const char *lhs_identity,
    std::uint64_t rhs_ordinal, const char *rhs_identity);

}  // namespace objc3c::runtime
