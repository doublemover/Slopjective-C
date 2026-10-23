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
  int unused_source = 13;
  void (^unused)(void) = ^[move unused_source] {};
  CAPTURE_CHECK(unused_source == 0);
  (void)unused;
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
    int (^nested)(void) = [^[weak source] {
      CaptureIntBlock inner = ^[strong source] {
        return source ? source->value : -1;
      };
      return inner();
    } copy];
    source = (id)0;
    CAPTURE_CHECK(weak_block() == 34 && promoted() == 34 && nested() == 34);
    promoted = (id)0;
    CAPTURE_CHECK(weak_block() == -1 && nested() == -1 && weak_source == (id)0);
  }
  CAPTURE_CHECK(capture_destroyed == destroyed_before + 4);
#endif
}

static int capture_defer_field_scope(int exit_kind) {
  int before = capture_destroyed;
  {
    // Registration before the literal makes an ordinary LIFO field release
    // run too early. A committed implicit release must follow this action.
    defer { CAPTURE_CHECK(capture_destroyed == before); }
    CaptureObject *source = [CaptureObject make:91];
#if !__has_feature(objc_arc)
    CaptureObject *manual_owner = source;
#endif
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      return source->value;
    };
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(source == (id)0 && stack() == 91);
    if (exit_kind == 1) return stack();
    if (exit_kind == 2) goto finished;
  }
finished:
  CAPTURE_CHECK(capture_destroyed == before + 1);
  return 91;
}
static void capture_defer_suite(void) {
  for (int exit_kind = 0; exit_kind != 3; ++exit_kind) {
    int before = capture_destroyed;
    CAPTURE_CHECK(capture_defer_field_scope(exit_kind) == 91);
    CAPTURE_CHECK(capture_destroyed == before + 1);
  }
  int before = capture_destroyed;
  CaptureObject *source = [CaptureObject make:92];
  {
    // The original declaration is in the outer scope; the literal fields
    // belong to this inner source scope and its release queue.
    defer { CAPTURE_CHECK(capture_destroyed == before); }
    CaptureIntBlock __unsafe_unretained stack = ^[strong source] {
      return source->value;
    };
#if !__has_feature(objc_arc)
    [source release];
#endif
    source = (id)0;
    CAPTURE_CHECK(stack() == 92);
  }
  CAPTURE_CHECK(capture_destroyed == before + 1);
  {
    defer { CAPTURE_CHECK(capture_destroyed == before + 2); }
    CaptureObject *outer = [CaptureObject make:93];
#if !__has_feature(objc_arc)
    CaptureObject *outer_owner = outer;
#endif
    CaptureIntBlock __unsafe_unretained outer_block = ^[move outer] {
      return outer->value;
    };
#if !__has_feature(objc_arc)
    [outer_owner release];
#endif
    {
      defer { CAPTURE_CHECK(capture_destroyed == before + 1); }
      CaptureObject *inner = [CaptureObject make:94];
#if !__has_feature(objc_arc)
      CaptureObject *inner_owner = inner;
#endif
      CaptureIntBlock __unsafe_unretained inner_block = ^[move inner] {
        return inner->value;
      };
#if !__has_feature(objc_arc)
      [inner_owner release];
#endif
      withLifetime(inner_block) {
        defer { CAPTURE_CHECK(inner_block() == 94 && outer_block() == 93); }
        CAPTURE_CHECK(inner_block() == 94);
      }
    }
    CAPTURE_CHECK(capture_destroyed == before + 2 && outer_block() == 93);
    keepAlive(outer_block);
  }
  CAPTURE_CHECK(capture_destroyed == before + 3);
}

#if __has_feature(objc_arc)
static void capture_carrier_move_suite(void) {
  int before = capture_destroyed;
  {
    CaptureObject *owner = [CaptureObject make:74];
    Optional(id) source = Some(id, owner);
    owner = (id)0;
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      guard let object = source else { return -1; }
      return object ? ((CaptureObject *)object)->value : -2;
    };
    // Tags remain valid; ownership-bearing source leaves are now nil.
    guard let moved_from = source else { ++capture_failures; return; }
    CAPTURE_CHECK(moved_from == (id)0 && stack() == 74);
    CaptureIntBlock heap = [stack copy];
    CAPTURE_CHECK(heap() == 74 && capture_destroyed == before);
    heap = (id)0;
  }
  CAPTURE_CHECK(capture_destroyed == before + 1);
  {
    CaptureObject *owner = [CaptureObject make:75];
    Optional(id __weak) source = Some(id __weak, owner);
    CaptureIntBlock weak = [^[move source] {
      guard let object = source else { return -1; }
      return object ? ((CaptureObject *)object)->value : -2;
    } copy];
    guard let moved_from = source else { ++capture_failures; return; }
    CAPTURE_CHECK(moved_from == (id)0 && weak() == 75);
    owner = (id)0;
    CAPTURE_CHECK(weak() == -2 && capture_destroyed == before + 2);
  }
}
#endif

