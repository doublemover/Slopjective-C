#ifndef OBJC3_CORE_H
#define OBJC3_CORE_H

#include <stddef.h>
#include <stdint.h>

#if !defined(__OBJC3_LANGUAGE_REVISION__)
#error "objc3.core requires Objective-C 3.0 language mode (-fobjc-version=3)"
#endif

#define OBJC3_STDLIB_VERSION_MAJOR 2u
#define OBJC3_STDLIB_VERSION_MINOR 0u
#define OBJC3_STDLIB_VERSION_PATCH 0u
#if defined(OBJC3_STDLIB_REQUIRED_MAJOR) && OBJC3_STDLIB_REQUIRED_MAJOR != 2
#error "incompatible Objective-C 3.0 standard-library major version"
#endif
#define OBJC3_LANGUAGE_REVISION __OBJC3_LANGUAGE_REVISION__
#define OBJC3_PROFILE_REVISION 1u
#define OBJC3_ACTIVE_PROFILE __OBJC3_PROFILE__

#define OBJC3_CORE_CAPABILITY_ID "objc3.cap.core"

/* These report compiler support for the active target and profile. A module's
 * presence does not imply that its entire normative feature set is complete. */
typedef enum objc3_language_feature {
  OBJC3_FEATURE_DEFER,
  OBJC3_FEATURE_OPTIONALS,
  OBJC3_FEATURE_RESULT,
  OBJC3_FEATURE_THROWS,
  OBJC3_FEATURE_ASYNC_AWAIT,
  OBJC3_FEATURE_ACTORS
} objc3_language_feature;

static inline int objc3_has_feature(objc3_language_feature feature) {
  switch (feature) {
  case OBJC3_FEATURE_DEFER: return __has_feature(objc3_defer);
  case OBJC3_FEATURE_OPTIONALS: return __has_feature(objc3_optionals);
  case OBJC3_FEATURE_RESULT: return __has_feature(objc3_result);
  case OBJC3_FEATURE_THROWS: return __has_feature(objc3_throws);
  case OBJC3_FEATURE_ASYNC_AWAIT: return __has_feature(objc3_async_await);
  case OBJC3_FEATURE_ACTORS: return __has_feature(objc3_actors);
  }
  return 0;
}

typedef struct objc3_byte_span {
  const uint8_t *data;
  uint64_t count;
} objc3_byte_span;

#endif
