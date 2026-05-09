#pragma once

#include "runtime/images/registration_table_shape.h"
#include "runtime/state/runtime_bootstrap_contracts.h"

namespace objc3c::runtime {

struct RegisteredImageMetadata;

void PublishRegistrationTableRecord(
    RegisteredImageMetadata &record,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    const RuntimeRegistrationTableDescriptorCounts &counts,
    bool linker_anchor_matches_discovery_root);

}  // namespace objc3c::runtime