static CaptureIntBlock capture_make_resource(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 7;
  CaptureIntBlock __unsafe_unretained stack = ^[move handle] { return handle; };
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
  CaptureIntBlock left = (id)0, right = (id)0;
  {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 11;
    // Keep the original literal on the stack even under ARC so these calls
    // create distinct heap Blocks which independently reference one owner.
    CaptureIntBlock __unsafe_unretained stack = ^[move handle] { return handle; };
    left = [stack copy];
    right = [stack copy];
    CAPTURE_CHECK(left != right && left() == 11 && right() == 11);
  }
#if __has_feature(objc_arc)
  left = (id)0;
#else
  _Block_release(left);
#endif
  CAPTURE_CHECK(right() == 11 && capture_closed == 2);
#if __has_feature(objc_arc)
  right = (id)0;
#else
  _Block_release(right);
#endif
  CAPTURE_CHECK(capture_closed == 3 && capture_close_sum == 27);
}

static void capture_resource_operations_suite(void) {
  int before = capture_closed, sum = capture_close_sum;
  {
    int handle __attribute__((objc_resource(close=capture_close_address,
                                             invalid=-1))) = 81;
    void (^operate)(void) = ^[move handle] {
      reset(handle, 82);
      discard(handle);
    };
    CAPTURE_CHECK(handle == -1);
    operate();
    CAPTURE_CHECK(capture_closed == before + 2 &&
                  capture_close_sum == sum + 163);
  }
  CAPTURE_CHECK(capture_closed == before + 2);
  CaptureIntBlock left = (id)0, right = (id)0;
  {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 83;
    CaptureIntBlock __unsafe_unretained stack = ^[move handle] {
      return take(handle);
    };
    left = [stack copy];
    right = [stack copy];
    CAPTURE_CHECK(left != right && handle == -1);
  }
  // The operations address the one shared owner payload, not the original
  // source local or a private copy of the handle in each heap Block.
  int acquired = left();
  CAPTURE_CHECK(acquired == 83 && right() == -1);
  capture_close(acquired);
#if __has_feature(objc_arc)
  left = right = (id)0;
#else
  _Block_release(left);
  _Block_release(right);
#endif
  CAPTURE_CHECK(capture_closed == before + 3 &&
                capture_close_sum == sum + 246);
}

#ifdef __cplusplus
struct CaptureMoveRecord {
  inline static int moves = 0, copies = 0, live = 0;
  int value;
  explicit CaptureMoveRecord(int value) : value(value) { ++live; }
  CaptureMoveRecord(const CaptureMoveRecord &other) : value(other.value) {
    ++copies; ++live;
  }
  CaptureMoveRecord(CaptureMoveRecord &&other) noexcept : value(other.value) {
    other.value = -1;
    ++moves; ++live;
  }
  ~CaptureMoveRecord() { --live; }
};
static void capture_record_move_suite(void) {
  CaptureMoveRecord::moves = CaptureMoveRecord::copies = 0;
  {
    CaptureMoveRecord source(71);
    CaptureIntBlock __unsafe_unretained stack = ^[move source] { return source.value; };
    CAPTURE_CHECK(source.value == -1 && stack() == 71);
    CAPTURE_CHECK(CaptureMoveRecord::moves == 1 && CaptureMoveRecord::copies == 0);
    CaptureIntBlock heap = [stack copy];
    CAPTURE_CHECK(heap() == 71 && CaptureMoveRecord::moves == 1 &&
                  CaptureMoveRecord::copies == 1);
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
}
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
  capture_record_move_suite();
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
  capture_defer_suite();
#if __has_feature(objc_arc)
  capture_carrier_move_suite();
#endif
  capture_resource_suite();
  capture_resource_operations_suite();
#ifdef __cplusplus
  capture_cpp_suite();
#endif
  CAPTURE_CHECK(capture_created == capture_destroyed);
}
