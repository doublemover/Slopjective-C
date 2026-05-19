#pragma once

#include "runtime/public/objc3_runtime_selector.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Looks up a selector spelling in the runtime selector table. The returned
 * handle and selector spelling are runtime-owned and borrowed by the caller.
 */
const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector);

#ifdef __cplusplus
}
#endif
