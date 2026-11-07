// Pending native execution witness; compile separately with provider.m under
// C++20/C++23, ARC and MRC, Blocks and native exceptions enabled.
#ifndef TYPED_KEYPATH_RUNTIME_PCH
#include "runtime.h"
#endif
#include <cassert>
#include "cxx_values.h"

struct SnapshotCapture {
  static inline bool fail_copy = false;
  static inline int live = 0;
  int value;
  explicit SnapshotCapture(int value) : value(value) { ++live; }
  SnapshotCapture(const SnapshotCapture &other) : value(other.value) {
    if (fail_copy) throw 17;
    ++live;
  }
  ~SnapshotCapture() { --live; }
};

int main() {
  keypath_cxx_value_contract();
  const bool modes[]={false,true};
  for (bool subscript : modes) {
  @autoreleasepool {
    SnapshotCapture captured(42);
    PathCallback __unsafe_unretained stack = ^{ return captured.value; };
    const int stack_live = SnapshotCapture::live;
    RuntimePathNode *owner = [RuntimePathNode newWithCount:1];
    const int destroyed = path_destroyed;
#if __has_feature(objc_arc)
    __weak RuntimePathNode *witness = owner;
#endif
    SnapshotCapture::fail_copy = true;
    bool failed = false;
    try {
      if (subscript) {
        owner[@keypath(RuntimePathNode,callback)]=
#if __has_feature(objc_arc)
            (owner=nil,stack);
#else
            (objc_release(owner),stack);
#endif
      } else KeyPathSet(@keypath(RuntimePathNode, callback), owner,
#if __has_feature(objc_arc)
          (owner = nil, stack)
#else
          (objc_release(owner), stack)
#endif
      );
    } catch (int value) {
      assert(value == 17);
      failed = true;
    }
    SnapshotCapture::fail_copy = false;
    assert(failed);
    assert(SnapshotCapture::live == stack_live);
    assert(path_destroyed == destroyed + 1);
#if __has_feature(objc_arc)
    assert(witness == nil);
#endif
  }
  }
  assert(SnapshotCapture::live == 0);
}
