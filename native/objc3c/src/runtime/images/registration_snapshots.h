#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RuntimeState;

void ClearImageWalkSnapshotUnlocked(RuntimeState &state);
int CopyRuntimeImageWalkStateForTesting(
    objc3_runtime_image_walk_state_snapshot *snapshot);
int CopyRuntimeRegistrationStateForTesting(
    objc3_runtime_registration_state_snapshot *snapshot);
int CopyRuntimeResetReplayStateForTesting(
    objc3_runtime_reset_replay_state_snapshot *snapshot);

}  // namespace objc3c::runtime
