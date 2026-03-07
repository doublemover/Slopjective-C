#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_image_descriptor {
  const char *module_name;
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

int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image);
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);
int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3);
void objc3_runtime_reset_for_testing(void);

#ifdef __cplusplus
}
#endif
