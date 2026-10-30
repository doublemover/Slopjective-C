#pragma once
// Public source and PCH witness. Compile main.m as Objective-C C17 or
// Objective-C++ C++20/C++23, with ARC or MRC, against GNUstep 2.2.
static int capture_failures, capture_closed, capture_close_sum;
#include <stdio.h>
#define CAPTURE_CHECK(value) do { if (!(value)) { ++capture_failures; fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #value); } } while (0)
typedef int (^CaptureIntBlock)(void);
#ifdef __cplusplus
extern "C" {
#endif
void *_Block_copy(const void *);
#ifdef __cplusplus
}
#endif
// libobjc supplies the Blocks C ABI without Foundation's -copy methods.
static CaptureIntBlock capture_heap_copy(CaptureIntBlock value)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_heap_copy(CaptureIntBlock value) {
#if __has_feature(objc_arc)
  return (__bridge_transfer CaptureIntBlock)_Block_copy((__bridge const void *)value);
#else
  return (CaptureIntBlock)_Block_copy(value);
#endif
}

typedef void (^CaptureVoidBlock)(void);
static CaptureVoidBlock capture_heap_copy_void(CaptureVoidBlock value)
    __attribute__((ns_returns_retained));
static CaptureVoidBlock capture_heap_copy_void(CaptureVoidBlock value) {
#if __has_feature(objc_arc)
  return (__bridge_transfer CaptureVoidBlock)_Block_copy((__bridge const void *)value);
#else
  return (CaptureVoidBlock)_Block_copy(value);
#endif
}

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
    CaptureIntBlock weak_block, nested;
    CaptureObject * __weak weak_source;
    @autoreleasepool {
    CaptureObject *source = [CaptureObject make:34];
    weak_source = source;
    weak_block = capture_heap_copy(^[weak source] {
      CaptureObject *snapshot = source;
      return snapshot ? snapshot->value : -1;
    });
    int (^promoted)(void) = capture_heap_copy(^[strong weak_source] {
      return weak_source ? weak_source->value : -1;
    });
    nested = capture_heap_copy(^[weak source] {
      CaptureIntBlock inner = ^[strong source] {
        CaptureObject *snapshot = source;
      return snapshot ? snapshot->value : -1;
      };
      return inner();
    });
    source = (id)0;
    CAPTURE_CHECK(weak_block() == 34 && promoted() == 34 && nested() == 34);
    promoted = (id)0;
    } // Destroy lexical capture owners and drain temporary weak loads.
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

// Plain carriers need no managed copy/dispose hooks. Their tags survive move,
// including inactive nested arms; only the selected scalar payload is cleared.
static void capture_plain_carrier_move_suite(void) {
  for (int present = 0; present != 2; ++present) {
    Optional(int) source = present ? Some(int, 76) : None(int);
    CaptureIntBlock __unsafe_unretained snapshot = ^[source] {
      guard let value = source else { return -1; }
      return value;
    };
    CaptureIntBlock __unsafe_unretained moved = ^[move source] {
      guard let value = source else { return -1; }
      return value;
    };
    CAPTURE_CHECK(__objc3_has_value(source) == present);
    if (present) {
      guard let zero = source else { ++capture_failures; return; }
      CAPTURE_CHECK(zero == 0);
    }
    CaptureIntBlock first = capture_heap_copy(moved);
    CaptureIntBlock second = capture_heap_copy(first);
    CAPTURE_CHECK(snapshot() == (present ? 76 : -1));
    CAPTURE_CHECK(first() == snapshot() && second() == snapshot());
#if !__has_feature(objc_arc)
    _Block_release(first);
    _Block_release(second);
#endif
  }
  for (int success = 0; success != 2; ++success) {
    __objc3_result(int, int) source = success ? __objc3_ok(int, int, 77)
                                            : __objc3_err(int, int, 78);
    CaptureIntBlock __unsafe_unretained moved = ^[move source] {
      return __objc3_match (source) {
        case .Ok(let value) => value;
        case .Err(let error) => -error;
      };
    };
    CAPTURE_CHECK(__objc3_is_ok(source) == success);
    int zero = __objc3_match (source) {
      case .Ok(let value) => value;
      case .Err(let error) => error;
    };
    CAPTURE_CHECK(zero == 0 && moved() == (success ? 77 : -78));
    CaptureIntBlock heap = capture_heap_copy(moved);
    CAPTURE_CHECK(heap() == moved());
#if !__has_feature(objc_arc)
    _Block_release(heap);
#endif
  }
  for (int present = 0; present != 2; ++present) {
    Optional(int) inner = present ? Some(int, 79) : None(int);
    Optional(Optional(int)) source = Some(Optional(int), inner);
    CaptureIntBlock __unsafe_unretained moved = ^[move source] {
      guard let nested = source else { return -2; }
      guard let value = nested else { return -1; }
      return value;
    };
    guard let nested_zero = source else { ++capture_failures; return; }
    CAPTURE_CHECK(__objc3_has_value(nested_zero) == present);
    if (present) {
      guard let zero = nested_zero else { ++capture_failures; return; }
      CAPTURE_CHECK(zero == 0);
    }
    CaptureIntBlock heap = capture_heap_copy(moved);
    CAPTURE_CHECK(heap() == (present ? 79 : -1));
#if !__has_feature(objc_arc)
    _Block_release(heap);
#endif
  }
}

