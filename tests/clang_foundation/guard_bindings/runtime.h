#pragma once
#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif
static int guard_failures, guard_calls, guard_events;
#define GUARD_CHECK(value) do { if (!(value)) ++guard_failures; } while (0)
static Optional(int) guard_next(int value, bool present) {
  ++guard_calls;
  return present ? Some(int, value) : None(int);
}
static int guard_ordered(bool first, bool second, bool condition) {
  guard let a = guard_next(3, first), b = guard_next(a + 4, second),
        condition, var c = Some(int, b + 2)
  else { return -guard_calls; }
  c += a;
  return a + b + c;
}
static int guard_snapshot(void) {
  Optional(int) source = Some(int, 7);
  guard let value = source else { return -1; }
  source = None(int);
  GUARD_CHECK(!hasValue(source));
  return value;
}
static void guard_if_list(bool present) {
  int before = guard_calls;
  if var first = guard_next(2, present), second = guard_next(first + 3, true) {
    first += second;
    second += first;
    GUARD_CHECK(first == 7 && second == 12);
    GUARD_CHECK(guard_calls == before + 2);
  } else {
    GUARD_CHECK(!present && guard_calls == before + 1);
  }
}
static void guard_loop(void) {
  int sum = 0;
  for (int i = 0; i != 5; ++i) {
    guard let value = guard_next(i, i != 2) else { continue; }
    sum += value;
  }
  GUARD_CHECK(sum == 8);
}
static int guard_deferred(bool present) {
  defer { guard_events = guard_events * 10 + 1; }
  guard let value = guard_next(4, present) else { return -1; }
  defer { guard_events = guard_events * 10 + value; }
  return value;
}
static int guard_statement_expression(bool present) {
  return ({ guard let value = guard_next(9, present) else { return -1; } value + 1; });
}
static void guard_nested(void) {
  Optional(int) source = Some(int, 5);
  guard let value = source else { ++guard_failures; return; }
  {
    guard let value = Some(int, 8) else { ++guard_failures; return; }
    GUARD_CHECK(value == 8);
  }
  GUARD_CHECK(value == 5);
  int (^block)(void) = ^{ return value + 1; };
  GUARD_CHECK(block() == 6);
}
#ifdef __cplusplus
template<class T> static int guard_template(T source) {
  guard let value = source else { return -1; }
  return value + 2;
}
static void guard_cpp(void) {
  GUARD_CHECK(guard_template(Some(int, 40)) == 42);
  GUARD_CHECK(guard_template(None(int)) == -1);
  int counter = 0;
  auto next = [&counter]() { ++counter; return Some(int, counter); };
  guard let first = next(), second = next() else { ++guard_failures; return; }
  GUARD_CHECK(first == 1 && second == 2 && counter == 2);
}
#endif
#if __has_feature(objc_arc)
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#ifdef __cplusplus
}
#endif
static int guard_created, guard_destroyed;
__attribute__((objc_root_class))
@interface GuardObject { Class isa; @public int number; }
+ (instancetype)make:(int)value __attribute__((ns_returns_retained));
@end
@implementation GuardObject
+ (instancetype)make:(int)value {
  GuardObject *object = class_createInstance(self, 0);
  object->number = value;
  ++guard_created;
  return object;
}
- (void)dealloc { ++guard_destroyed; object_dispose(self); }
@end
static void guard_managed(void) {
  GuardObject * __weak witness;
  {
    GuardObject *owner = [GuardObject make:42];
    witness = owner;
    guard let value = owner else { ++guard_failures; return; }
    owner = (id)0;
    GUARD_CHECK(witness != (id)0 && value->number == 42);
    int (^capture)(void) = ^{ return value->number; };
    GUARD_CHECK(capture() == 42);
  }
  GUARD_CHECK(witness == (id)0);
  GUARD_CHECK(guard_created == guard_destroyed);
  Optional(id) tagged = Some(id, (id)0);
  guard let present_nil = tagged else { ++guard_failures; return; }
  GUARD_CHECK(present_nil == (id)0);
  id __weak absent = (id)0;
  if let unexpected = absent { (void)unexpected; ++guard_failures; }
  else { GUARD_CHECK(absent == (id)0); }
  int (^source)(int) = ^(int value) { return value + 3; };
  guard let bound = source else { ++guard_failures; return; }
  source = (id)0;
  GUARD_CHECK(bound(4) == 7);
}
#endif
static void guard_plain_suite(void) {
  guard_calls = 0; GUARD_CHECK(guard_ordered(false, true, true) == -1);
  guard_calls = 0; GUARD_CHECK(guard_ordered(true, false, true) == -2);
  guard_calls = 0; GUARD_CHECK(guard_ordered(true, true, false) == -2);
  guard_calls = 0; GUARD_CHECK(guard_ordered(true, true, true) == 22);
  GUARD_CHECK(guard_snapshot() == 7);
  guard_if_list(false); guard_if_list(true); guard_loop(); guard_nested();
  guard_events = 0; GUARD_CHECK(guard_deferred(false) == -1); GUARD_CHECK(guard_events == 1);
  guard_events = 0; GUARD_CHECK(guard_deferred(true) == 4); GUARD_CHECK(guard_events == 41);
  GUARD_CHECK(guard_statement_expression(false) == -1);
  GUARD_CHECK(guard_statement_expression(true) == 10);
#ifdef __cplusplus
  guard_cpp();
#endif
#if __has_feature(objc_arc)
  guard_managed();
#endif
}
