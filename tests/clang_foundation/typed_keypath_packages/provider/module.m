@import objc3.keypath;
#include <objc/runtime.h>
typedef struct PackagePathPair { long first, second; } PackagePathPair;
__attribute__((objc_root_class))
@interface PackagePathRoot { Class isa; int _count; PackagePathPair _pair; }
+ (instancetype)new;
@property(nonatomic) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic) PackagePathPair pair;
- (void)dealloc;
@end
@implementation PackagePathRoot
@synthesize count = _count, pair = _pair;
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
WritableKeyPath<PackagePathRoot *, long> package_field_path(void) {
  return @keypath(PackagePathRoot, pair.first);
}
