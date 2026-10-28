#ifndef TYPED_KEYPATH_RUNTIME_PCH
#include "runtime.h"
#endif

int path_child_reads, path_count_reads, path_count_sets;
int path_copies, path_destroyed;
int path_pair_reads, path_pair_sets;
int path_bits_reads, path_bits_sets, path_inherited_reads, path_inherited_sets;
int path_throw_pair_getter, path_throw_pair_setter;
int path_optional_reads, path_value_reads, path_callback_reads;
int path_throw_optional_value, path_throw_optional_setter;
int path_value_sets, path_callback_sets, path_route_reads, path_route_sets;
__unsafe_unretained RuntimePathNode *path_clear_optional_parent;

@implementation RuntimePathNode
@synthesize copied = _copied;
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
- (PathRoute)route { ++path_route_reads; return _route; }
- (void)setRoute:(PathRoute)value { ++path_route_sets; _route = value; }
- (PathPair)pair {
  ++path_pair_reads;
  if (path_throw_pair_getter) @throw self;
  return _pair;
}
- (void)setPair:(PathPair)value {
  ++path_pair_sets;
  if (path_throw_pair_setter) @throw self;
  _pair = value;
}
- (PathBits)bits { ++path_bits_reads; return _bits; }
- (void)setBits:(PathBits)value { ++path_bits_sets; _bits=value; }
#ifdef __cplusplus
- (PathInherited)inherited { ++path_inherited_reads; return _inherited; }
- (void)setInherited:(PathInherited)value { ++path_inherited_sets; _inherited=value; }
#endif
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
- (RuntimePathNode *)readOptionalNext {
  ++path_optional_reads;
  return _optionalNext;
}
- (void)writeOptionalNext:(RuntimePathNode *)value {
#if __has_feature(objc_arc)
  _optionalNext = value;
#else
  value = objc_retain(value);
  objc_release(_optionalNext);
  _optionalNext = value;
#endif
}
- (RuntimePathNode *)readOptionalValue {
  ++path_value_reads;
  if (path_throw_optional_value) @throw self;
  return _optionalValue;
}
- (void)writeOptionalValue:(RuntimePathNode *)value {
  ++path_value_sets;
  if (path_clear_optional_parent) {
    path_clear_optional_parent.optionalNext = nil;
    path_clear_optional_parent = nil;
  }
  if (path_throw_optional_setter) @throw self;
#if __has_feature(objc_arc)
  _optionalValue = value;
#else
  value = objc_retain(value);
  objc_release(_optionalValue);
  _optionalValue = value;
#endif
}
- (PathCallback)readCallback { ++path_callback_reads; return _callback; }
- (void)writeCallback:(PathCallback)value {
  ++path_callback_sets;
#if __has_feature(objc_arc)
  _callback = value;
#else
  PathCallback copy = Block_copy(value);
  Block_release(_callback);
  _callback = copy;
#endif
}
- (RuntimePathNode *)ownedValue {
  return [RuntimePathNode newWithCount:_count];
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
  objc_release(_optionalNext);
  objc_release(_optionalValue);
  Block_release(_callback);
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

WritableKeyPath<RuntimePathNode *, unsigned> runtime_bits_path(void) {
  return @keypath(RuntimePathNode, bits.value);
}
#ifdef __cplusplus
WritableKeyPath<RuntimePathNode *, long> runtime_inherited_path(void) {
  return @keypath(RuntimePathNode, inherited.right);
}
#endif
