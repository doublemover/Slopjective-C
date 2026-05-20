#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_error_bridge_state_snapshot {
  uint64_t store_call_count;
  uint64_t load_call_count;
  uint64_t status_bridge_call_count;
  uint64_t nserror_bridge_call_count;
  uint64_t foreign_exception_bridge_call_count;
  uint64_t catch_match_call_count;
  int last_stored_error_value;
  int last_loaded_error_value;
  int last_status_bridge_status_value;
  int last_status_bridge_error_value;
  int last_nserror_bridge_error_value;
  int last_foreign_exception_kind;
  int last_foreign_exception_payload_value;
  int last_foreign_exception_mapped_error_value;
  int last_foreign_exception_bridge_result;
  int last_catch_match_error_value;
  int last_catch_match_kind;
  int last_catch_match_is_catch_all;
  int last_catch_match_result;
  const char *last_foreign_exception_kind_name;
  const char *last_catch_kind_name;
} objc3_runtime_error_bridge_state_snapshot;

typedef enum objc3_runtime_foreign_exception_bridge_code {
  OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_INVALID_KIND = -3901,
  OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_MISSING_PAYLOAD = -3902,
} objc3_runtime_foreign_exception_bridge_code;

// error-runtime/bridge-helper anchor: the supported runnable Part 6
// slice now uses one narrow private helper ABI for thrown-error slot traffic,
// bridge normalization, and catch-kind matching without widening the public
// runtime header or claiming generalized foreign exception support.
// live catch/bridge/runtime integration anchor: linked native Part 6
// probes now execute through this same helper cluster and validate the helper
// traffic through the retained private snapshot surface below.
// cross-module preservation anchor: imported modules preserve this
// same Part 6 helper cluster indirectly through replay sidecars and cross-image
// link-plan validation rather than by widening the runtime helper ABI again.
// bridge-packaging/toolchain anchor: the truthful Part 11 runtime
// boundary also stays private and snapshot-backed. The current freeze claims
// packaged-runtime archive continuity, registration-manifest/link-plan
// topology, and operator-visible evidence only; header/module/bridge
// generation remains deferred to the next runtime step.
void objc3_runtime_store_thrown_error_i32(int *slot, int value);
int objc3_runtime_load_thrown_error_i32(const int *slot);
int objc3_runtime_bridge_status_error_i32(int status_value,
                                          int mapped_error_value);
int objc3_runtime_bridge_nserror_error_i32(int error_value);
int objc3_runtime_bridge_foreign_exception_error_i32(
    int foreign_kind, int payload_value, int mapped_error_value);
int objc3_runtime_catch_matches_error_i32(int error_value, int catch_kind,
                                          int catch_all);
int objc3_runtime_copy_error_bridge_state_for_testing(
    objc3_runtime_error_bridge_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
