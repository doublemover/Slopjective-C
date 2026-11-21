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
    WritableKeyPath<PackagePathRoot *, int> checked_scalar = package_checked_scalar_path();
    WritableKeyPath<PackagePathRoot *, PackagePathPair> checked_record = package_checked_record_path();
    KeyPathSet(checked_scalar, root, 29);
    assert(KeyPathGet(checked_scalar, root) == 29 && package_checked_read(root) == 29);
    PackagePathPair checked_pair = {31, 37};
    KeyPathSet(checked_record, root, checked_pair);
    checked_pair = KeyPathGet(checked_record, root);
    assert(checked_pair.first == 31 && checked_pair.second == 37);
    const objc3_keypath_descriptor_v2 *checked_metadata = KeyPathMetadata(checked_scalar);
    assert(objc3_keypath_validate_v2(checked_metadata) == 0);
    assert(checked_metadata->flags & OBJC3_KEYPATH_CHECKED_MISSING_READ);

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
#ifdef __cplusplus
    assert(PackageDependentPaths::read(path, root) == 42);
    auto typed = PackageDependentPaths::typed_make<PackagePathRoot *>();
    auto typed_read = PackageDependentPaths::typed_narrow(typed);
    assert(PackageDependentPaths::typed_read(typed_read, root) == 42);
    PackageDependentPaths::TypedHolder<PackagePathRoot *, int> typed_holder{typed_read, typed};
    typed_holder.set(root, 97);
    assert(typed_holder.get(root) == 97);
    PackageDependentPaths::typed_write(typed, root, 42);

    auto literal = PackageDependentPaths::literal_count_path<PackagePathRoot *>();
    assert(PackageDependentPaths::read(literal, root) == 42);
    assert(PackageDependentPaths::literal_read_count(root) == 42);
    auto literal_checked = PackageDependentPaths::literal_checked_path<PackagePathRoot *>();
    const objc3_keypath_descriptor_v2 *literal_metadata = KeyPathMetadata(literal_checked);
    assert(objc3_keypath_validate_v2(literal_metadata) == 0);
    assert(literal_metadata->flags & OBJC3_KEYPATH_CHECKED_MISSING_READ);
    KeyPathSet(literal_checked, root, 99); // next is nil: no update.
    assert(PackageDependentPaths::literal_read_count(root) == 42);

    PackageDependentPaths::write(path, root, 71);
    auto dependent_read = PackageDependentPaths::readonly(path);
    assert(PackageDependentPaths::read(dependent_read, root) == 71);
    assert(PackageDependentPaths::packed_read(path, root) == 71);
    PackageDependentPaths::all_write(path, root, 83);
    assert(PackageDependentPaths::all_read(path, root) == 83);
    assert(PackageDependentPaths::metadata(path) == KeyPathMetadata(path));
    PackageDependentPaths::write(path, root, 42);
#endif
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
#ifdef __cplusplus
    WritableKeyPath<PackagePathRoot *, PackagePathPair> dependent_pair =
        @keypath(PackagePathRoot, pair);
    PackageDependentPaths::braced_write(dependent_pair, root);
    pair = PackageDependentPaths::all_read(dependent_pair, root);
    assert(pair.first == 13L && pair.second == 17L);
#endif
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
