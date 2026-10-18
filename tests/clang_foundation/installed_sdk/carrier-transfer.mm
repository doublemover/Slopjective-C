// Installed Objective-C++ witness for explicit native ARC carrier transfer.
#include <objc/runtime.h>

static int destroyed;
@protocol Error @end
__attribute__((objc_root_class))
@interface TransferProbe <Error> { Class isa; }
+ (id)new;
@end
@implementation TransferProbe
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end

using Strong = __objc3_optional(id);
using Weak = __objc3_optional(id __weak);
using Unsafe = __objc3_optional(id __unsafe_unretained);
using Outcome = __objc3_result(id, id);
using Action = void (^)(void);
using OptionalAction = __objc3_optional(Action);

static bool copy_and_move_strong() {
  int before = destroyed;
  __weak id observer = nil;
  {
    id object = [TransferProbe new];
    observer = object;
    Strong source = __objc3_some(id, object);
    object = nil;
    {
      const Strong copy = __objc3_copy(source);
      Strong moved_const = __objc3_move(copy);
      Strong another = __objc3_move(moved_const);
      if (!__objc3_has_value(copy) || !__objc3_has_value(moved_const) ||
          !__objc3_has_value(another)) return false;
    }
    if (observer == nil || destroyed != before) return false;
  }
  return observer == nil && destroyed == before + 1;
}

static bool copy_and_move_weak() {
  int before = destroyed;
  __weak id observer = nil;
  {
    id object = [TransferProbe new];
    observer = object;
    Weak source = __objc3_some(id __weak, object);
    Weak copy = __objc3_copy(source);
    Weak moved = __objc3_move(copy);
    object = nil;
    if (observer != nil || !__objc3_has_value(source) ||
        !__objc3_has_value(copy) || !__objc3_has_value(moved)) return false;
    Weak zeroed = __objc3_copy(moved);
    if (!__objc3_has_value(zeroed)) return false;
  }
  return destroyed == before + 1;
}

static bool copy_and_move_result(bool success) {
  int before = destroyed;
  __weak id observer = nil;
  {
    id object = [TransferProbe new];
    observer = object;
    if (success) {
      Outcome source = __objc3_ok(id, id, object);
      Outcome copy = __objc3_copy(source);
      Outcome moved = __objc3_move(copy);
      object = nil;
      if (!__objc3_is_ok(copy) || !__objc3_is_ok(moved) || observer == nil) return false;
    } else {
      Outcome source = __objc3_err(id, id, object);
      Outcome copy = __objc3_copy(source);
      Outcome moved = __objc3_move(copy);
      object = nil;
      if (__objc3_is_ok(copy) || __objc3_is_ok(moved) || observer == nil) return false;
    }
  }
  return observer == nil && destroyed == before + 1;
}

static bool copy_and_move_blocks() {
  int before = destroyed;
  __weak id observer = nil;
  {
    id object = [TransferProbe new];
    observer = object;
    Action action = ^{ if (!object) __builtin_trap(); };
    OptionalAction source = __objc3_some(Action, action);
    object = nil;
    action = nil;
    {
      OptionalAction copy = __objc3_copy(source);
      OptionalAction moved = __objc3_move(copy);
      if (!__objc3_has_value(copy) || !__objc3_has_value(moved)) return false;
    }
    if (observer == nil) return false;
  }
  return observer == nil && destroyed == before + 1;
}

template <class Carrier> static bool template_copy(const Carrier &source) {
  Carrier copy = __objc3_copy(source);
  return __objc3_has_value(copy);
}
static bool nil_presence_and_unwind() {
  Strong absent = __objc3_none(id);
  Strong absent_copy = __objc3_copy(absent);
  Strong present = __objc3_some(id, nil);
  Strong present_copy = __objc3_move(present);
  Unsafe unsafe = __objc3_some(id __unsafe_unretained, nil);
  Unsafe unsafe_copy = __objc3_copy(unsafe);
  if (__objc3_has_value(absent_copy) || !__objc3_has_value(present_copy) ||
      !__objc3_has_value(unsafe_copy) || !template_copy(present)) return false;
  int before = destroyed;
  __weak id observer = nil;
  try {
    id object = [TransferProbe new];
    observer = object;
    Strong source = __objc3_some(id, object);
    Strong copy = __objc3_copy(source);
    Strong moved = __objc3_move(copy);
    Weak weak = __objc3_some(id __weak, object);
    Weak weak_copy = __objc3_copy(weak);
    object = nil;
    defer { if (observer == nil) __builtin_trap(); }
    throw 42;
  } catch (int value) {
    return value == 42 && observer == nil && destroyed == before + 1;
  }
  return false;
}

static id checked_value(id value, id<Error> error) __objc3_throws(id<Error>) {
  if (error) __objc3_throw(error);
  return value;
}
static bool transfer_try_optional_results() {
  int before = destroyed;
  __weak id observer = nil;
  @autoreleasepool {
    id object = [TransferProbe new];
    id<Error> error = [TransferProbe new];
    observer = object;
    Strong success = __objc3_try_optional(checked_value(object, nil));
    Strong failure = __objc3_try_optional(checked_value(object, error));
    Strong nil_success = __objc3_try_optional(checked_value(nil, nil));
    {
      Strong copy = __objc3_copy(success);
      Strong moved = __objc3_move(copy);
      Strong failure_copy = __objc3_copy(failure);
      Strong failure_move = __objc3_move(failure_copy);
      Strong nil_copy = __objc3_copy(nil_success);
      if (!__objc3_has_value(copy) || !__objc3_has_value(moved) ||
          __objc3_has_value(failure_copy) || __objc3_has_value(failure_move) ||
          !__objc3_has_value(nil_copy)) return false;
    }
    object = nil;
    error = nil;
    if (observer == nil) return false;
  }
  return observer == nil && destroyed == before + 2;
}

int main() {
  if (!copy_and_move_strong()) return 1;
  if (!copy_and_move_weak()) return 2;
  if (!copy_and_move_result(true)) return 3;
  if (!copy_and_move_result(false)) return 4;
  if (!copy_and_move_blocks()) return 5;
  if (!nil_presence_and_unwind()) return 6;
  if (!transfer_try_optional_results()) return 7;
  return 0;
}
