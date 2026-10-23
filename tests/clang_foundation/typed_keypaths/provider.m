#ifndef TYPED_KEYPATH_RUNTIME_PCH
#include "runtime.h"
#endif

int path_child_reads, path_count_reads, path_count_sets;
int path_copies, path_destroyed;

@implementation RuntimePathNode
@synthesize copied = _copied, pair = _pair;
#if __has_feature(objc_arc)
@synthesize observer = _observer;
#endif
+ (instancetype)newWithCount:(int)value {
  RuntimePathNode *node = class_createInstance(self, 0);
  node->_count = value;
  return node;
}
- (int)readCount { ++path_count_reads; return _count; }
- (void)writeCount:(int)value { ++path_count_sets; _count = value; }
- (int)identity { return _count + 100; }
- (RuntimePathNode *)readChild { ++path_child_reads; return _child; }
- (void)writeChild:(RuntimePathNode *)child {
#if __has_feature(objc_arc)
  _child = child;
#else
  child = objc_retain(child);
  objc_release(_child);
  _child = child;
#endif
}
- (id)copyWithZone:(void *)zone {
  (void)zone;
  ++path_copies;
  return [RuntimePathNode newWithCount:_count];
}
- (id)copy { return [self copyWithZone:0]; }
- (WritableKeyPath<RuntimePathNode *, int>)selfPath {
  return @keypath(self, count);
}
- (void)dealloc {
  ++path_destroyed;
#if !__has_feature(objc_arc)
  objc_release(_child);
  objc_release(_copied);
#endif
  object_dispose(self);
}
@end

WritableKeyPath<RuntimePathNode *, int> runtime_nested_path(void) {
  return @keypath(RuntimePathNode, child.count);
}
KeyPath<RuntimePathNode *, int> runtime_identity_path(void) {
  return @keypath(RuntimePathNode, identity);
}
WritableKeyPath<RuntimePathNode *, PathPair> runtime_pair_path(void) {
  return @keypath(RuntimePathNode, pair);
}
