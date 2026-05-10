#pragma once

#include "runtime/public/objc3_runtime_registration.h"
#include "runtime/public/objc3_runtime_registration_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Registers one emitted image descriptor. Returns an
 * objc3_runtime_registration_status_code value.
 */
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);

/*
 * Copies caller-visible registration state into caller-owned storage. Snapshot
 * string fields are borrowed from runtime-owned storage.
 */
int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
