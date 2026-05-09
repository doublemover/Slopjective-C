#pragma once

#include "runtime/concurrency/runtime_concurrency_snapshot_contracts.h"

namespace objc3c::runtime {

struct RuntimeTaskState;

void ResetRuntimeTaskRuntimeStateSnapshot(
    objc3_runtime_task_runtime_state_snapshot &snapshot);
void PopulateRuntimeTaskRuntimeStateSnapshot(
    const RuntimeTaskState &state,
    objc3_runtime_task_runtime_state_snapshot &snapshot);

}  // namespace objc3c::runtime
