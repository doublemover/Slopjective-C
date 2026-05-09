#include "support/objc3_property_storage_modifier_spelling.h"

namespace objc3c::support {

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
