#pragma once
// Public source and PCH witness. Compile main.m as Objective-C C17 or
// Objective-C++ C++20/C++23, with ARC or MRC, against GNUstep 2.2.
static int capture_failures, capture_closed, capture_close_sum;
#define CAPTURE_CHECK(value) do { if (!(value)) ++capture_failures; } while (0)
typedef int (^CaptureIntBlock)(void);
static void capture_close(int value) __attribute__((nothrow));
static void capture_close(int value) {
  ++capture_closed;
  capture_close_sum += value;
}
static void capture_close_address(int *value) __attribute__((nothrow));
static void capture_close_address(int *value) {
  capture_close(*value);
  *value = -1;
}
#if !__has_feature(objc_arc)
#ifdef __cplusplus
extern "C"
#endif
void _Block_release(const void *);
#endif
__attribute__((objc_root_class))
@interface CaptureObject {
  Class isa;
@public
  int value;
#if !__has_feature(objc_arc)
  unsigned references;
#endif
}
+ (instancetype)make:(int)value __attribute__((ns_returns_retained));
- (id)copy;
#if !__has_feature(objc_arc)
- (id)retain;
- (oneway void)release;
#endif
@end
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
static int capture_created, capture_destroyed;
@implementation CaptureObject
+ (instancetype)make:(int)number {
  CaptureObject *object = class_createInstance(self, 0);
  object->value = number;
#if !__has_feature(objc_arc)
  object->references = 1;
#endif
  ++capture_created;
  return object;
}
- (id)copy { return self; }
#if !__has_feature(objc_arc)
- (id)retain { ++references; return self; }
- (oneway void)release { if (--references == 0) [self dealloc]; }
#endif
- (void)dealloc { ++capture_destroyed; object_dispose(self); }
@end

static void capture_scalar_suite(void) {
  int source = 42;
  CaptureIntBlock moved = ^[move source] { return source; };
  CAPTURE_CHECK(source == 0 && moved() == 42 && moved() == 42);
  int snapshot = 9;
  CaptureIntBlock copied = ^[snapshot] { return snapshot; };
  snapshot = 100;
  CAPTURE_CHECK(copied() == 9);
  __block int byref = 7;
  CaptureIntBlock explicit_snapshot = ^[strong byref] { return byref; };
  CaptureIntBlock implicit_reference = ^{ return byref; };
  byref = 19;
  CAPTURE_CHECK(explicit_snapshot() == 7 && implicit_reference() == 19);
  int strong = 1, weak = 2, unowned = 3, move = 4;
  CaptureIntBlock contextual = ^[strong, weak, unowned, move] {
    return strong + weak + unowned + move;
  };
  CAPTURE_CHECK(contextual() == 10);
}

static void capture_object_suite(void) {
  int destroyed_before = capture_destroyed;
  {
    CaptureObject *source = [CaptureObject make:31];
    CaptureIntBlock snapshot = ^[strong source] { return source->value; };
#if __has_feature(objc_arc)
    source = (id)0;
#else
    [source release];
    source = (id)0;
#endif
    CAPTURE_CHECK(capture_destroyed == destroyed_before && snapshot() == 31);
  }
  CAPTURE_CHECK(capture_destroyed == destroyed_before + 1);
  {
    CaptureObject *source = [CaptureObject make:32];
#if !__has_feature(objc_arc)
    // MRC pointer storage has no tracked +1. Preserve the manual owner while
    // the move snapshots the +0 pointer and clears its source variable.
    CaptureObject *manual_owner = source;
#endif
    CaptureIntBlock moved = ^[move source] { return source->value; };
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(source == (id)0 && moved() == 32);
  }
  CAPTURE_CHECK(capture_destroyed == destroyed_before + 2);
  {
    CaptureObject *source = [CaptureObject make:33];
    CaptureIntBlock unowned = ^[unowned source] { return source->value; };
    CAPTURE_CHECK(unowned() == 33);
#if !__has_feature(objc_arc)
    [source release];
#endif
    // No invocation after the unsafe capture's referent dies.
  }
  CAPTURE_CHECK(capture_destroyed == destroyed_before + 3);
#if __has_feature(objc_arc)
  {
    CaptureObject *source = [CaptureObject make:34];
    CaptureObject * __weak weak_source = source;
    int (^weak_block)(void) = [^[weak source] {
      return source ? source->value : -1;
    } copy];
    int (^promoted)(void) = [^[strong weak_source] {
      return weak_source ? weak_source->value : -1;
    } copy];
    source = (id)0;
    CAPTURE_CHECK(weak_block() == 34 && promoted() == 34);
    promoted = (id)0;
    CAPTURE_CHECK(weak_block() == -1 && weak_source == (id)0);
  }
  CAPTURE_CHECK(capture_destroyed == destroyed_before + 4);
#endif
}

static CaptureIntBlock capture_make_resource(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 7;
  CaptureIntBlock stack = ^[move handle] { return handle; };
  CAPTURE_CHECK(handle == -1 && capture_closed == 0);
  return [stack copy];
}
static void capture_resource_suite(void) {
  capture_closed = capture_close_sum = 0;
  CaptureIntBlock first = capture_make_resource();
  CaptureIntBlock second = [first copy];
  CAPTURE_CHECK(first() == 7 && second() == 7 && capture_closed == 0);
#if __has_feature(objc_arc)
  first = (id)0;
#else
  _Block_release(first);
#endif
  CAPTURE_CHECK(second() == 7 && capture_closed == 0);
#if __has_feature(objc_arc)
  second = (id)0;
#else
  _Block_release(second);
#endif
  CAPTURE_CHECK(capture_closed == 1 && capture_close_sum == 7);
  {
    int handle __attribute__((objc_resource(close=capture_close_address,
                                             invalid=-1))) = 9;
    CaptureIntBlock immediate = ^[move handle] { return handle; };
    CAPTURE_CHECK(handle == -1 && immediate() == 9 && capture_closed == 1);
  }
  CAPTURE_CHECK(capture_closed == 2 && capture_close_sum == 16);
  {
    int absent __attribute__((objc_resource(close=capture_close, invalid=-1))) = -1;
    CaptureIntBlock empty = ^[move absent] { return absent; };
    CAPTURE_CHECK(empty() == -1);
  }
  CAPTURE_CHECK(capture_closed == 2);
}

#ifdef __cplusplus
struct CaptureOrder {
  int *source;
  int observed;
  explicit CaptureOrder(int *source) : source(source), observed(-1) {}
  CaptureOrder(const CaptureOrder &other)
      : source(other.source), observed(*other.source) {}
};
template<class T> static int capture_template(T value) {
  auto closure = ^[move value] { return value; };
  CAPTURE_CHECK(value == 0);
  return closure();
}
static void capture_cpp_suite(void) {
  int source = 23;
  CaptureOrder later(&source);
  auto ordered = ^[move source, later] {
    return source + later.observed;
  };
  CAPTURE_CHECK(source == 0 && ordered() == 23);
  CAPTURE_CHECK(capture_template(42) == 42);
}
#endif
static void capture_suite(void) {
  capture_scalar_suite();
  capture_object_suite();
  capture_resource_suite();
#ifdef __cplusplus
  capture_cpp_suite();
#endif
  CAPTURE_CHECK(capture_created == capture_destroyed);
}
