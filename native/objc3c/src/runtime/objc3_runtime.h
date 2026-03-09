#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum objc3_runtime_registration_status_code {
  OBJC3_RUNTIME_REGISTRATION_STATUS_OK = 0,
  OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR = -1,
  OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY = -2,
  OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION = -3,
} objc3_runtime_registration_status_code;

typedef struct objc3_runtime_image_descriptor {
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
  const char *selector;
  uint64_t stable_id;
} objc3_runtime_selector_handle;

typedef struct objc3_runtime_registration_state_snapshot {
  uint64_t registered_image_count;
  uint64_t registered_descriptor_total;
  uint64_t next_expected_registration_order_ordinal;
  uint64_t last_successful_registration_order_ordinal;
  int last_registration_status;
  const char *last_registered_module_name;
  const char *last_registered_translation_unit_identity_key;
  const char *last_rejected_module_name;
  const char *last_rejected_translation_unit_identity_key;
  uint64_t last_rejected_registration_order_ordinal;
} objc3_runtime_registration_state_snapshot;

// M255-D001 lookup-dispatch-runtime anchor: the canonical runtime-owned lookup
// and dispatch surface remains objc3_runtime_lookup_selector plus
// objc3_runtime_dispatch_i32 over objc3_runtime_selector_handle. Later
// selector-table, method-cache, and protocol/category-aware slow-path work must
// extend this ABI without renaming or silently narrowing it.
// M254-D001 runtime-bootstrap-api anchor: these exported C ABI type names and
// function signatures are the canonical bootstrap runtime surface. Later image
// walk, realization, and deterministic-reset issues must extend this boundary
// without renaming or silently narrowing these entrypoints.
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);
int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot);
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
