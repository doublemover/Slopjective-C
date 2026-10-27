@import objc3.keypath;
#include <objc/runtime.h>
typedef struct PackagePathPair { long first, second; } PackagePathPair;
typedef struct PackagePathBits { unsigned value:3; signed small:5; unsigned sibling:7; } PackagePathBits;
__attribute__((objc_root_class))
@interface PackagePathRoot { Class isa; int _count; PackagePathPair _pair; PackagePathBits _bits; }
+ (instancetype)new;
@property(nonatomic) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic) PackagePathPair pair;
@property(nonatomic) PackagePathBits bits;
- (void)dealloc;
@end
static unsigned live_roots;
unsigned package_live_roots(void) { return live_roots; }
@implementation PackagePathRoot
@synthesize count = _count, pair = _pair, bits = _bits;
+ (instancetype)new {
  PackagePathRoot *root = class_createInstance(self, 0);
  ++live_roots;
  return root;
}
- (int)identity { return _count + 100; }
- (void)dealloc { --live_roots; object_dispose(self); }
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

KeyPath<PackagePathRoot *, int> package_narrowed_path(void) {
  return package_count_path();
}

WritableKeyPath<PackagePathRoot *, unsigned> package_bits_path(void) {
  return @keypath(PackagePathRoot, bits.value);
}
