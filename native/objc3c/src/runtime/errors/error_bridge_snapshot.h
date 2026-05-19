#pragma once

#include "runtime/errors/error_bridge_snapshot_contracts.h"

namespace objc3c::runtime {

int CopyRuntimeErrorBridgeStateForTesting(
    objc3_runtime_error_bridge_state_snapshot *snapshot);

}  // namespace objc3c::runtime
