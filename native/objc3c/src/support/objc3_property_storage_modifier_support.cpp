#include "support/objc3_property_storage_modifier_support.h"

#include "support/objc3_property_storage_modifier_flags.h"

namespace objc3c::support {

bool SupportsRuntimeManagedPropertyOwnership(
    bool is_vector,
    bool id_spelling,
    bool class_spelling,
    bool instancetype_spelling,
    bool object_pointer_type_spelling) {
  return !is_vector &&
         (id_spelling || class_spelling || instancetype_spelling ||
          object_pointer_type_spelling);
}

bool HasRuntimeManagedPropertyOwnershipModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_unsafe_unretained) {
  return HasRuntimeManagedPropertyOwnershipModifier(
      MakePropertyStorageModifierFlags(is_copy, is_strong, is_retain, is_weak,
                                       is_unowned, false,
                                       is_unsafe_unretained));
}

bool HasExplicitRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return HasExplicitRuntimeBackedPropertyStorageModifier(
      MakePropertyStorageModifierFlags(is_copy, is_strong, is_retain, is_weak,
                                       is_unowned, is_assign,
                                       is_unsafe_unretained));
}

}  // namespace objc3c::support
