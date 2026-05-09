#pragma once

#include "runtime/images/registration_table_shape.h"
#include "runtime/state/runtime_bootstrap_contracts.h"

namespace objc3c::runtime {

struct RegisteredImageMetadata;
struct RuntimeState;

bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record);

}  // namespace objc3c::runtime
