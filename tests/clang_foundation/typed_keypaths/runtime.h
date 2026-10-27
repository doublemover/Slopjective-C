#ifndef OBJC3_TYPED_KEYPATH_RUNTIME_FIXTURE_H
#define OBJC3_TYPED_KEYPATH_RUNTIME_FIXTURE_H
@import objc3.keypath;
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <objc/blocks_runtime.h>

typedef struct PathPair { long first, second; } PathPair;
typedef struct PathBits { unsigned value:3; signed small:5; unsigned sibling:7; } PathBits;
#ifdef __cplusplus
struct PathLeft { int left; };
struct PathRight { long right; };
struct PathInherited : PathLeft, PathRight { int tail; };
#endif
typedef int (^PathCallback)(void);
extern int path_child_reads, path_count_reads, path_count_sets;
extern int path_copies, path_destroyed;
extern int path_pair_reads, path_pair_sets;
extern int path_bits_reads, path_bits_sets, path_inherited_reads, path_inherited_sets;
extern int path_throw_pair_getter, path_throw_pair_setter;
extern int path_optional_reads, path_value_reads, path_callback_reads;
extern int path_throw_optional_value;

__attribute__((objc_root_class))
@interface RuntimePathNode {
  Class isa;
  int _count;
  RuntimePathNode *_child;
  RuntimePathNode *_copied;
  RuntimePathNode *_optionalNext;
  RuntimePathNode *_optionalValue;
  PathCallback _callback;
#if __has_feature(objc_arc)
  __weak RuntimePathNode *_observer;
#endif
  PathPair _pair;
  PathBits _bits;
#ifdef __cplusplus
  PathInherited _inherited;
#endif
}
+ (instancetype)newWithCount:(int)count;
@property(nonatomic, getter=readCount, setter=writeCount:) int count;
@property(nonatomic, readonly) int identity;
@property(nonatomic, strong, getter=readChild, setter=writeChild:)
    RuntimePathNode * _Nonnull child;
@property(nonatomic, copy) RuntimePathNode * _Nullable copied;
@property(nonatomic) PathPair pair;
@property(nonatomic) PathBits bits;
#ifdef __cplusplus
@property(nonatomic) PathInherited inherited;
#endif
@property(nonatomic, strong, getter=readOptionalNext, setter=writeOptionalNext:)
    RuntimePathNode * _Nullable optionalNext;
@property(nonatomic, strong, getter=readOptionalValue, setter=writeOptionalValue:)
    RuntimePathNode * _Nullable optionalValue;
@property(nonatomic, copy, getter=readCallback, setter=writeCallback:)
    PathCallback _Nullable callback;
@property(nonatomic, readonly, getter=ownedValue)
    RuntimePathNode * _Nullable produced;
- (RuntimePathNode * _Nullable)ownedValue __attribute__((ns_returns_retained));
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
WritableKeyPath<RuntimePathNode *, unsigned> runtime_bits_path(void);
#ifdef __cplusplus
WritableKeyPath<RuntimePathNode *, long> runtime_inherited_path(void);
#endif
#endif
