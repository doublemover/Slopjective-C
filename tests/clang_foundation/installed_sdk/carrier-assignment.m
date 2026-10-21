// Installed ARC witness, compiled as both Objective-C and Objective-C++.
#include <objc/runtime.h>

static int destroyed[32];
__attribute__((objc_root_class))
@interface AssignmentProbe { Class isa; @public int key; }
+ (id)newWithKey:(int)value;
@end
@implementation AssignmentProbe
+ (id)newWithKey:(int)value {
  AssignmentProbe *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (void)dealloc { ++destroyed[key]; object_dispose(self); }
@end

typedef __objc3_optional(id) Strong;
typedef __objc3_optional(id __weak) Weak;
typedef __objc3_optional(id __unsafe_unretained) Unsafe;
typedef __objc3_result(id, id) Outcome;
typedef __objc3_result(id, id __weak) Mixed;
typedef __objc3_result(id, int) ScalarError;
typedef void (^Action)(void);
typedef __objc3_optional(Action) OptionalAction;

#define REQUIRE(condition) do { if (!(condition)) return __LINE__; } while (0)

static int optional_assignment(void) {
  __weak id first = nil, second = nil;
  {
    id a = [AssignmentProbe newWithKey:0];
    id b = [AssignmentProbe newWithKey:1];
    first = a; second = b;
    Strong destination = __objc3_some(id, a);
    const Strong source = __objc3_some(id, b);
    a = nil; b = nil;
    Strong implicit = source;
    destination = implicit;
    REQUIRE(first == nil && second != nil && destroyed[0] == 1);
    destination = destination;
    implicit = __objc3_none(id);
    REQUIRE(__objc3_has_value(destination) && second != nil);
    destination = __objc3_some(id, nil);
    REQUIRE(__objc3_has_value(destination) && second != nil);
    destination = __objc3_none(id);
    REQUIRE(!__objc3_has_value(destination));
  }
  REQUIRE(second == nil && destroyed[1] == 1);
  {
    Strong absent = __objc3_none(id);
    Strong present = __objc3_some(id, nil);
    const Strong copy = present;
    absent = present;
    REQUIRE(__objc3_has_value(absent) && __objc3_has_value(copy));
    present = __objc3_none(id);
    absent = present;
    REQUIRE(!__objc3_has_value(absent));
    // Conditional construction and assignment preserve explicit nil presence.
    Strong selected = destroyed[0] ? copy : present;
    REQUIRE(__objc3_has_value(selected));
    selected = destroyed[0] ? __objc3_none(id) : __objc3_some(id, nil);
    REQUIRE(!__objc3_has_value(selected));
  }
  return 0;
}

static int result_transitions(void) {
  for (int initial = 0; initial != 2; ++initial) {
    for (int final = 0; final != 2; ++final) {
      int oldKey = 2 + 2 * initial + final;
      int newKey = 6 + 2 * initial + final;
      __weak id oldObserver = nil, newObserver = nil;
      {
        id oldObject = [AssignmentProbe newWithKey:oldKey];
        id newObject = [AssignmentProbe newWithKey:newKey];
        oldObserver = oldObject; newObserver = newObject;
        Outcome destination = initial ? __objc3_ok(id, id, oldObject)
                                      : __objc3_err(id, id, oldObject);
        Outcome source = final ? __objc3_ok(id, id, newObject)
                               : __objc3_err(id, id, newObject);
        oldObject = nil; newObject = nil;
        Outcome copy = source;
        destination = copy;
        REQUIRE(__objc3_is_ok(destination) == (final != 0));
        REQUIRE(oldObserver == nil && newObserver != nil);
        REQUIRE(destroyed[oldKey] == 1);
        source = __objc3_ok(id, id, nil);
        copy = __objc3_err(id, id, nil);
        REQUIRE(newObserver != nil);
        destination = destination;
        REQUIRE(__objc3_is_ok(destination) == (final != 0));
      }
      REQUIRE(newObserver == nil && destroyed[newKey] == 1);
    }
  }
  return 0;
}

static int weak_and_unsafe(void) {
  __weak id observer = nil;
  {
    id object = [AssignmentProbe newWithKey:10];
    observer = object;
    Weak source = __objc3_some(id __weak, object);
    const Weak copy = source;
    Weak destination = __objc3_none(id __weak);
    destination = source;
    destination = destination;
    source = __objc3_none(id __weak);
    object = nil;
    REQUIRE(observer == nil && destroyed[10] == 1);
    REQUIRE(__objc3_has_value(copy) && __objc3_has_value(destination));
    // Copying zeroed weak storage must keep a present nil payload.
    destination = copy;
    REQUIRE(__objc3_has_value(destination));
    destination = __objc3_none(id __weak);
    REQUIRE(!__objc3_has_value(destination));
  }
  {
    id object = [AssignmentProbe newWithKey:11];
    observer = object;
    Unsafe source = __objc3_some(id __unsafe_unretained, object);
    Unsafe destination = source;
    destination = source;
    destination = destination;
    object = nil;
    REQUIRE(observer == nil && destroyed[11] == 1);
    REQUIRE(__objc3_has_value(destination));
    destination = __objc3_none(id __unsafe_unretained);
  }
  {
    id object = [AssignmentProbe newWithKey:12];
    observer = object;
    Mixed value = __objc3_ok(id, id __weak, object);
    Mixed weak = __objc3_err(id, id __weak, object);
    object = nil;
    value = weak;
    REQUIRE(observer == nil && !__objc3_is_ok(value));
    value = __objc3_ok(id, id __weak, nil);
    REQUIRE(__objc3_is_ok(value));
  }
  {
    id object = [AssignmentProbe newWithKey:13];
    observer = object;
    ScalarError value = __objc3_ok(id, int, object);
    object = nil;
    value = __objc3_err(id, int, 42);
    REQUIRE(observer == nil && !__objc3_is_ok(value));
    value = __objc3_ok(id, int, nil);
    REQUIRE(__objc3_is_ok(value));
  }
  return 0;
}

static int blocks(void) {
  __weak id observer = nil;
  {
    OptionalAction source = __objc3_none(Action);
    {
      id object = [AssignmentProbe newWithKey:14];
      observer = object;
      Action action = ^{ if (!object) __builtin_trap(); };
      source = __objc3_some(Action, action);
      object = nil; action = nil;
    }
    OptionalAction destination = source;
    destination = source;
    destination = destination;
    source = __objc3_none(Action);
    REQUIRE(observer != nil);
    destination = __objc3_some(Action, nil);
    REQUIRE(observer == nil && destroyed[14] == 1);
    REQUIRE(__objc3_has_value(destination));
  }
  return 0;
}

#ifdef __cplusplus
static int sequence, leftCalls, rightCalls;
static Strong &left(Strong &destination) {
  sequence = sequence * 10 + 2; ++leftCalls;
  return destination;
}
static const Strong &right(const Strong &source) {
  sequence = sequence * 10 + 1; ++rightCalls;
  return source;
}
static Strong &mutating_left(Strong &destination, Strong &source, bool fail) {
  source = __objc3_none(id);
  if (fail) throw 42;
  return destination;
}
static const Strong &throwing_right(const Strong &source) {
  throw 43;
  return source;
}
static int sequencing_and_unwind(void) {
  Strong source = __objc3_some(id, nil);
  Strong destination = __objc3_none(id);
  sequence = leftCalls = rightCalls = 0;
  Strong &reference = (left(destination) = right(source));
  REQUIRE(&reference == &destination && sequence == 12);
  REQUIRE(leftCalls == 1 && rightCalls == 1);
  Strong copy = (destination = source);
  REQUIRE(__objc3_has_value(copy));
  Strong other = __objc3_none(id);
  other = destination = source;
  REQUIRE(__objc3_has_value(other));
  destination = __objc3_move(copy);
  REQUIRE(__objc3_has_value(copy) && __objc3_has_value(destination));
  {
    __weak id observer = nil;
    id object = [AssignmentProbe newWithKey:18];
    observer = object;
    Strong original = __objc3_some(id, object);
    object = nil;
    destination = __objc3_move(original);
    REQUIRE(__objc3_has_value(original) && observer != nil);
    destination = __objc3_move(destination);
    REQUIRE(__objc3_has_value(destination) && observer != nil);
    Strong movedFromCopy = original;
    REQUIRE(__objc3_has_value(movedFromCopy));
    destination = __objc3_none(id);
    REQUIRE(observer == nil && destroyed[18] == 1);
  }
  for (int iteration = 0; iteration != 2; ++iteration) {
    bool fail = iteration != 0;
    int key = fail ? 16 : 15;
    __weak id observer = nil;
    {
      id object = [AssignmentProbe newWithKey:key];
      observer = object;
      Strong original = __objc3_some(id, object);
      object = nil;
      try {
        mutating_left(destination, original, fail) = original;
        REQUIRE(!fail && observer != nil);
        REQUIRE(!__objc3_has_value(original));
        destination = __objc3_none(id);
      } catch (int error) {
        REQUIRE(fail && error == 42 && observer == nil);
      }
    }
    REQUIRE(observer == nil && destroyed[key] == 1);
  }
  leftCalls = 0;
  try { left(destination) = throwing_right(source); }
  catch (int error) { REQUIRE(error == 43 && leftCalls == 0); }
  __weak id observer = nil;
  try {
    id object = [AssignmentProbe newWithKey:17];
    observer = object;
    Strong original = __objc3_some(id, object);
    Strong implicit = original;
    Weak weak = __objc3_some(id __weak, object);
    Weak weakCopy = weak;
    implicit = __objc3_move(original);
    weakCopy = weak;
    object = nil;
    throw 44;
  } catch (int error) {
    REQUIRE(error == 44 && observer == nil && destroyed[17] == 1);
  }
  return 0;
}
#endif

int main(void) {
  int result;
  if ((result = optional_assignment())) return result;
  if ((result = result_transitions())) return result;
  if ((result = weak_and_unsafe())) return result;
  if ((result = blocks())) return result;
#ifdef __cplusplus
  if ((result = sequencing_and_unwind())) return result;
#endif
  return 0;
}
