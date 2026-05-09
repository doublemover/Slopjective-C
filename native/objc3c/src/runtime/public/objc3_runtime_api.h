#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"
#include "runtime/public/objc3_runtime_registration.h"
#include "runtime/public/objc3_runtime_registration_status.h"
#include "runtime/public/objc3_runtime_selector.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exported runtime C ABI.
 *
 * Header ownership:
 * - objc3_runtime_api.h owns callable runtime entrypoints and caller-visible
 *   snapshot structs.
 * - objc3_runtime_registration_status.h owns registration status codes.
 * - objc3_runtime_dispatch_result.h owns checked i32 dispatch payloads.
 * - runtime implementation directories own storage, dispatch, selector, image,
 *   class, and reset behavior behind this ABI.
 *
 * Borrowed input strings are copied into runtime-owned storage only where the
 * individual entrypoint says so. Borrowed output strings are runtime-owned and
 * valid until the owning table/snapshot state is reset or mutated.
 */
/*
 * Registers one emitted image descriptor. Returns an
 * objc3_runtime_registration_status_code value.
 */
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);

/*
 * Looks up a selector spelling in the runtime selector table. The returned
 * handle and selector spelling are runtime-owned and borrowed by the caller.
 */
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);

/*
 * Strict checked i32 dispatch. Non-OK results carry canonical diagnostic fields
 * and a zero value; OK results carry the returned i32 value.
 */
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);

/*
 * Narrow plain i32 dispatch entrypoint used by lowered runnable sends. Callers
 * that need status/diagnostic ownership should use
 * objc3_runtime_dispatch_i32_checked().
 */
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);

/*
 * Copies caller-visible registration state into caller-owned storage. Snapshot
 * string fields are borrowed from runtime-owned storage.
 */
int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot);

/*
 * Clears runtime-owned state tables used by this public ABI.
 */
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
