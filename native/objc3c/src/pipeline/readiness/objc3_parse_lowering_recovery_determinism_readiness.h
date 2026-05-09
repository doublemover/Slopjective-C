#pragma once

#include "pipeline/objc3_frontend_types.h"

struct Objc3ParseLoweringRecoveryDeterminismReadinessRecord {
  bool toolchain_runtime_ga_operations_recovery_determinism_consistent = false;
  bool toolchain_runtime_ga_operations_recovery_determinism_ready = false;
};

Objc3ParseLoweringRecoveryDeterminismReadinessRecord
ApplyObjc3ParseLoweringRecoveryDeterminismReadiness(
    Objc3ParseLoweringReadinessSurface &surface);
