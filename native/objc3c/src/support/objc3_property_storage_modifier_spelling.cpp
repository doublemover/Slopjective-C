#include "support/objc3_property_storage_modifier_spelling.h"

#include "support/objc3_property_storage_modifier_flags.h"

namespace objc3c::support {

std::string BuildRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return BuildRuntimeBackedPropertyStorageModifier(
      MakePropertyStorageModifierFlags(is_copy, is_strong, is_retain, is_weak,
                                       is_unowned, is_assign,
                                       is_unsafe_unretained));
}

}  // namespace objc3c::support
