#pragma once
// Emit this header as PCH, then include it in persistence.m. Capture-list source
// ranges, explicit ownership, unused fields and move kinds must survive reload.
__attribute__((objc_root_class))
@interface CapturePersistenceMethods
- (id)copy;
@end
typedef int (^CapturePersistenceBlock)(void);
static inline CapturePersistenceBlock capture_persist(int *after) {
  int value = 17;
  CapturePersistenceBlock block = ^[move value] { return value; };
  *after = value;
  return [block copy];
}
#if __has_feature(objc_arc)
static inline id (^capture_persist_weak(id input))(void) {
  return [^[weak input] { return input; } copy];
}
#endif
#ifdef __cplusplus
template<class T> static int capture_persist_template(T input) {
  auto closure = ^[move input] { return input; };
  return closure() + input;
}
#endif
