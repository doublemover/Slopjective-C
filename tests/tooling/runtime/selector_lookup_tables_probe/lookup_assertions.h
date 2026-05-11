#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>

namespace objc3c {
namespace runtime {
namespace probe {
namespace selector_lookup_tables {

inline int CopySelectorEntry(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  return objc3_runtime_copy_selector_lookup_entry_for_testing(selector, snapshot);
}

inline std::uint64_t LookupSelectorStableId(const char *selector) {
  const objc3_runtime_selector_handle *handle =
      objc3_runtime_lookup_selector(selector);
  return handle != nullptr ? handle->stable_id : 0;
}

}  // namespace selector_lookup_tables
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
