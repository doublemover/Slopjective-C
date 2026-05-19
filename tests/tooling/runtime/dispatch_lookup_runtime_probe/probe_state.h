#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_PROBE_STATE_H_

#include "runtime/public/objc3_runtime_api.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

struct SelectorLookupCapture {
  bool lookup_null_is_null = false;
  bool copy_selector_reused = false;
  bool copy_selector_spelling_matches = false;
  std::uint64_t copy_selector_stable_id = 0;
  std::uint64_t gamma_selector_stable_id = 0;
};

struct DispatchCapture {
  int dispatch_result = 0;
  int expected_dispatch_result = 0;
  int nil_dispatch_result = 0;
};

struct RegistrationSnapshotCapture {
  objc3_runtime_registration_state_snapshot snapshot{};
  int snapshot_status = 0;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
};

struct ResetLookupCapture {
  std::uint64_t copy_after_reset_stable_id = 0;
};

struct DispatchLookupProbeResult {
  int register_status = 0;
  SelectorLookupCapture selectors;
  DispatchCapture dispatch;
  RegistrationSnapshotCapture registration;
  ResetLookupCapture reset_lookup;
};

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_PROBE_STATE_H_
