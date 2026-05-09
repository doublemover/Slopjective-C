#pragma once

#include <stddef.h>
#include <stdint.h>

#include "runtime/public/objc3_runtime_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Exported runtime C ABI.
 *
 * Header ownership:
 * - objc3_runtime_api.h owns callable runtime entrypoints and caller-visible
 *   snapshot structs.
 * - objc3_runtime_result.h owns status/result payload layout.
 * - runtime implementation directories own storage, dispatch, selector, image,
 *   class, and reset behavior behind this ABI.
 *
 * Borrowed input strings are copied into runtime-owned storage only where the
 * individual entrypoint says so. Borrowed output strings are runtime-owned and
 * valid until the owning table/snapshot state is reset or mutated.
 */
typedef struct objc3_runtime_image_descriptor {
  /* Borrowed by the registration call and copied into runtime-owned storage. */
  const char *module_name;
  const char *translation_unit_identity_key;
  uint64_t registration_order_ordinal;
  uint64_t class_descriptor_count;
  uint64_t protocol_descriptor_count;
  uint64_t category_descriptor_count;
  uint64_t property_descriptor_count;
  uint64_t ivar_descriptor_count;
} objc3_runtime_image_descriptor;

typedef struct objc3_runtime_selector_handle {
  /* Runtime-owned spelling; valid until the selector table is reset/mutated. */
  const char *selector;
  uint64_t stable_id;
} objc3_runtime_selector_handle;

typedef struct objc3_runtime_registration_state_snapshot {
  uint64_t registered_image_count;
  uint64_t registered_descriptor_total;
  uint64_t next_expected_registration_order_ordinal;
  uint64_t last_successful_registration_order_ordinal;
  int last_registration_status;
  /* Runtime-owned borrowed snapshot strings; callers must not free them. */
  const char *last_registered_module_name;
  const char *last_registered_translation_unit_identity_key;
  const char *last_rejected_module_name;
  const char *last_rejected_translation_unit_identity_key;
  uint64_t last_rejected_registration_order_ordinal;
} objc3_runtime_registration_state_snapshot;

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
