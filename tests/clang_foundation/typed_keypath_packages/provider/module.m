@import objc3.keypath;
#include <objc/runtime.h>
#include <objc/objc-arc.h>
typedef struct PackagePathPair { long first, second; } PackagePathPair;
typedef struct PackagePathBits { unsigned value:3; signed small:5; unsigned sibling:7; } PackagePathBits;
__attribute__((objc_root_class))
@interface PackagePathRoot { Class isa; int _count; PackagePathPair _pair; PackagePathBits _bits; PackagePathRoot *_next, *_value; }
+ (instancetype)new;
@property(nonatomic) int count;
@property(nonatomic, strong) PackagePathRoot * _Nullable next;
@property(nonatomic, strong) PackagePathRoot * _Nullable value;
@property(nonatomic, readonly) int identity;
@property(nonatomic) PackagePathPair pair;
@property(nonatomic) PackagePathBits bits;
- (void)dealloc;
@end
static unsigned live_roots;
unsigned package_live_roots(void) { return live_roots; }
@implementation PackagePathRoot
@synthesize count = _count, pair = _pair, bits = _bits, next = _next, value = _value;
+ (instancetype)new {
  PackagePathRoot *root = class_createInstance(self, 0);
  ++live_roots;
  return root;
}
- (int)identity { return _count + 100; }
- (void)dealloc {
#if !__has_feature(objc_arc)
  objc_release(_next); objc_release(_value);
#endif
  --live_roots; object_dispose(self);
}
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

int package_indexed_update(PackagePathRoot *_Nonnull root,
    WritableKeyPath<PackagePathRoot*,int> path,int value) {
  int assigned=(root[path]=value);int increased=(root[path]+=2);
  int before=root[path]++;int after=++root[path];
  return assigned+increased+before+after;
}

WritableKeyPath<PackagePathRoot *, PackagePathRoot * _Nullable> package_nullable_path(void) {
  return @keypath(PackagePathRoot, next.value);
}

#ifdef __cplusplus
namespace PackageDependentPaths {
template<class Root> auto literal_count_path() {
  return @keypath(Root, count);
}
template<class Root> auto literal_read_count(Root root) {
  return KeyPathGet(@keypath(Root, count), root);
}
template<class Root> auto literal_checked_path() {
  return @keypath(Root, next.count);
}

template<class Path, class Root> auto read(Path path, Root root) {
  return KeyPathGet(path, root);
}
template<class Path, class Root, class Value>
void write(Path path, Root root, Value value) {
  KeyPathSet(path, root, value);
}
template<class Path> auto readonly(Path path) {
  return KeyPathAsReadOnly(path);
}
template<class Path> auto metadata(Path path) {
  return KeyPathMetadata(path);
}
template<class Path, class... Roots> auto packed_read(Path path, Roots... roots) {
  return KeyPathGet(path, roots...);
}
template<class... Arguments> auto all_read(Arguments... arguments) {
  return KeyPathGet(arguments...);
}
template<class... Arguments> void all_write(Arguments... arguments) {
  KeyPathSet(arguments...);
}
template<class Path, class Root> void braced_write(Path path, Root root) {
  KeyPathSet(path, {root}, {13, 17});
}
}
#endif

WritableKeyPath<PackagePathRoot *, int> package_checked_scalar_path(void) {
  return @keypath(PackagePathRoot, next.count);
}
WritableKeyPath<PackagePathRoot *, PackagePathPair> package_checked_record_path(void) {
  return @keypath(PackagePathRoot, next.pair);
}
inline int package_checked_read(PackagePathRoot *_Nonnull root) {
  return KeyPathGet(@keypath(PackagePathRoot, next.count), root);
}
