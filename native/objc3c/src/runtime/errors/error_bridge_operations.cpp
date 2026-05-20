#include "runtime/errors/error_bridge_operations.h"

#include "runtime/errors/error_bridge_kind.h"
#include "runtime/errors/error_bridge_snapshot_contracts.h"
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

int RuntimeBridgeForeignExceptionErrorI32(int foreign_kind, int payload_value,
                                          int mapped_error_value) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.foreign_exception_bridge_call_count;
  state.last_foreign_exception_kind = foreign_kind;
  state.last_foreign_exception_payload_value = payload_value;
  state.last_foreign_exception_mapped_error_value = mapped_error_value;
  state.last_foreign_exception_kind_name =
      RuntimeForeignExceptionKindName(foreign_kind);

  int result = OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_INVALID_KIND;
  if (RuntimeForeignExceptionKindIsSupported(foreign_kind)) {
    if (mapped_error_value != 0) {
      result = mapped_error_value;
    } else if (payload_value != 0) {
      result = payload_value;
    } else {
      result = OBJC3_RUNTIME_FOREIGN_EXCEPTION_BRIDGE_MISSING_PAYLOAD;
    }
  }
  state.last_foreign_exception_bridge_result = result;
  return result;
}

}  // namespace objc3c::runtime
