#include "private.h"
static int created, destroyed;
static id owner;
@implementation ProjectionObject
+ (id)new { ++created; return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
#ifdef __cplusplus
extern "C" {
#endif
id projected_new(void) __attribute__((ns_returns_retained));
id projected_new(void) { return [ProjectionObject new]; }
id projected_borrow(void) __attribute__((ns_returns_not_retained));
id projected_borrow(void) {
  if (!owner) owner = [ProjectionObject new];
  return owner;
}
id projected_autoreleased(void) __attribute__((ns_returns_autoreleased));
id projected_autoreleased(void) {
#if __has_feature(objc_arc)
  return [ProjectionObject new];
#else
  return objc_autorelease([ProjectionObject new]);
#endif
}
void projected_clear(void) {
#if !__has_feature(objc_arc)
  objc_release(owner);
#endif
  owner = (id)0;
}
int projected_observe(id value) { return value != (id)0; }
int projected_created(void) { return created; }
int projected_destroyed(void) { return destroyed; }
#ifdef __cplusplus
}
#endif
