#ifndef OBJC3_CARRIER_DEBUGGER_VALUES_H
#define OBJC3_CARRIER_DEBUGGER_VALUES_H

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

typedef Optional(int) Number;
typedef Optional(id _Nullable) Reference;
typedef Optional(Number) Nested;
typedef Result(int, long) Outcome;
typedef Optional(void) Unit;
typedef Result(void, int) FallibleUnit;
typedef Result(int, void) EmptyError;
typedef Result(void, void) UnitResult;
typedef struct DebugPair { int first, second; } DebugPair;
typedef Optional(DebugPair) Aggregate;

__attribute__((noinline)) static void carrier_values_stop(void) {
  __asm__ volatile("" ::: "memory");
}
__attribute__((noinline)) static void carrier_transition_stop(void) {
  __asm__ volatile("" ::: "memory");
}
__attribute__((noinline)) static void carrier_then_stop(void) {
  __asm__ volatile("" ::: "memory");
}
__attribute__((noinline)) static void carrier_else_stop(void) {
  __asm__ volatile("" ::: "memory");
}
__attribute__((noinline)) static void carrier_continuation_stop(void) {
  __asm__ volatile("" ::: "memory");
}

static int inspect_values(void) {
  Number present = Some(int, 7);
  const Number immutable = Some(int, 11);
  Number absent = None(int);
  // The inactive storage deliberately contains old bytes. None must neither
  // render a payload nor turn those bytes into a synthetic active child.
  for (__SIZE_TYPE__ index = 1; index < sizeof absent; ++index)
    ((unsigned char *)&absent)[index] = 0xa5;
  Reference tagged_nil = Some(id _Nullable, (id)0);
  Nested nested = Some(Number, Some(int, 9));
  DebugPair pair = {3, 4};
  Aggregate aggregate = Some(DebugPair, pair);
  Unit unit = Some(void, (void)0);
  Outcome outcome = Ok(int, long, 42);
  Outcome zero_error = Err(int, long, 0L);
  FallibleUnit unit_ok = Ok(void, int, (void)0);
  EmptyError unit_error = Err(int, void, (void)0);
  UnitResult both_ok = Ok(void, void, (void)0);
  UnitResult both_error = Err(void, void, (void)0);
  guard let bound_nil = tagged_nil else { return 1; }
  carrier_values_stop();
  present = None(int);
  tagged_nil = None(id _Nullable);
  nested = Some(Number, None(int));
  outcome = Err(int, long, -5L);
  carrier_transition_stop();
  return hasValue(present) || hasValue(tagged_nil) || isOk(outcome) ||
         !hasValue(nested) || !hasValue(aggregate) || !hasValue(unit) ||
         isOk(zero_error) || !isOk(unit_ok) || isOk(unit_error) ||
         !isOk(both_ok) || isOk(both_error) || bound_nil != (id)0;
}

static int cleanup_events;
static void close_debug_resource(int value) {
  cleanup_events = cleanup_events * 10 + value;
}

static int inspect_scopes(bool take_then) {
  int handle __attribute__((objc_resource(close=close_debug_resource,
                                          invalid=-1))) = 3;
  defer { cleanup_events = cleanup_events * 10 + 4; }
  guard let stable = Some(int, 41) else { return -1; }
  if var local = Some(int, 1), take_then {
    local += stable;
    carrier_then_stop();
  } else {
    carrier_else_stop();
  }
  carrier_continuation_stop();
  return stable + handle;
}

#endif
