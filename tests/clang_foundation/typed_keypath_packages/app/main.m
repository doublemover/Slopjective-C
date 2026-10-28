@import objc3.keypath;
@import fixture.TypedKeyPaths;
#include <assert.h>
#include <string.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>
int main(void) {
  unsigned initial_roots = package_live_roots();
  @autoreleasepool {
    PackagePathRoot *root = [PackagePathRoot new];
    assert(package_live_roots() == initial_roots + 1);
    WritableKeyPath<PackagePathRoot *, PackagePathRoot * _Nullable> nullable_path = package_nullable_path();
    KeyPathSet(nullable_path, root, root);
    assert(root.value == nil);
    assert((root[nullable_path] = root) == root);
    PackagePathRoot *child = [PackagePathRoot new];
    root.next = child;
    KeyPathSet(nullable_path, root, root);
    assert(child.value == root);
    const objc3_keypath_descriptor_v2 *nullable_metadata = KeyPathMetadata(nullable_path);
    assert(objc3_keypath_validate_v2(nullable_metadata) == 0);
    assert(nullable_metadata->flags & OBJC3_KEYPATH_NULLABLE_WRITES);
    int nullable_equal = 0;
    assert(objc3_keypath_equal_v2(nullable_metadata,
        KeyPathMetadata(@keypath(PackagePathRoot, next.value)), &nullable_equal) == 0 && nullable_equal);
    root[nullable_path] = nil;
    root.next = nil;
#if !__has_feature(objc_arc)
    objc_release(child);
#endif
    WritableKeyPath<PackagePathRoot *, int> path = package_count_path();
    KeyPathSet(path, root, 42);
    assert(KeyPathGet(path, root) == 42);
    KeyPath<PackagePathRoot *, int> read = path;
    assert(KeyPathGet(read, root) == 42);
    assert(KeyPathGet(package_narrowed_path(), root) == 42);
    assert(KeyPathGet(package_identity_path(), root) == 142);
    assert(root[read]==42);
    assert(package_indexed_update(root,path,10)==48);
    assert(root[path]==14);
    assert((root[path]=42)==42);
    const objc3_keypath_descriptor_v2 *metadata = KeyPathMetadata(path);
    assert(objc3_keypath_validate_v2(metadata) == 0);
    assert(strstr(metadata->root_type, "objc-runtime=") != 0);
    assert(!strcmp(metadata->path, "count"));
    const objc3_keypath_descriptor_v2 *local =
        KeyPathMetadata(@keypath(PackagePathRoot, count));
    int equal = 0;
    assert(objc3_keypath_equal_v2(metadata, local, &equal) == 0 && equal);
    assert(objc3_keypath_equal_v2(metadata,
        KeyPathMetadata(package_identity_path()), &equal) == 0 && !equal);
    PackagePathPair pair = {1L, 17L};
    root.pair = pair;
    KeyPathSet(package_field_path(), root, 42L);
    pair = root.pair;
    assert(pair.first == 42L && pair.second == 17L);
    PackagePathBits bits={1,-9,91};
    root.bits=bits;
    KeyPathSet(package_bits_path(),root,10U);
    bits=root.bits;
    assert(bits.value==2U && bits.small==-9 && bits.sibling==91);
    assert(KeyPathGet(package_bits_path(),root)==2U);
    assert(objc3_keypath_equal_v2(KeyPathMetadata(package_bits_path()),
        KeyPathMetadata(@keypath(PackagePathRoot,bits.value)),&equal)==0 && equal);
#if !__has_feature(objc_arc)
    objc_release(root);
#endif
  }
  assert(package_live_roots() == initial_roots);
}
