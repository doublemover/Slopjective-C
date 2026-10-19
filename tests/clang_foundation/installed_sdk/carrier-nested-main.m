#include "carrier-nested-call.h"
#include <objc/runtime.h>
#include <objc/encoding.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "nested failure %d: %s\n", __LINE__, #x); return __LINE__; } } while (0)
static int metadata(void) {
  const char *strong = @encode(NestedStrong), *weak = @encode(NestedWeak);
  CHECK(strstr(strong, "__objc3_v12_optional_N") && strstr(strong, "__objc3_v12_optional_S40="));
  CHECK(strcmp(strong, weak));
  CHECK(objc_sizeof_type(strong) == sizeof(NestedStrong));
  CHECK(objc_alignof_type(strong) == __alignof__(NestedStrong));
  CHECK(objc_sizeof_type(@encode(NestedDeep)) == sizeof(NestedDeep));
  CHECK(objc_alignof_type(@encode(NestedDeep)) == __alignof__(NestedDeep));
  CHECK(objc_sizeof_type(@encode(NestedPlainMixed)) == sizeof(NestedPlainMixed));
  CHECK(*objc_skip_typespec(@encode(NestedDeep)) == '\0');
  Method method = class_getInstanceMethod(objc_getClass("NestedCalls"), @selector(echo:));
  CHECK(method && method_getNumberOfArguments(method) == 3);
  char *result = method_copyReturnType(method), *argument = method_copyArgumentType(method, 2);
  CHECK(!strcmp(result, strong) && !strcmp(argument, strong));
  free(result); free(argument);
  return 0;
}
static int lifetimes(void) {
  {
    NestedStrong source = nested_make(0), copy = nested_identity(source);
    source = __objc3_none(CallStrong);
    CHECK(!call_destroyed(0) && nested_state(copy) == 0);
    copy = copy;
#ifdef __cplusplus
    copy = __objc3_copy(copy);
#endif
    CHECK(!call_destroyed(0));
    copy = __objc3_none(CallStrong); CHECK(call_destroyed(0) == 1);
  }
  {
    id object = call_object(1);
    NestedWeak source = __objc3_some(CallWeak, __objc3_some(id __weak, object)), copy = source;
    CHECK(nested_weak_state(copy) == 0);
    object = (id)0;
    CHECK(call_destroyed(1) == 1 && nested_weak_state(copy) == 1);
    source = __objc3_none(CallWeak); copy = __objc3_none(CallWeak);
  }
  {
    NestedMixed value = __objc3_ok(CallStrong, CallWeak, call_make(2));
    id object = call_object(3);
    NestedMixed weak = __objc3_err(CallStrong, CallWeak, __objc3_some(id __weak, object));
    value = nested_mixed(weak); CHECK(call_destroyed(2) == 1);
    object = (id)0; CHECK(call_destroyed(3) == 1);
    value = __objc3_ok(CallStrong, CallWeak, call_make(4));
    CHECK(__objc3_is_ok(value));
    value = __objc3_err(CallStrong, CallWeak, __objc3_none(id __weak));
    CHECK(call_destroyed(4) == 1);
  }
  {
    NestedBlock source = __objc3_some(CallBlock, call_block(call_object(5))), copy = source;
    source = __objc3_none(CallBlock); CHECK(!call_destroyed(5));
    copy = __objc3_none(CallBlock); CHECK(call_destroyed(5) == 1);
  }
  {
    NestedDeep source = __objc3_some(NestedMixed, __objc3_ok(CallStrong, CallWeak, call_make(6)));
    NestedDeep result = nested_deep(source);
    source = __objc3_none(NestedMixed); CHECK(!call_destroyed(6));
    result = __objc3_none(NestedMixed); CHECK(call_destroyed(6) == 1);
  }
  {
    NestedPlainMixed value = __objc3_ok(NestedPlain, CallStrong,
        __objc3_some(__objc3_optional(int), __objc3_some(int, 42))), copy = value;
    value = __objc3_err(NestedPlain, CallStrong, call_make(40));
    CHECK(__objc3_is_ok(copy) && !__objc3_is_ok(value));
    value = copy; CHECK(call_destroyed(40) == 1);
  }
  return 0;
}
static int matches(void) {
  NestedStrong source = nested_make(7);
  int matched = 0;
  __objc3_match_stmt(source) {
    case __objc3_case_some(let child): {
      source = __objc3_none(CallStrong);
      CHECK(!call_destroyed(7) && __objc3_has_value(child));
      __objc3_match_stmt(child) {
        case __objc3_case_some(let object): { CHECK(object != (id)0); matched = 1; }
        case __objc3_case_none: { return 201; }
      }
    }
    case __objc3_case_none: { return 202; }
  }
  CHECK(matched && call_destroyed(7) == 1);
  @autoreleasepool {
    @try {
      __objc3_match_stmt(nested_make(8)) {
        case __objc3_case_some(let child): { CHECK(__objc3_has_value(child)); @throw call_object(9); }
        case __objc3_case_none: { return 203; }
      }
    } @catch (id error) { CHECK(error != (id)0 && call_destroyed(8) == 1); }
  }
  CHECK(call_destroyed(9) == 1);
  // A rejected guard can mutate the source; later bindings still refer to the
  // one snapshot captured before pattern selection.
  source = nested_make(36);
  matched = 0;
  __objc3_match_stmt(source) {
    case __objc3_case_some(let child) where ((source = __objc3_none(CallStrong)), (__typeof__(__objc3_has_value(source)))0): { return 206; }
    case __objc3_case_some(let child): { CHECK(__objc3_has_value(child) && !call_destroyed(36)); matched = 1; }
    case __objc3_case_none: { return 207; }
  }
  CHECK(matched && call_destroyed(36) == 1);
  return 0;
}
static int calls(void) {
  NestedCalls *receiver = [NestedCalls new];
  NestedStrong source = nested_make(10);
  NestedStrong result = [receiver echo:source];
  result = [receiver direct:result];
  NestedStrong (^block)(NestedStrong) = ^NestedStrong(NestedStrong value) { return value; };
  result = block(result);
  source = __objc3_none(CallStrong); CHECK(!call_destroyed(10));
  result = __objc3_none(CallStrong); CHECK(call_destroyed(10) == 1);
  NestedCalls *missing = (id)0;
  result = [missing echo:nested_make(11)];
  CHECK(nested_state(result) == 3 && call_destroyed(11) == 1);
  result = [missing direct:nested_make(12)];
  CHECK(nested_state(result) == 3 && call_destroyed(12) == 1);
  [receiver echo:nested_make(13)]; CHECK(call_destroyed(13) == 1);
  NestedMixed mixed = [missing mixed:__objc3_ok(CallStrong, CallWeak, call_make(14))];
  CHECK(!__objc3_is_ok(mixed) && call_destroyed(14) == 1);
  @autoreleasepool {
    @try { result = [receiver native:nested_make(15)]; }
    @catch (id error) { CHECK(error != (id)0 && call_destroyed(15) == 1 && call_destroyed(30) == 1); }
  }
  CHECK(call_destroyed(31) == 1);
#ifdef __cplusplus
  {
    NestedDerived derived;
    NestedBase *base = &derived;
    NestedStrong value = base->apply(nested_make(16));
    NestedStrong (NestedBase::*member)(NestedStrong) = &NestedBase::apply;
    value = (base->*member)(value);
    CHECK(nested_state(value) == 0 && !call_destroyed(16));
  }
  CHECK(call_destroyed(16) == 1);
#endif
  return 0;
}
static int match_propagate(NestedStrong value) __objc3_throws(id<Error>) {
  __objc3_match_stmt(value) {
    case __objc3_case_some(let child): {
      value = __objc3_none(CallStrong);
      if (!__objc3_has_value(child)) __builtin_trap();
      return __objc3_try(throws_scalar(__objc3_none(id), 35, 1));
    }
    case __objc3_case_none: { return 0; }
  }
  return -1;
}
static int errors(void) {
  {
    NestedStrong value = __objc3_try_optional(throws_value(call_make(17), 18, 0));
    CHECK(nested_state(value) == 0);
    value = __objc3_none(CallStrong); CHECK(call_destroyed(17) == 1);
  }
  @autoreleasepool {
    NestedStrong missing = __objc3_try_optional(throws_value(call_make(19), 20, 1));
    CHECK(nested_state(missing) == 3 && call_destroyed(19) == 1 && call_destroyed(20) == 1);
  }
  {
    NestedStrong value = nested_make(21);
    @autoreleasepool {
      int caught = 0;
      __objc3_do { value = __objc3_try(nested_forward(value, 22, 1)); }
      __objc3_catch(id<Error> error) { CHECK(error != (id)0); caught = 1; }
      CHECK(caught && nested_state(value) == 0 && !call_destroyed(21) && call_destroyed(22) == 2);
    }
    value = __objc3_none(CallStrong); CHECK(call_destroyed(21) == 1);
  }
  NestedCalls *receiver = [NestedCalls new];
  {
    __objc3_optional(NestedStrong) value = __objc3_try_optional([receiver throwing:nested_make(23) key:24 fail:0]);
    CHECK(__objc3_has_value(value) && !call_destroyed(23) && call_destroyed(24) == 1);
  }
  CHECK(call_destroyed(23) == 1);
  {
    id object = call_object(37);
    NestedWeak value = __objc3_try_optional(throws_weak(__objc3_some(id __weak, object), 0));
    CHECK(nested_weak_state(value) == 0);
    object = (id)0;
    CHECK(nested_weak_state(value) == 1 && call_destroyed(37) == 1);
  }
  {
    NestedBlock value = __objc3_try_optional(throws_payload_block(call_block(call_object(38)), 0));
    CHECK(__objc3_has_value(value) && !call_destroyed(38));
    value = __objc3_none(CallBlock); CHECK(call_destroyed(38) == 1);
  }
  @autoreleasepool {
    NestedBlock value = __objc3_try_optional(throws_payload_block(call_block(call_object(39)), 1));
    CHECK(!__objc3_has_value(value) && call_destroyed(39) == 1);
  }
  @autoreleasepool {
    int caught = 0;
    __objc3_do { (void)__objc3_try(match_propagate(nested_make(34))); }
    __objc3_catch(id<Error> error) { CHECK(error != (id)0); caught = 1; }
    CHECK(caught && call_destroyed(34) == 1 && call_destroyed(35) == 1);
  }
  __objc3_try_optional(nested_forward(nested_make(41), 42, 0));
  CHECK(call_destroyed(41) == 1 && call_destroyed(42) == 1);
  @autoreleasepool {
    __objc3_try_optional(nested_forward(nested_make(43), 44, 1));
    CHECK(call_destroyed(43) == 1 && call_destroyed(44) == 2);
  }
  CHECK(__objc3_has_value(__objc3_try_optional(nested_forward(nested_make(48), 49, 0))));
  CHECK(call_destroyed(48) == 1 && call_destroyed(49) == 1);
  CHECK(throws_created() == throws_destroyed());
#ifdef __cplusplus
  {
    NestedStrong original = nested_make(25), moved = __objc3_move(original);
    CHECK(nested_state(original) == 1 && nested_state(moved) == 0);
    moved = __objc3_none(CallStrong); CHECK(call_destroyed(25) == 1);
  }
  {
    const NestedStrong original = nested_make(26);
    NestedStrong copied = __objc3_move(original);
    copied = __objc3_none(CallStrong); CHECK(!call_destroyed(26));
  }
  CHECK(call_destroyed(26) == 1);
  try { NestedStrong value = __objc3_try_force(nested_cleanup(27, 1)); return 204; }
  catch (int error) { CHECK(error == 42 && call_destroyed(27) == 1); }
  try {
    __objc3_match_stmt(nested_make(28)) {
      case __objc3_case_some(let child): { CHECK(__objc3_has_value(child)); throw 43; }
      case __objc3_case_none: { return 205; }
    }
  } catch (int error) { CHECK(error == 43 && call_destroyed(28) == 1); }
#endif
  return 0;
}
int main(int argc, char **argv) {
  if (argc == 2 && !strcmp(argv[1], "force-nested")) {
    NestedStrong value = __objc3_try_force(nested_forward(nested_make(32), 33, 1));
    return nested_state(value);
  }
  int result;
  if ((result = metadata())) return result;
  if ((result = lifetimes())) return result;
  if ((result = matches())) return result;
  if ((result = calls())) return result;
  return errors();
}
