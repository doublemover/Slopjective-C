#pragma once

#include <cstddef>
#include <string>

namespace objc3c::support {

struct Objc3PropertyStorageModifierFlags {
  bool is_copy = false;
  bool is_strong = false;
  bool is_retain = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool is_unsafe_unretained = false;
};

Objc3PropertyStorageModifierFlags MakePropertyStorageModifierFlags(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);
bool HasRuntimeManagedPropertyOwnershipModifier(
    const Objc3PropertyStorageModifierFlags &flags);
bool HasExplicitRuntimeBackedPropertyStorageModifier(
    const Objc3PropertyStorageModifierFlags &flags);
std::size_t CountRuntimeBackedPropertyOwnershipModifierSlots(
    const Objc3PropertyStorageModifierFlags &flags);
std::string BuildRuntimeBackedPropertyStorageModifier(
    const Objc3PropertyStorageModifierFlags &flags);

}  // namespace objc3c::support
