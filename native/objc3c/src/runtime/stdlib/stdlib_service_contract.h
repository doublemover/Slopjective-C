#pragma once

#include "runtime/public/objc3_text.h"
#include "runtime/public/objc3_collections.h"

#ifdef __cplusplus
extern "C" {
#endif

// Version 1 int handles remain a separate compatibility domain. These
// functions retain/release only handles returned by the matching legacy text
// or collections API above.
int objc3_runtime_stdlib_text_retain_i32(int handle);
int objc3_runtime_stdlib_text_release_i32(int handle);
int objc3_runtime_stdlib_collections_retain_i32(int handle);
int objc3_runtime_stdlib_collections_release_i32(int handle);

objc3_text_v2_t objc3_runtime_stdlib_text_v2_create_utf8(
    const char *utf8_bytes, uint64_t byte_count);
objc3_text_v2_t objc3_runtime_stdlib_text_v2_retain(objc3_text_v2_t text);
void objc3_runtime_stdlib_text_v2_release(objc3_text_v2_t text);
int objc3_runtime_stdlib_text_v2_byte_count(objc3_text_v2_t text,
                                            uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_byte_at(objc3_text_v2_t text,
                                         uint64_t index,
                                         uint8_t *out_byte);

objc3_collection_v2_t objc3_runtime_stdlib_collections_v2_create_i32(
    const int32_t *values, uint64_t count);
objc3_collection_v2_t objc3_runtime_stdlib_collections_v2_retain(
    objc3_collection_v2_t collection);
void objc3_runtime_stdlib_collections_v2_release(
    objc3_collection_v2_t collection);
int objc3_runtime_stdlib_collections_v2_count(objc3_collection_v2_t collection,
                                              uint64_t *out_count);
int objc3_runtime_stdlib_collections_v2_get(objc3_collection_v2_t collection,
                                             uint64_t index,
                                             int32_t *out_value);

#ifdef __cplusplus
}
#endif
