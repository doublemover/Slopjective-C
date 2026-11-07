#pragma once
// Compile as a PCH and consume from persistence.m. Ownership and move source
// locations survive reload, with a real GNUstep Block copy in ARC and MRC.
#ifdef __cplusplus
extern "C"
#endif
void *_Block_copy(const void *);
typedef int (^CapturePersistenceBlock)(void);
static inline CapturePersistenceBlock capture_persist(int *after)
    __attribute__((ns_returns_retained)) {
  int value = 17;
  CapturePersistenceBlock block = ^[move value] { return value; };
  *after = value;
#if __has_feature(objc_arc)
  return (__bridge_transfer CapturePersistenceBlock)
      _Block_copy((__bridge const void *)block);
#else
  return (CapturePersistenceBlock)_Block_copy((const void *)block);
#endif
}
#if __has_feature(objc_arc)
typedef id (^CapturePersistenceWeakBlock)(void);
static inline CapturePersistenceWeakBlock capture_persist_weak(id input)
    __attribute__((ns_returns_retained)) {
  CapturePersistenceWeakBlock block = ^[weak input] { return input; };
  return (__bridge_transfer CapturePersistenceWeakBlock)
      _Block_copy((__bridge const void *)block);
}
#endif
#ifdef __cplusplus
template<class T> static int capture_persist_template(T input) {
  auto closure = ^[move input] { return input; };
  return closure() + input;
}
#endif
