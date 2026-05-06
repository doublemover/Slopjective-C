#pragma once

#include <stddef.h>
#include <stdint.h>

#include "runtime/public/objc3_runtime_result.h"

#ifdef __cplusplus
extern "C" {
#endif

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

int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver,
    const char *selector,
    int a0,
    int a1,
    int a2,
    int a3);
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);
int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot);
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
