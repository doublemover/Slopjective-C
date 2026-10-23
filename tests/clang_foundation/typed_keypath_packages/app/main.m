@import objc3.keypath;
@import fixture.TypedKeyPaths;
#include <assert.h>
#include <string.h>
int main(void) {
  @autoreleasepool {
    PackagePathRoot *root = [PackagePathRoot new];
    WritableKeyPath<PackagePathRoot *, int> path = package_count_path();
    KeyPathSet(path, root, 42);
    assert(KeyPathGet(path, root) == 42);
    assert(KeyPathGet(package_identity_path(), root) == 142);
    const objc3_keypath_descriptor_v2 *metadata = KeyPathMetadata(path);
    assert(objc3_keypath_validate_v2(metadata) == 0);
    assert(strstr(metadata->root_type, "objc-runtime=") != 0);
    assert(!strcmp(metadata->path, "count"));
  }
}
