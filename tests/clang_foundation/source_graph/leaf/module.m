typedef int LeafValue;
int leaf(void) { return 40; }

#if defined(__OBJC__) && defined(__cplusplus) && __has_feature(objc_arc)
#include <objc/runtime.h>
extern "C" id class_createInstance(Class cls, unsigned long extra)
    __attribute__((ns_returns_retained));
extern "C" id object_dispose(id object);
typedef Optional(id) PackageStrong;
typedef Optional(id __weak) PackageWeak;
struct PackageValue {
  int marker;
  PackageStrong value;
  PackageWeak weak;
};
static int package_releases;
__attribute__((objc_root_class))
@interface PackageObject {
  Class isa;
}
+ (id)new;
@end
@implementation PackageObject
+ (id)new {
  return class_createInstance(self, 0);
}
- (void)dealloc {
  ++package_releases;
  object_dispose(self);
}
@end
struct PackageValue package_value(void) {
  id object = [PackageObject new];
  return PackageValue{42, Some(id, object),
                      Some(id __weak, object)};
}
int package_destroyed(void) { return package_releases; }
#endif
