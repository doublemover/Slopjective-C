#ifndef OBJC3_RUNTIME_PUBLIC_OBJC3_TEXT_H
#define OBJC3_RUNTIME_PUBLIC_OBJC3_TEXT_H

#include "objc3_stdlib_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// Typed opaque v2 domains. Values are opaque identities and are never native
// Objective-C object IDs or dereferenceable user pointers.
typedef struct objc3_text_v2_opaque *objc3_text_v2_t;
typedef struct objc3_text_builder_v2_opaque *objc3_text_builder_v2_t;

// Checked UTF-8 operations use byte lengths and byte offsets explicitly.
// Text inputs must be valid UTF-8; embedded NUL is ordinary data.
int objc3_runtime_stdlib_text_v2_create_utf8_checked(
    const char *bytes, uint64_t byte_count, objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_v2_retain_checked(objc3_text_v2_t text,
                                                 objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_v2_release_checked(objc3_text_v2_t text);
int objc3_runtime_stdlib_text_v2_byte_count_checked(objc3_text_v2_t text,
                                                     uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_byte_at_checked(objc3_text_v2_t text,
                                                  uint64_t index,
                                                  uint8_t *out_byte);
int objc3_runtime_stdlib_text_v2_scalar_count_checked(objc3_text_v2_t text,
                                                       uint64_t *out_count);
int objc3_runtime_stdlib_text_v2_slice_scalars(
    objc3_text_v2_t text, uint64_t start_scalar, uint64_t scalar_count,
    objc3_text_v2_t *out_text);
// Scalar-index search only reports matches whose beginning and end are UTF-8
// scalar boundaries. start_scalar and the returned index count Unicode scalar
// values, not bytes or grapheme clusters. The needle must be valid UTF-8.
// Empty needles match start_scalar, including the end of the text. A start
// beyond the scalar count returns OUT_OF_BOUNDS; no match returns OK/found=0.
int objc3_runtime_stdlib_text_v2_find_scalars(
    objc3_text_v2_t text, const char *needle_utf8, uint64_t needle_byte_count,
    uint64_t start_scalar, int *out_found, uint64_t *out_scalar_index);
int objc3_runtime_stdlib_text_v2_find_bytes(
    objc3_text_v2_t text, const char *needle, uint64_t needle_byte_count,
    uint64_t start_byte, int *out_found, uint64_t *out_byte_index);
int objc3_runtime_stdlib_text_v2_concat(
    objc3_text_v2_t left, objc3_text_v2_t right,
    objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_builder_v2_create(
    objc3_text_builder_v2_t *out_builder);
int objc3_runtime_stdlib_text_builder_v2_append_utf8(
    objc3_text_builder_v2_t builder, const char *bytes, uint64_t byte_count);
// Appends one Unicode scalar, encoded as UTF-8. Surrogates and out-of-range
// code points return INVALID_ARGUMENT without changing the builder.
int objc3_runtime_stdlib_text_builder_v2_append_scalar(
    objc3_text_builder_v2_t builder, uint32_t scalar);
int objc3_runtime_stdlib_text_builder_v2_append_text(
    objc3_text_builder_v2_t builder, objc3_text_v2_t text);
// Finish produces an independent text snapshot and preserves builder contents.
// Reset clears those contents. Both operations leave the builder reusable.
int objc3_runtime_stdlib_text_builder_v2_finish(
    objc3_text_builder_v2_t builder, objc3_text_v2_t *out_text);
int objc3_runtime_stdlib_text_builder_v2_reset(
    objc3_text_builder_v2_t builder);
int objc3_runtime_stdlib_text_builder_v2_release(
    objc3_text_builder_v2_t builder);


#ifdef __cplusplus
}
#endif
#endif
