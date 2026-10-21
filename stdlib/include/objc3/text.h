#ifndef OBJC3_TEXT_H
#define OBJC3_TEXT_H

#include "core.h"
#include <objc3c/runtime/objc3_text.h>

#define OBJC3_TEXT_CAPABILITY_ID "objc3.cap.text"

/* Literal/imported metadata supplies both counts. Validate bytes and the
 * declared scalar shape before transferring ownership to the caller. */
static inline int objc3_text_from_utf8_record(const char *bytes,
    uint64_t byte_count, uint64_t scalar_count, objc3_text_v2_t *out_text) {
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = (objc3_text_v2_t)0;
  objc3_text_v2_t text = (objc3_text_v2_t)0;
  int status = objc3_runtime_stdlib_text_v2_create_utf8_checked(bytes, byte_count, &text);
  if (status != OBJC3_STDLIB_V2_OK) return status;
  uint64_t actual = 0;
  status = objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &actual);
  if (status != OBJC3_STDLIB_V2_OK || actual != scalar_count) {
    (void)objc3_runtime_stdlib_text_v2_release_checked(text);
    return status != OBJC3_STDLIB_V2_OK ? status : OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  }
  *out_text = text;
  return OBJC3_STDLIB_V2_OK;
}

/* Prefix counts are Unicode scalar counts, never byte or grapheme counts.
 * The result owns an independent immutable snapshot. */
static inline int objc3_text_prefix(objc3_text_v2_t text, uint64_t requested,
                                   objc3_text_v2_t *out_text) {
  uint64_t count = 0;
  if (!out_text) return OBJC3_STDLIB_V2_INVALID_ARGUMENT;
  *out_text = (objc3_text_v2_t)0;
  int status = objc3_runtime_stdlib_text_v2_scalar_count_checked(text, &count);
  if (status != OBJC3_STDLIB_V2_OK) return status;
  return objc3_runtime_stdlib_text_v2_slice_scalars(
      text, 0, requested < count ? requested : count, out_text);
}

#endif
