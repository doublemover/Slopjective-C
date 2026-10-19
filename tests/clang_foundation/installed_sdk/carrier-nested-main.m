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
    source = None(CallStrong);
    CHECK(!call_destroyed(0) && nested_state(copy) == 0);
    copy = copy;
#ifdef __cplusplus
    copy = copyValue(copy);
#endif
    CHECK(!call_destroyed(0));
    copy = None(CallStrong); CHECK(call_destroyed(0) == 1);
  }
  {
    id object = call_object(1);
    NestedWeak source = Some(CallWeak, Some(id __weak, object)), copy = source;
    CHECK(nested_weak_state(copy) == 0);
    object = (id)0;
    CHECK(call_destroyed(1) == 1 && nested_weak_state(copy) == 1);
    source = None(CallWeak); copy = None(CallWeak);
  }
  {
    NestedMixed value = Ok(CallStrong, CallWeak, call_make(2));
    id object = call_object(3);
    NestedMixed weak = Err(CallStrong, CallWeak, Some(id __weak, object));
    value = nested_mixed(weak); CHECK(call_destroyed(2) == 1);
    object = (id)0; CHECK(call_destroyed(3) == 1);
    value = Ok(CallStrong, CallWeak, call_make(4));
    CHECK(isOk(value));
    value = Err(CallStrong, CallWeak, None(id __weak));
    CHECK(call_destroyed(4) == 1);
  }
  {
    NestedBlock source = Some(CallBlock, call_block(call_object(5))), copy = source;
    source = None(CallBlock); CHECK(!call_destroyed(5));
    copy = None(CallBlock); CHECK(call_destroyed(5) == 1);
  }
  {
    NestedDeep source = Some(NestedMixed, Ok(CallStrong, CallWeak, call_make(6)));
    NestedDeep result = nested_deep(source);
    source = None(NestedMixed); CHECK(!call_destroyed(6));
    result = None(NestedMixed); CHECK(call_destroyed(6) == 1);
  }
  {
    NestedPlainMixed value = Ok(NestedPlain, CallStrong,
        Some(Optional(int), Some(int, 42))), copy = value;
    value = Err(NestedPlain, CallStrong, call_make(40));
    CHECK(isOk(copy) && !isOk(value));
    value = copy; CHECK(call_destroyed(40) == 1);
  }
  return 0;
}
static int matches(void) {
  NestedStrong source = nested_make(7);
  int matched = 0;
  match(source) {
    case .Some(let child): {
      source = None(CallStrong);
      CHECK(!call_destroyed(7) && hasValue(child));
      match(child) {
        case .Some(let object): { CHECK(object != (id)0); matched = 1; }
        case .None: { return 201; }
      }
    }
    case .None: { return 202; }
  }
  CHECK(matched && call_destroyed(7) == 1);
  @autoreleasepool {
    @try {
      match(nested_make(8)) {
        case .Some(let child): { CHECK(hasValue(child)); @throw call_object(9); }
        case .None: { return 203; }
      }
    } @catch (id error) { CHECK(error != (id)0 && call_destroyed(8) == 1); }
  }
  CHECK(call_destroyed(9) == 1);
  // A rejected guard can mutate the source; later bindings still refer to the
  // one snapshot captured before pattern selection.
  source = nested_make(36);
  matched = 0;
  match(source) {
    case .Some(let child) where ((source = None(CallStrong)), (__typeof__(hasValue(source)))0): { return 206; }
    case .Some(let child): { CHECK(hasValue(child) && !call_destroyed(36)); matched = 1; }
    case .None: { return 207; }
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
  source = None(CallStrong); CHECK(!call_destroyed(10));
  result = None(CallStrong); CHECK(call_destroyed(10) == 1);
  NestedCalls *missing = (id)0;
  result = [missing echo:nested_make(11)];
  CHECK(nested_state(result) == 3 && call_destroyed(11) == 1);
  result = [missing direct:nested_make(12)];
  CHECK(nested_state(result) == 3 && call_destroyed(12) == 1);
  [receiver echo:nested_make(13)]; CHECK(call_destroyed(13) == 1);
  NestedMixed mixed = [missing mixed:Ok(CallStrong, CallWeak, call_make(14))];
  CHECK(!isOk(mixed) && call_destroyed(14) == 1);
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
  match(value) {
    case .Some(let child): {
      value = None(CallStrong);
      if (!hasValue(child)) __builtin_trap();
      return __objc3_try(throws_scalar(None(id), 35, 1));
    }
    case .None: { return 0; }
  }
  return -1;
}
static int errors(void) {
  {
    NestedStrong value = __objc3_try_optional(throws_value(call_make(17), 18, 0));
    CHECK(nested_state(value) == 0);
    value = None(CallStrong); CHECK(call_destroyed(17) == 1);
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
    value = None(CallStrong); CHECK(call_destroyed(21) == 1);
  }
  NestedCalls *receiver = [NestedCalls new];
  {
    Optional(NestedStrong) value = __objc3_try_optional([receiver throwing:nested_make(23) key:24 fail:0]);
    CHECK(hasValue(value) && !call_destroyed(23) && call_destroyed(24) == 1);
  }
  CHECK(call_destroyed(23) == 1);
  {
    id object = call_object(37);
    NestedWeak value = __objc3_try_optional(throws_weak(Some(id __weak, object), 0));
    CHECK(nested_weak_state(value) == 0);
    object = (id)0;
    CHECK(nested_weak_state(value) == 1 && call_destroyed(37) == 1);
  }
  {
    NestedBlock value = __objc3_try_optional(throws_payload_block(call_block(call_object(38)), 0));
    CHECK(hasValue(value) && !call_destroyed(38));
    value = None(CallBlock); CHECK(call_destroyed(38) == 1);
  }
  @autoreleasepool {
    NestedBlock value = __objc3_try_optional(throws_payload_block(call_block(call_object(39)), 1));
    CHECK(!hasValue(value) && call_destroyed(39) == 1);
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
  CHECK(hasValue(__objc3_try_optional(nested_forward(nested_make(48), 49, 0))));
  CHECK(call_destroyed(48) == 1 && call_destroyed(49) == 1);
  CHECK(throws_created() == throws_destroyed());
#ifdef __cplusplus
  {
    NestedStrong original = nested_make(25), moved = moveValue(original);
    CHECK(nested_state(original) == 1 && nested_state(moved) == 0);
    moved = None(CallStrong); CHECK(call_destroyed(25) == 1);
  }
  {
    const NestedStrong original = nested_make(26);
    NestedStrong copied = moveValue(original);
    copied = None(CallStrong); CHECK(!call_destroyed(26));
  }
  CHECK(call_destroyed(26) == 1);
  try { NestedStrong value = __objc3_try_force(nested_cleanup(27, 1)); return 204; }
  catch (int error) { CHECK(error == 42 && call_destroyed(27) == 1); }
  try {
    match(nested_make(28)) {
      case .Some(let child): { CHECK(hasValue(child)); throw 43; }
      case .None: { return 205; }
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
