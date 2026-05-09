#include "runtime/errors/error_bridge_operations.h"

#include "runtime/errors/catch_filter.h"
#include "runtime/errors/error_bridge_kind.h"
#include "runtime/errors/error_bridge_state.h"

namespace objc3c::runtime {

void RuntimeStoreThrownErrorI32(int *slot, int value) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.store_call_count;
  state.last_stored_error_value = value;
  if (slot != nullptr) {
    *slot = value;
  }
}

int RuntimeLoadThrownErrorI32(const int *slot) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.load_call_count;
  const int value = slot != nullptr ? *slot : 0;
  state.last_loaded_error_value = value;
  return value;
}

int RuntimeBridgeStatusErrorI32(int status_value, int mapped_error_value) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.status_bridge_call_count;
  state.last_status_bridge_status_value = status_value;
  const int bridged_error =
      mapped_error_value != 0 ? mapped_error_value : status_value;
  state.last_status_bridge_error_value = bridged_error;
  return bridged_error;
}

int RuntimeBridgeNSErrorErrorI32(int error_value) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.nserror_bridge_call_count;
  state.last_nserror_bridge_error_value = error_value;
  return error_value;
}

int RuntimeCatchMatchesErrorI32(int error_value, int catch_kind,
                                int catch_all) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.catch_match_call_count;
  state.last_catch_match_error_value = error_value;
  state.last_catch_match_kind = catch_kind;
  state.last_catch_match_is_catch_all = catch_all != 0 ? 1 : 0;
  state.last_catch_kind_name = RuntimeErrorCatchKindName(catch_kind);
  const int matches =
      RuntimeCatchFilterMatches(error_value, catch_kind, catch_all) ? 1 : 0;
  state.last_catch_match_result = matches;
  return matches;
}

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
  snapshot->catch_match_call_count = state.catch_match_call_count;
  snapshot->last_stored_error_value = state.last_stored_error_value;
  snapshot->last_loaded_error_value = state.last_loaded_error_value;
  snapshot->last_status_bridge_status_value =
      state.last_status_bridge_status_value;
  snapshot->last_status_bridge_error_value =
      state.last_status_bridge_error_value;
  snapshot->last_nserror_bridge_error_value =
      state.last_nserror_bridge_error_value;
  snapshot->last_catch_match_error_value =
      state.last_catch_match_error_value;
  snapshot->last_catch_match_kind = state.last_catch_match_kind;
  snapshot->last_catch_match_is_catch_all =
      state.last_catch_match_is_catch_all;
  snapshot->last_catch_match_result = state.last_catch_match_result;
  snapshot->last_catch_kind_name = state.last_catch_kind_name.empty()
                                       ? nullptr
                                       : state.last_catch_kind_name.c_str();
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime
