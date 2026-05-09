#include "support/objc3_property_storage_profile_helpers.h"

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

std::string BuildRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  if (is_copy) {
    return "copy";
  }
  if (is_retain) {
    return "retain";
  }
  if (is_strong) {
    return "strong";
  }
  if (is_weak) {
    return "weak";
  }
  if (is_unowned) {
    return "unowned";
  }
  if (is_unsafe_unretained) {
    return "unsafe_unretained";
  }
  if (is_assign) {
    return "assign";
  }
  return {};
}

}  // namespace objc3c::support
