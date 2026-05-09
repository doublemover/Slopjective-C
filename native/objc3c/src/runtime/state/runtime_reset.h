#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

using RuntimeImageRegistrationReplayCallback = int (*)(
    RuntimeState &state,
    const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record,
    bool mark_image_local_init_state);

bool RuntimeResetPreservesBootstrapCatalog();
bool RuntimeResetClearsLiveExecutionState();
std::uint64_t ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
    RuntimeState &state);
void ClearLiveRegistrationStateUnlocked(RuntimeState &state);
int ReplayRegisteredImagesForTestingUnlocked(
    RuntimeState &state, RuntimeImageRegistrationReplayCallback register_image);

}  // namespace objc3c::runtime
