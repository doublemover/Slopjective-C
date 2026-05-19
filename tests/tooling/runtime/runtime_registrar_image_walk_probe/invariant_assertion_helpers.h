#pragma once

#include "probe_result.h"
#include "registrar_image_fixture_setup.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

inline std::uint64_t StableSelectorIdFor(const char *selector_name) {
  const objc3_runtime_selector_handle *selector =
      objc3_runtime_lookup_selector(selector_name);
  return selector != nullptr ? selector->stable_id : 0;
}

inline SelectorInvariantObservation CaptureSelectorInvariantObservation() {
  return {
      StableSelectorIdFor(kKnownSelectorName),
      StableSelectorIdFor(kUnknownSelectorName),
  };
}

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
