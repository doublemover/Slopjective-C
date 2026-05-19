#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

namespace objc3c::runtime {

struct RegisteredImageMetadata;
struct RuntimeState;

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status);
void ClearRejectedRegistrationUnlocked(RuntimeState &state);
void ApplyImageWalkRecordUnlocked(RuntimeState &state,
                                  const RegisteredImageMetadata &record);
void RetainBootstrapRecordUnlocked(RuntimeState &state,
                                   const RegisteredImageMetadata &record);

}  // namespace objc3c::runtime
