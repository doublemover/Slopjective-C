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

/* Preserve the concrete error type and value recipe; strict profiles enforce
 * the Error bound even when a particular execution selects the success arm. */
#define __OBJC3_STDLIB_CHECK_ERROR(value) __builtin_objc3_checked_error(value)

#define okOr(value, makeError) \
  (match(value) { \
    case .Some(let __objc3_stdlib_payload) => \
      Ok(__OBJC3_STDLIB_PAYLOAD_TYPE(value), __typeof__((makeError)()), \
         __objc3_stdlib_payload); \
    case .None => \
      Err(__OBJC3_STDLIB_PAYLOAD_TYPE(value), __typeof__((makeError)()), \
          __OBJC3_STDLIB_CHECK_ERROR((makeError)())); \
  })

/* Bridge zero-success status APIs without eagerly constructing either arm.
 * status is evaluated exactly once. successValue is evaluated only on success;
 * makeError (including its construction) is evaluated only on failure and sees
 * the original status value. The caller supplies the API's domain-specific
 * error representation, rather than losing it in an integer error box. */
#define resultFromStatus(status, successValue, makeError) \
  ({ __typeof__(status) __objc3_stdlib_status = (status); \
    __objc3_stdlib_status == 0 \
      ? Ok(__typeof__(successValue), __typeof__((makeError)(status)), (successValue)) \
      : Err(__typeof__(successValue), __typeof__((makeError)(status)), \
            __OBJC3_STDLIB_CHECK_ERROR((makeError)(__objc3_stdlib_status))); })

/* NSError-style BOOL/error-out bridge. Run the API before reading its outputs.
 * A domain adapter maps the raw error (including a missing error on failure)
 * into the caller's Error representation. Success never reads error storage. */
#define resultFromErrorOut(success, successValue, errorValue, makeError) \
  ({ __typeof__(success) __objc3_stdlib_success = (success); \
    __objc3_stdlib_success \
      ? Ok(__typeof__(successValue), __typeof__((makeError)(errorValue)), (successValue)) \
      : Err(__typeof__(successValue), __typeof__((makeError)(errorValue)), \
            __OBJC3_STDLIB_CHECK_ERROR((makeError)(errorValue))); })

#ifdef __OBJC__
#ifdef __cplusplus
/* A reference-capturing thunk is cheap to construct and does not copy the
 * user's C++ factory. Its body, including a factory temporary's construction,
 * is evaluated only on the failing arm. Keep the outer expression a direct
 * checked call so tryValue/tryForce retain their ordinary effect semantics. */
template<class Payload, class Factory>
static inline Payload __objc3_stdlib_or_throw(Optional(Payload) value,
                                               Factory &&factory) throws(id<Error>) {
  match(value) {
    case .Some(let payload): { return payload; }
    case .None: { throwError(__OBJC3_STDLIB_CHECK_ERROR(factory())); }
  }
}
template<class Status, class Factory>
static inline void __objc3_stdlib_status_or_throw(Status status,
                                                 Factory &&factory) throws(id<Error>) {
  if (status != 0)
    throwError(__OBJC3_STDLIB_CHECK_ERROR(factory(status)));
}
#define statusOrThrow(status, makeError) \
  __objc3_stdlib_status_or_throw((status), \
    [&](auto __objc3_stdlib_status) { return (makeError)(__objc3_stdlib_status); })
#define orThrow(value, makeError) \
  __objc3_stdlib_or_throw((value), [&]() { return (makeError)(); })
#else
/* C has no user-defined capture copy constructors. A throwing Block keeps
 * propagation local to the native checked-call boundary. */
#define statusOrThrow(status, makeError) \
  (^(__typeof__(status) __objc3_stdlib_status) throws(id<Error>) { \
    if (__objc3_stdlib_status != 0) \
      throwError(__OBJC3_STDLIB_CHECK_ERROR((makeError)(__objc3_stdlib_status))); \
  })(status)
#define orThrow(value, makeError) \
  (^(__typeof__(value) __objc3_stdlib_input) throws(id<Error>) { \
    match(__objc3_stdlib_input) { \
      case .Some(let __objc3_stdlib_payload): { return __objc3_stdlib_payload; } \
      case .None: { throwError(__OBJC3_STDLIB_CHECK_ERROR((makeError)())); } \
    } \
  })(value)
#endif
#endif

#endif
