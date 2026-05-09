#include "support/objc3_property_storage_modifier_counts.h"

namespace objc3c::support {

std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return (is_copy ? 1u : 0u) +
         ((is_strong || is_retain) ? 1u : 0u) + (is_weak ? 1u : 0u) +
         (is_unowned ? 1u : 0u) +
         ((is_assign || is_unsafe_unretained) ? 1u : 0u);
}

}  // namespace objc3c::support
