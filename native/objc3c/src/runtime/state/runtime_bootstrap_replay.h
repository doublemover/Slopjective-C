#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

using RuntimeImageRegistrationReplayCallback = int (*)(
    RuntimeState &state,
    const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record,
    bool mark_image_local_init_state);

std::uint64_t ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
    RuntimeState &state);
int ReplayRegisteredImagesForTestingUnlocked(
    RuntimeState &state, RuntimeImageRegistrationReplayCallback register_image);

}  // namespace objc3c::runtime