static int capture_native_function(int value) { return value + 2; }
typedef int (*CaptureNativeFunction)(int);
typedef __complex__ double CaptureNativeComplex;
struct CaptureNativeRecord { int values[2]; int *pointer; };
static void capture_plain_native_carrier_suite(void) {
  int value = 94;
  Optional(int *) source = Some(int *, &value);
  CaptureIntBlock __unsafe_unretained stack = ^[move source] {
    guard let pointer = source else { return -1; }
    return pointer ? *pointer : 0;
  };
  guard let zero = source else { ++capture_failures; return; }
  CAPTURE_CHECK(zero == (int *)0 && stack() == 94);
  CaptureIntBlock heap = capture_heap_copy(stack);
  CAPTURE_CHECK(heap() == 94);
#if !__has_feature(objc_arc)
  _Block_release(heap);
#endif
  {
    Optional(CaptureNativeFunction) function = Some(CaptureNativeFunction, capture_native_function);
    CaptureIntBlock __unsafe_unretained moved = ^[move function] {
      guard let call = function else { return -1; }
      return call(93);
    };
    guard let null_function = function else { ++capture_failures; return; }
    CAPTURE_CHECK(null_function == (CaptureNativeFunction)0 && moved() == 95);
    CaptureIntBlock heap_function = capture_heap_copy(moved);
    CAPTURE_CHECK(heap_function() == 95);
#if !__has_feature(objc_arc)
    _Block_release(heap_function);
#endif
  }
  {
    CaptureNativeComplex number;
    __real__ number = 47; __imag__ number = 49;
    Optional(CaptureNativeComplex) complex_source = Some(CaptureNativeComplex, number);
    CaptureIntBlock __unsafe_unretained moved = ^[move complex_source] {
      guard let selected = complex_source else { return -1; }
      return (int)(__real__ selected + __imag__ selected);
    };
    guard let complex_zero = complex_source else { ++capture_failures; return; }
    CAPTURE_CHECK(__real__ complex_zero == 0 && __imag__ complex_zero == 0);
    CaptureIntBlock complex_heap = capture_heap_copy(moved);
    CAPTURE_CHECK(complex_heap() == 96);
#if !__has_feature(objc_arc)
    _Block_release(complex_heap);
#endif
  }
  for (int success = 0; success != 2; ++success) {
    CaptureNativeComplex number;
    __real__ number = 47; __imag__ number = 49;
    __objc3_result(CaptureNativeComplex, CaptureNativeComplex) result = success
        ? __objc3_ok(CaptureNativeComplex, CaptureNativeComplex, number)
        : __objc3_err(CaptureNativeComplex, CaptureNativeComplex, number);
    CaptureIntBlock __unsafe_unretained moved = ^[move result] {
      return __objc3_match (result) {
        case .Ok(let selected) => (int)(__real__ selected + __imag__ selected);
        case .Err(let selected) => -(int)(__real__ selected + __imag__ selected);
      };
    };
    CAPTURE_CHECK(__objc3_is_ok(result) == success);
    int zero = __objc3_match (result) {
      case .Ok(let selected) => (int)(__real__ selected + __imag__ selected);
      case .Err(let selected) => (int)(__real__ selected + __imag__ selected);
    };
    CaptureIntBlock result_heap = capture_heap_copy(moved);
    CAPTURE_CHECK(zero == 0 && result_heap() == (success ? 96 : -96));
#if !__has_feature(objc_arc)
    _Block_release(result_heap);
#endif
  }
#ifndef __cplusplus
  {
    struct CaptureNativeRecord record = {{47, 50}, &value};
    Optional(struct CaptureNativeRecord) aggregate = Some(struct CaptureNativeRecord, record);
    CaptureIntBlock __unsafe_unretained moved = ^[move aggregate] {
      guard let selected = aggregate else { return -1; }
      return selected.values[0] + selected.values[1];
    };
    guard let empty = aggregate else { ++capture_failures; return; }
    CAPTURE_CHECK(empty.values[0] == 0 && empty.values[1] == 0 && empty.pointer == (int *)0);
    CaptureIntBlock aggregate_heap = capture_heap_copy(moved);
    CAPTURE_CHECK(aggregate_heap() == 97);
#if !__has_feature(objc_arc)
    _Block_release(aggregate_heap);
#endif
  }
#endif
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
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(heap() == 74 && capture_destroyed == before);
    heap = (id)0;
  }
  CAPTURE_CHECK(capture_destroyed == before + 1);
  {
    CaptureObject *owner = [CaptureObject make:75];
    Optional(id __weak) source = Some(id __weak, owner);
    CaptureIntBlock weak = capture_heap_copy(^[move source] {
      guard let object = source else { return -1; }
      return object ? ((CaptureObject *)object)->value : -2;
    });
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
  return capture_heap_copy(stack);
}
static void capture_resource_suite(void) {
  capture_closed = capture_close_sum = 0;
  CaptureIntBlock first = capture_make_resource();
  CaptureIntBlock second = capture_heap_copy(first);
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
    left = capture_heap_copy(stack);
    right = capture_heap_copy(stack);
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
    left = capture_heap_copy(stack);
    right = capture_heap_copy(stack);
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

static void capture_nested_resource_suite(void) {
  int before = capture_closed;
  __block CaptureIntBlock inner = (id)0;
  __block int transfers = 0;
  void (^left)(void) = (id)0, (^right)(void) = (id)0;
  {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 128;
    void (^__unsafe_unretained stack)(void) = ^[move handle] {
      if (handle != -1) {
        CaptureIntBlock __unsafe_unretained next = ^[move handle] {
          return handle;
        };
        inner = capture_heap_copy(next);
        ++transfers;
      }
    };
    left = capture_heap_copy_void(stack);
    right = capture_heap_copy_void(stack);
    CAPTURE_CHECK(handle == -1 && left != right);
  }
  left();
  right();
  CAPTURE_CHECK(transfers == 1 && inner() == 128 && capture_closed == before);
#if __has_feature(objc_arc)
  left = right = (id)0;
#else
  _Block_release(left);
  _Block_release(right);
#endif
  // Both old copies now own an empty payload; the nested closure has the one
  // live cleanup obligation in its new owner, independent of their lifetime.
  CAPTURE_CHECK(inner() == 128 && capture_closed == before);
#if __has_feature(objc_arc)
  inner = (id)0;
#else
  _Block_release(inner);
  inner = (id)0;
#endif
  CAPTURE_CHECK(capture_closed == before + 1);
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
    CaptureIntBlock heap = capture_heap_copy(stack);
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
static void capture_cpp_carrier_move_suite(void) {
  CaptureMoveRecord::moves = CaptureMoveRecord::copies = 0;
  {
    Optional(CaptureMoveRecord) source = Some(CaptureMoveRecord, CaptureMoveRecord(81));
    int moves = CaptureMoveRecord::moves, copies = CaptureMoveRecord::copies;
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      guard let value = source else { return -2; }
      return value.value;
    };
    // Observe the actual source payload, without creating a guard-binding copy.
    int source_value = __objc3_match (source) {
      case .Some(let value) => value.value;
      case .None => -2;
    };
    CAPTURE_CHECK(source_value == -1);
    // The observation can copy the selected object, so snapshot its counters.
    int before_heap_moves = CaptureMoveRecord::moves;
    int before_heap_copies = CaptureMoveRecord::copies;
    CAPTURE_CHECK(before_heap_moves == moves + 1);
    CaptureIntBlock first = capture_heap_copy(stack);
    CaptureIntBlock second = capture_heap_copy(stack);
    CAPTURE_CHECK(CaptureMoveRecord::moves == before_heap_moves &&
                  CaptureMoveRecord::copies == before_heap_copies + 2);
    CAPTURE_CHECK(first() == 81 && second() == 81);
#if __has_feature(objc_arc)
    first = second = (id)0;
#else
    _Block_release(first);
    _Block_release(second);
#endif
    (void)copies;
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    __objc3_result(CaptureMoveRecord, int) source = __objc3_ok(CaptureMoveRecord, int, CaptureMoveRecord(82));
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      return __objc3_match (source) {
        case .Ok(let value) => value.value;
        case .Err(let error) => -error;
      };
    };
    CAPTURE_CHECK(__objc3_is_ok(source) == 1);
    int after = __objc3_match (source) {
      case .Ok(let value) => value.value;
      case .Err(let error) => error;
    };
    CAPTURE_CHECK(after == (-1));
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(heap() == (82));
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    __objc3_result(CaptureMoveRecord, int) source = __objc3_err(CaptureMoveRecord, int, 83);
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      return __objc3_match (source) {
        case .Ok(let value) => value.value;
        case .Err(let error) => -error;
      };
    };
    CAPTURE_CHECK(__objc3_is_ok(source) == 0);
    int after = __objc3_match (source) {
      case .Ok(let value) => value.value;
      case .Err(let error) => error;
    };
    CAPTURE_CHECK(after == (0));
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(heap() == (-83));
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    Optional(CaptureMoveRecord) inner = Some(CaptureMoveRecord, CaptureMoveRecord(86));
    Optional(Optional(CaptureMoveRecord)) source = Some(Optional(CaptureMoveRecord), moveValue(inner));
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      guard let nested = source else { return -2; }
      guard let value = nested else { return -3; }
      return value.value;
    };
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(heap() == 86 && __objc3_has_value(source));
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    Optional(CaptureMoveRecord) source = Some(CaptureMoveRecord, CaptureMoveRecord(87));
    CaptureIntBlock __unsafe_unretained stack = ^[source] {
      guard let value = source else { return -2; }
      return value.value;
    };
    CaptureIntBlock heap = capture_heap_copy(stack);
    int original = __objc3_match (source) {
      case .Some(let value) => value.value;
      case .None => -2;
    };
    CAPTURE_CHECK(original == 87 && heap() == 87);
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    Optional(CaptureMoveRecord) absent = None(CaptureMoveRecord);
    CaptureIntBlock heap = capture_heap_copy(^[move absent] {
      return (int)__objc3_has_value(absent);
    });
    CAPTURE_CHECK(heap() == 0 && !__objc3_has_value(absent));
