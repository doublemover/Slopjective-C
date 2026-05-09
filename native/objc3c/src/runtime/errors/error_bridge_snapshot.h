#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

int CopyRuntimeErrorBridgeStateForTesting(
    objc3_runtime_error_bridge_state_snapshot *snapshot);

}  // namespace objc3c::runtime
