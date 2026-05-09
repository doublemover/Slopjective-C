#include "support/objc3_property_storage_modifier_support.h"

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
  return is_copy || is_strong || is_retain || is_weak || is_unowned ||
         is_unsafe_unretained;
}

bool HasExplicitRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return HasRuntimeManagedPropertyOwnershipModifier(
             is_copy, is_strong, is_retain, is_weak, is_unowned,
             is_unsafe_unretained) ||
         is_assign;
}

}  // namespace objc3c::support
