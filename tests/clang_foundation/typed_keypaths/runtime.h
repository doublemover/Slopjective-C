#ifndef OBJC3_TYPED_KEYPATH_RUNTIME_FIXTURE_H
#define OBJC3_TYPED_KEYPATH_RUNTIME_FIXTURE_H
@import objc3.keypath;
#include <objc/runtime.h>
#include <objc/objc-arc.h>

typedef struct PathPair { long first, second; } PathPair;
extern int path_child_reads, path_count_reads, path_count_sets;
extern int path_copies, path_destroyed;
extern int path_pair_reads, path_pair_sets;
extern int path_throw_pair_getter, path_throw_pair_setter;

__attribute__((objc_root_class))
@interface RuntimePathNode {
  Class isa;
  int _count;
  RuntimePathNode *_child;
  RuntimePathNode *_copied;
#if __has_feature(objc_arc)
  __weak RuntimePathNode *_observer;
#endif
  PathPair _pair;
}
+ (instancetype)newWithCount:(int)count;
@property(nonatomic, getter=readCount, setter=writeCount:) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic, strong, getter=readChild, setter=writeChild:)
    RuntimePathNode * _Nonnull child;
@property(nonatomic, copy) RuntimePathNode * _Nullable copied;
@property(nonatomic) PathPair pair;
#if __has_feature(objc_arc)
@property(nonatomic, weak) RuntimePathNode * _Nullable observer;
#endif
- (id)copyWithZone:(void *)zone;
- (id)copy;
- (WritableKeyPath<RuntimePathNode *, int>)selfPath;
- (void)dealloc;
@end

WritableKeyPath<RuntimePathNode *, int> runtime_nested_path(void);
KeyPath<RuntimePathNode *, int> runtime_identity_path(void);
WritableKeyPath<RuntimePathNode *, PathPair> runtime_pair_path(void);
#endif
