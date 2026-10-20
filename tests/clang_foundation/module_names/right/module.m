#include "owned.h"
int shared_global = 41;
int shared_helper(void) { return 1 + PRIVATE_BIAS - PRIVATE_BIAS; }
#ifdef __OBJC__
@protocol RuntimeTwin
+ (int)value;
@end
__attribute__((objc_root_class))
@interface RuntimeTwin <RuntimeTwin> { Class isa; @public int slot; }
+ (id)new;
+ (int)value;
+ (int)directValue __attribute__((objc_direct));
+ (int)a_b:(int)value;
+ (int)a:(int)value b:(int)other;
@end
static int destroyed;
@implementation RuntimeTwin
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
+ (int)value { return shared_global + shared_helper(); }
+ (int)directValue { return [self value]; }
+ (int)a_b:(int)value { return value; }
+ (int)a:(int)value b:(int)other { return value + other; }
@end
@interface RuntimeChild : RuntimeTwin
+ (int)childValue;
@end
@implementation RuntimeChild
+ (int)childValue { return [super value]; }
@end
@interface RuntimeTwin (Shared)
+ (int)categoryValue;
@end
@implementation RuntimeTwin (Shared)
+ (int)categoryValue { return [RuntimeTwin directValue]; }
@end
#endif
int right_value(void) {
#ifdef __cplusplus
  if (private_template<int>(0) + private_template<long>(0) + private_variable<0> != 3 * PRIVATE_TEMPLATE_BIAS || PrivateNamespace::private_namespace_helper() != 0) return -1;
#endif
#ifdef __OBJC__
  int observed = 0;
  @autoreleasepool {
    RuntimeTwin *object = [RuntimeTwin new];
    object->slot = 42;
    @try { @throw object; }
    @catch (RuntimeTwin *caught) { observed = caught->slot; }
    if (!class_conformsToProtocol(object_getClass(object), @protocol(RuntimeTwin))) observed = -1;
#if !__has_feature(objc_arc)
    objc_release(object);
#endif
  }
  if (observed != 42 || destroyed != 1 || [RuntimeChild childValue] != [RuntimeTwin value]) return -1;

  if (!@protocol(RuntimeTwin) || [RuntimeTwin a_b:2] != 2 ||
      [RuntimeTwin a:2 b:3] != 5) return -1;
  return [RuntimeTwin value] + [RuntimeTwin categoryValue];
#elif defined(__cplusplus)
  return private_template<int>(shared_global) + private_template<long>(shared_global) + 2 * shared_helper() + private_variable<0> - 3 * PRIVATE_TEMPLATE_BIAS + PrivateNamespace::private_namespace_helper();
#endif
}

int implementation_only(void) { return 7; }
