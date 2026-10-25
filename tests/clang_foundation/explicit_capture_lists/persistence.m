#ifndef CAPTURE_PERSISTENCE_PCH
#include "persistence.h"
#endif
#if !__has_feature(objc_arc)
#ifdef __cplusplus
extern "C"
#endif
void _Block_release(const void *);
#endif
int capture_persistence_client(void) {
  int after = -1;
  CapturePersistenceBlock owner = capture_persist(&after);
  int result = owner() + after;
#if __has_feature(objc_arc)
  CapturePersistenceWeakBlock weak = capture_persist_weak((id)0);
  if (weak() != (id)0) __builtin_trap();
#endif
#if !__has_feature(objc_arc)
  _Block_release(owner);
#endif
#ifdef __cplusplus
  result += capture_persist_template(25);
#endif
  return result;
}
