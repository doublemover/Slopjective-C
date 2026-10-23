@import objc3.keypath;
#include <objc/runtime.h>
__attribute__((objc_root_class))
@interface PackagePathRoot { Class isa; int _count; }
+ (instancetype)new;
@property(nonatomic) int count;
@property(nonatomic, readonly) int identity;
- (void)dealloc;
@end
@implementation PackagePathRoot
@synthesize count = _count;
+ (instancetype)new { return class_createInstance(self, 0); }
- (int)identity { return _count + 100; }
- (void)dealloc { object_dispose(self); }
@end
WritableKeyPath<PackagePathRoot *, int> package_count_path(void) {
  return @keypath(PackagePathRoot, count);
}
KeyPath<PackagePathRoot *, int> package_identity_path(void) {
  return @keypath(PackagePathRoot, identity);
}
