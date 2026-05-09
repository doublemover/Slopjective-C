#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

void RuntimeStoreThrownErrorI32(int *slot, int value);
int RuntimeLoadThrownErrorI32(const int *slot);
int RuntimeBridgeStatusErrorI32(int status_value, int mapped_error_value);
int RuntimeBridgeNSErrorErrorI32(int error_value);
int RuntimeCatchMatchesErrorI32(int error_value, int catch_kind,
                                int catch_all);
int CopyRuntimeErrorBridgeStateForTesting(
    objc3_runtime_error_bridge_state_snapshot *snapshot);

}  // namespace objc3c::runtime