#if __has_feature(objc_arc)
    heap = (id)0;
#else
    _Block_release(heap);
#endif
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
}
#if __has_feature(objc_arc)
static void capture_mixed_carrier_move_suite(void) {
  using Mixed = __objc3_result(id, CaptureMoveRecord);
  int before = capture_destroyed;
  {
    CaptureObject *owner = [CaptureObject make:88];
    Mixed source = __objc3_ok(id, CaptureMoveRecord, owner);
    owner = (id)0;
    {
      defer { CAPTURE_CHECK(capture_destroyed == before); }
      CaptureIntBlock __unsafe_unretained stack = ^[move source] {
        return __objc3_match (source) {
          case .Ok(let value) => value ? ((CaptureObject *)value)->value : -1;
          case .Err(let error) => error.value;
        };
      };
      int moved_from = __objc3_match (source) {
        case .Ok(let value) => value == (id)0;
        case .Err(let error) => 0;
      };
      CAPTURE_CHECK(moved_from && stack() == 88);
      CaptureIntBlock first = capture_heap_copy(stack);
      CaptureIntBlock second = capture_heap_copy(stack);
      CAPTURE_CHECK(first() == 88 && second() == 88 && capture_destroyed == before);
      first = second = (id)0;
    }
    CAPTURE_CHECK(capture_destroyed == before + 1);
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    Mixed source = __objc3_err(id, CaptureMoveRecord, CaptureMoveRecord(89));
    {
      defer { CAPTURE_CHECK(CaptureMoveRecord::live == 1); }
      CaptureIntBlock __unsafe_unretained stack = ^[move source] {
        return __objc3_match (source) {
          case .Ok(let value) => -1;
          case .Err(let error) => error.value;
        };
      };
      int after = __objc3_match (source) {
        case .Ok(let value) => 0;
        case .Err(let error) => error.value;
      };
      CAPTURE_CHECK(after == -1 && stack() == 89);
      CaptureIntBlock heap = capture_heap_copy(stack);
      CAPTURE_CHECK(heap() == 89);
      heap = (id)0;
    }
    CAPTURE_CHECK(CaptureMoveRecord::live == 1);
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0 && capture_destroyed == before + 1);
  {
    using WeakMixed = __objc3_result(id __weak, CaptureMoveRecord);
    CaptureObject *owner = [CaptureObject make:90];
    WeakMixed source = __objc3_ok(id __weak, CaptureMoveRecord, owner);
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      return __objc3_match (source) {
        case .Ok(let value) => value ? ((CaptureObject *)value)->value : -1;
        case .Err(let error) => error.value;
      };
    };
    int moved_from = __objc3_match (source) {
      case .Ok(let value) => value == (id)0;
      case .Err(let error) => 0;
    };
    CAPTURE_CHECK(moved_from && stack() == 90);
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(heap() == 90);
    owner = (id)0;
    CAPTURE_CHECK(heap() == -1 && stack() == -1 && capture_destroyed == before + 2);
    heap = (id)0;
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    Mixed source = __objc3_err(id, CaptureMoveRecord, CaptureMoveRecord(94));
    {
      defer { CAPTURE_CHECK(CaptureMoveRecord::live == 1); }
      CaptureIntBlock __unsafe_unretained snapshot = ^{
        return __objc3_match (source) {
          case .Ok(let value) => -1;
          case .Err(let error) => error.value;
        };
      };
      CAPTURE_CHECK(snapshot() == 94);
      CaptureIntBlock heap = capture_heap_copy(snapshot);
      CAPTURE_CHECK(heap() == 94 && CaptureMoveRecord::live == 3);
      heap = (id)0;
    }
    CAPTURE_CHECK(CaptureMoveRecord::live == 1);
  }
  CAPTURE_CHECK(CaptureMoveRecord::live == 0);
  {
    CaptureObject *owner = [CaptureObject make:93];
    Mixed inner = __objc3_ok(id, CaptureMoveRecord, owner);
    Optional(Mixed) source = Some(Mixed, moveValue(inner));
    owner = (id)0;
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      guard let selected = source else { return -2; }
      return __objc3_match (selected) {
        case .Ok(let value) => value ? ((CaptureObject *)value)->value : -1;
        case .Err(let error) => error.value;
      };
    };
    CaptureIntBlock heap = capture_heap_copy(stack);
    CAPTURE_CHECK(stack() == 93 && heap() == 93);
    guard let selected = source else { ++capture_failures; return; }
    int zero = __objc3_match (selected) {
      case .Ok(let value) => value == (id)0;
      case .Err(let error) => 0;
    };
    CAPTURE_CHECK(zero);
    heap = (id)0;
  }
  CAPTURE_CHECK(capture_destroyed == before + 3 && CaptureMoveRecord::live == 0);
}
#endif
struct CaptureNativeMember {
  int value;
  int add(int delta) const { return value + delta; }
};
static void capture_member_pointer_carrier_suite(void) {
  using Data = int CaptureNativeMember::*;
  using Method = int (CaptureNativeMember::*)(int) const;
  Optional(Data) source = Some(Data, &CaptureNativeMember::value);
  Optional(Method) method = Some(Method, &CaptureNativeMember::add);
  CaptureIntBlock __unsafe_unretained moved = ^[move source, move method] {
    guard let member = source else { return -1; }
    guard let call = method else { return -2; }
    CaptureNativeMember object{98};
    return object.*member + (object.*call)(2);
  };
  guard let null_member = source else { ++capture_failures; return; }
  guard let null_method = method else { ++capture_failures; return; }
  CAPTURE_CHECK(null_member == nullptr && null_method == nullptr);
  CaptureIntBlock heap = capture_heap_copy(moved);
  CAPTURE_CHECK(heap() == 198);
#if !__has_feature(objc_arc)
  _Block_release(heap);
#endif
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
  capture_cpp_carrier_move_suite();
  capture_member_pointer_carrier_suite();
#if __has_feature(objc_arc)
  capture_mixed_carrier_move_suite();
#endif
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
  capture_plain_carrier_move_suite();
  capture_plain_native_carrier_suite();
  capture_object_suite();
  capture_defer_suite();
#if __has_feature(objc_arc)
  capture_carrier_move_suite();
#endif
  capture_resource_suite();
  capture_resource_operations_suite();
  capture_nested_resource_suite();
#ifdef __cplusplus
  capture_cpp_suite();
#endif
  CAPTURE_CHECK(capture_created == capture_destroyed);
}
