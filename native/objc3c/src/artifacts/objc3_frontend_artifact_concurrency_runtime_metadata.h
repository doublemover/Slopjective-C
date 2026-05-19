#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendConcurrencyRuntimeMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string
        &concurrency_task_runtime_interop_cancellation_lowering_replay_key,
    const Objc3TaskRuntimeInteropCancellationLoweringContract
        &concurrency_task_runtime_interop_cancellation_lowering_contract,
    const std::string &concurrency_replay_race_guard_lowering_replay_key,
    const Objc3ConcurrencyReplayRaceGuardLoweringContract
        &concurrency_replay_race_guard_lowering_contract);

}  // namespace objc3::artifacts::frontend
