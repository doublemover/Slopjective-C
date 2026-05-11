#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>

namespace objc3c::runtime::probe::deterministic_reset_replay {

inline constexpr const char *kKnownSelectorName = "tokenValue";
inline constexpr const char *kUnknownSelectorName =
    "__objc3_d003_unknown_selector";

inline std::uint64_t LookupSelectorStableId(const char *selector_name) {
  const objc3_runtime_selector_handle *selector =
      objc3_runtime_lookup_selector(selector_name);
  return selector != nullptr ? selector->stable_id : 0;
}

}  // namespace objc3c::runtime::probe::deterministic_reset_replay
