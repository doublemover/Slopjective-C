#pragma once

#include <cstddef>

namespace objc3c::support {

std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);

}  // namespace objc3c::support
