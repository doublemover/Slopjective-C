#ifndef OBJC3_ERRORS_H
#define OBJC3_ERRORS_H

#include "core.h"
#include <objc3c/runtime/objc3_runtime_services.h>

#define OBJC3_ERRORS_CAPABILITY_ID "objc3.cap.errors"

#ifdef __OBJC__
@protocol Error
@end
#endif

/* Result(T, E), Ok(T, E, value), Err(T, E, error), isOk and match are native
 * compiler operations. Runtime error boxes preserve their payload operations
 * across C ABI task boundaries; they do not implicitly erase an Error object. */

/* A throwing Block keeps propagation local to the ordinary call boundary.
 * typeof operands are unevaluated: input runs once, and the factory expression
 * (including construction of the factory itself) runs only on the None arm.
 * The inferred payload type avoids a separate, unchecked C spelling. */
#define __OBJC3_STDLIB_PAYLOAD_TYPE(value) \
  __typeof__((^(__typeof__(value) __objc3_stdlib_probe) { \
    match(__objc3_stdlib_probe) { \
      case .Some(let __objc3_stdlib_payload): { return __objc3_stdlib_payload; } \
      case .None: { __builtin_unreachable(); } \
    } \
  })(value))

#define okOr(value, makeError) \
  (^Result(__OBJC3_STDLIB_PAYLOAD_TYPE(value), __typeof__((makeError)())) \
      (__typeof__(value) __objc3_stdlib_input) { \
    match(__objc3_stdlib_input) { \
      case .Some(let __objc3_stdlib_payload): { \
        return Ok(__OBJC3_STDLIB_PAYLOAD_TYPE(value), \
                  __typeof__((makeError)()), __objc3_stdlib_payload); \
      } \
      case .None: { \
        return Err(__OBJC3_STDLIB_PAYLOAD_TYPE(value), \
                   __typeof__((makeError)()), (makeError)()); \
      } \
    } \
  })(value)

/* Bridge zero-success status APIs without eagerly constructing either arm.
 * status is evaluated exactly once. successValue is evaluated only on success;
 * makeError (including its construction) is evaluated only on failure and sees
 * the original status value. The caller supplies the API's domain-specific
 * error representation, rather than losing it in an integer error box. */
#define resultFromStatus(status, successValue, makeError) \
  ({ __typeof__(status) __objc3_stdlib_saved_status = (status); \
  (^Result(__typeof__(successValue), __typeof__((makeError)(status))) \
      (__typeof__(status) __objc3_stdlib_status) { \
    if (__objc3_stdlib_status == 0) \
      return Ok(__typeof__(successValue), __typeof__((makeError)(status)), \
                (successValue)); \
    return Err(__typeof__(successValue), __typeof__((makeError)(status)), \
               (makeError)(__objc3_stdlib_status)); \
  })(__objc3_stdlib_saved_status); })

/* NSError-style BOOL/error-out bridge. Run the API before capturing its
 * outputs. A domain adapter maps the raw error (including a missing error on
 * failure) into the caller's Error representation; success ignores it. */
#define resultFromErrorOut(success, successValue, errorValue, makeError) \
  ({ __typeof__(success) __objc3_stdlib_succeeded = (success); \
  (^Result(__typeof__(successValue), __typeof__((makeError)(errorValue))) \
      (__typeof__(success) __objc3_stdlib_success) { \
    if (__objc3_stdlib_success) \
      return Ok(__typeof__(successValue), __typeof__((makeError)(errorValue)), \
                (successValue)); \
    return Err(__typeof__(successValue), __typeof__((makeError)(errorValue)), \
               (makeError)(errorValue)); \
  })(__objc3_stdlib_succeeded); })

#ifdef __OBJC__
/* Checked-error counterpart for status APIs whose successful result is void. */
#define statusOrThrow(status, makeError) \
  (^(__typeof__(status) __objc3_stdlib_status) throws(id<Error>) { \
    if (__objc3_stdlib_status != 0) \
      throwError((makeError)(__objc3_stdlib_status)); \
  })(status)
#endif

#ifdef __OBJC__
#define orThrow(value, makeError) \
  (^(__typeof__(value) __objc3_stdlib_input) throws(id<Error>) { \
    match(__objc3_stdlib_input) { \
      case .Some(let __objc3_stdlib_payload): { return __objc3_stdlib_payload; } \
      case .None: { throwError((makeError)()); } \
    } \
  })(value)
#endif

#endif
