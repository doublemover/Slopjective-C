#include "support/objc3_property_storage_modifier_flags.h"

namespace objc3c::support {

Objc3PropertyStorageModifierFlags MakePropertyStorageModifierFlags(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained) {
  return {
      is_copy,
      is_strong,
      is_retain,
      is_weak,
      is_unowned,
      is_assign,
      is_unsafe_unretained,
  };
}

bool HasRuntimeManagedPropertyOwnershipModifier(
    const Objc3PropertyStorageModifierFlags &flags) {
  return flags.is_copy || flags.is_strong || flags.is_retain || flags.is_weak ||
         flags.is_unowned || flags.is_unsafe_unretained;
}

bool HasExplicitRuntimeBackedPropertyStorageModifier(
    const Objc3PropertyStorageModifierFlags &flags) {
  return HasRuntimeManagedPropertyOwnershipModifier(flags) || flags.is_assign;
}

std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    const Objc3PropertyStorageModifierFlags &flags) {
  return (flags.is_copy ? 1u : 0u) +
         ((flags.is_strong || flags.is_retain) ? 1u : 0u) +
         (flags.is_weak ? 1u : 0u) + (flags.is_unowned ? 1u : 0u) +
         ((flags.is_assign || flags.is_unsafe_unretained) ? 1u : 0u);
}

std::string BuildRuntimeBackedPropertyStorageModifier(
    const Objc3PropertyStorageModifierFlags &flags) {
  if (flags.is_copy) {
    return "copy";
  }
  if (flags.is_retain) {
    return "retain";
  }
  if (flags.is_strong) {
    return "strong";
  }
  if (flags.is_weak) {
    return "weak";
  }
  if (flags.is_unowned) {
    return "unowned";
  }
  if (flags.is_unsafe_unretained) {
    return "unsafe_unretained";
  }
  if (flags.is_assign) {
    return "assign";
  }
  return {};
}

}  // namespace objc3c::support
