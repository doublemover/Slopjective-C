#include "runtime/errors/error_bridge_snapshot.h"

#include "runtime/errors/error_bridge_state.h"
#include "runtime/public/objc3_runtime_registration_status.h"

namespace objc3c::runtime {

int CopyRuntimeErrorBridgeStateForTesting(
    objc3_runtime_error_bridge_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  snapshot->store_call_count = state.store_call_count;
  snapshot->load_call_count = state.load_call_count;
  snapshot->status_bridge_call_count = state.status_bridge_call_count;
  snapshot->nserror_bridge_call_count = state.nserror_bridge_call_count;
  snapshot->foreign_exception_bridge_call_count =
      state.foreign_exception_bridge_call_count;
  snapshot->catch_match_call_count = state.catch_match_call_count;
  snapshot->last_stored_error_value = state.last_stored_error_value;
  snapshot->last_loaded_error_value = state.last_loaded_error_value;
  snapshot->last_status_bridge_status_value =
      state.last_status_bridge_status_value;
  snapshot->last_status_bridge_error_value =
      state.last_status_bridge_error_value;
  snapshot->last_nserror_bridge_error_value =
      state.last_nserror_bridge_error_value;
  snapshot->last_foreign_exception_kind = state.last_foreign_exception_kind;
  snapshot->last_foreign_exception_payload_value =
      state.last_foreign_exception_payload_value;
  snapshot->last_foreign_exception_mapped_error_value =
      state.last_foreign_exception_mapped_error_value;
  snapshot->last_foreign_exception_bridge_result =
      state.last_foreign_exception_bridge_result;
  snapshot->last_catch_match_error_value =
      state.last_catch_match_error_value;
  snapshot->last_catch_match_kind = state.last_catch_match_kind;
  snapshot->last_catch_match_is_catch_all =
      state.last_catch_match_is_catch_all;
  snapshot->last_catch_match_result = state.last_catch_match_result;
  snapshot->last_foreign_exception_kind_name =
      state.last_foreign_exception_kind_name.empty()
          ? nullptr
          : state.last_foreign_exception_kind_name.c_str();
  snapshot->last_catch_kind_name = state.last_catch_kind_name.empty()
                                       ? nullptr
                                       : state.last_catch_kind_name.c_str();
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime
