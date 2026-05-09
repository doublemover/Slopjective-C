#pragma once

#include "runtime/images/registration_table_shape.h"
#include "runtime/state/runtime_bootstrap_contracts.h"

namespace objc3c::runtime {

struct RuntimeState;

int RegisterImageUnlocked(
    RuntimeState &state,
    const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record,
    bool mark_image_local_init_state);

}  // namespace objc3c::runtime
