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

using Strong = Optional(id);
using Weak = Optional(id __weak);
using Unsafe = Optional(id __unsafe_unretained);
using Outcome = Result(id, id);
using Action = void (^)(void);
using OptionalAction = Optional(Action);

static bool copy_and_move_strong() {
  int before = destroyed;
  __weak id observer = nil;
  {
    id object = [TransferProbe new];
    observer = object;
    Strong source = Some(id, object);
    object = nil;
    {
      const Strong copy = copyValue(source);
      Strong moved_const = moveValue(copy);
      Strong another = moveValue(moved_const);
      if (!hasValue(copy) || !hasValue(moved_const) ||
          !hasValue(another)) return false;
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
    Weak source = Some(id __weak, object);
    Weak copy = copyValue(source);
    Weak moved = moveValue(copy);
    object = nil;
    if (observer != nil || !hasValue(source) ||
        !hasValue(copy) || !hasValue(moved)) return false;
    Weak zeroed = copyValue(moved);
    if (!hasValue(zeroed)) return false;
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
      Outcome source = Ok(id, id, object);
      Outcome copy = copyValue(source);
      Outcome moved = moveValue(copy);
      object = nil;
      if (!isOk(copy) || !isOk(moved) || observer == nil) return false;
    } else {
      Outcome source = Err(id, id, object);
      Outcome copy = copyValue(source);
      Outcome moved = moveValue(copy);
      object = nil;
      if (isOk(copy) || isOk(moved) || observer == nil) return false;
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
    OptionalAction source = Some(Action, action);
    object = nil;
    action = nil;
    {
      OptionalAction copy = copyValue(source);
      OptionalAction moved = moveValue(copy);
      if (!hasValue(copy) || !hasValue(moved)) return false;
    }
    if (observer == nil) return false;
  }
  return observer == nil && destroyed == before + 1;
}

template <class Carrier> static bool template_copy(const Carrier &source) {
  Carrier copy = copyValue(source);
  return hasValue(copy);
}
static bool nil_presence_and_unwind() {
  Strong absent = None(id);
  Strong absent_copy = copyValue(absent);
  Strong present = Some(id, nil);
  Strong present_copy = moveValue(present);
  Unsafe unsafe = Some(id __unsafe_unretained, nil);
  Unsafe unsafe_copy = copyValue(unsafe);
  if (hasValue(absent_copy) || !hasValue(present_copy) ||
      !hasValue(unsafe_copy) || !template_copy(present)) return false;
  int before = destroyed;
  __weak id observer = nil;
  try {
    id object = [TransferProbe new];
    observer = object;
    Strong source = Some(id, object);
    Strong copy = copyValue(source);
    Strong moved = moveValue(copy);
    Weak weak = Some(id __weak, object);
    Weak weak_copy = copyValue(weak);
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
    Strong assigned = None(id);
    assigned = __objc3_try_optional(checked_value(object, nil));
    if (!hasValue(assigned)) return false;
    assigned = __objc3_try_optional(checked_value(object, error));
    if (hasValue(assigned)) return false;
    assigned = __objc3_try_optional(checked_value(nil, nil));
    if (!hasValue(assigned)) return false;
    {
      Strong copy = copyValue(success);
      Strong moved = moveValue(copy);
      Strong failure_copy = copyValue(failure);
      Strong failure_move = moveValue(failure_copy);
      Strong nil_copy = copyValue(nil_success);
      if (!hasValue(copy) || !hasValue(moved) ||
          hasValue(failure_copy) || hasValue(failure_move) ||
          !hasValue(nil_copy)) return false;
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
