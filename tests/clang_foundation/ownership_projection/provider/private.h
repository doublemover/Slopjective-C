#include <objc/runtime.h>
#include <objc/objc-arc.h>
#ifdef __cplusplus
extern "C" {
#endif
// The runtime allocator returns an owned instance in both compilation modes.
id class_createInstance(Class type, size_t extra)
    __attribute__((ns_returns_retained));
#ifdef __cplusplus
}
#endif
__attribute__((objc_root_class))
@interface ProjectionObject {
  Class isa;
}
+ (id)new;
@end
