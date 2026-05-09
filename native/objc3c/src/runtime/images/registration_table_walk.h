#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime {

struct RegisteredImageMetadata;
struct RuntimeState;

bool RuntimeRegistrationTableShapeIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image);
bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record);

}  // namespace objc3c::runtime
