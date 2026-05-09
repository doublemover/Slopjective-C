#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_selector_handle {
  /* Runtime-owned spelling; valid until the selector table is reset/mutated. */
  const char *selector;
  uint64_t stable_id;
} objc3_runtime_selector_handle;

#ifdef __cplusplus
}
#endif
