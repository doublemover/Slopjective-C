#include "runtime/errors/error_bridge.h"

#include "runtime/errors/error_bridge_catch_operations.h"
#include "runtime/errors/error_bridge_operations.h"
#include "runtime/errors/error_bridge_snapshot.h"
#include "runtime/errors/error_bridge_state.h"

namespace objc3c::runtime {

void ResetRuntimeErrorBridgeStateForTesting() {
  ResetRuntimeErrorBridgeThreadState();
}

}  // namespace objc3c::runtime

extern "C" void objc3_runtime_store_thrown_error_i32(int *slot, int value) {
  objc3c::runtime::RuntimeStoreThrownErrorI32(slot, value);
}

extern "C" int objc3_runtime_load_thrown_error_i32(const int *slot) {
  return objc3c::runtime::RuntimeLoadThrownErrorI32(slot);
}

extern "C" int objc3_runtime_bridge_status_error_i32(
    int status_value, int mapped_error_value) {
  return objc3c::runtime::RuntimeBridgeStatusErrorI32(status_value,
                                                      mapped_error_value);
}

extern "C" int objc3_runtime_bridge_nserror_error_i32(int error_value) {
  return objc3c::runtime::RuntimeBridgeNSErrorErrorI32(error_value);
}

extern "C" int objc3_runtime_bridge_foreign_exception_error_i32(
    int foreign_kind, int payload_value, int mapped_error_value) {
  return objc3c::runtime::RuntimeBridgeForeignExceptionErrorI32(
      foreign_kind, payload_value, mapped_error_value);
}

extern "C" int objc3_runtime_catch_matches_error_i32(int error_value,
                                                     int catch_kind,
                                                     int catch_all) {
  return objc3c::runtime::RuntimeCatchMatchesErrorI32(error_value, catch_kind,
                                                      catch_all);
}

extern "C" int objc3_runtime_copy_error_bridge_state_for_testing(
    objc3_runtime_error_bridge_state_snapshot *snapshot) {
  return objc3c::runtime::CopyRuntimeErrorBridgeStateForTesting(snapshot);
}
