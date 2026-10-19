#include "carrier-call.h"
#define CHECK(test) do { if (!(test)) return __LINE__; } while (0)

static int strong_calls(void) {
  int present = __objc3_has_value(call_make(42));
  CHECK(present && call_destroyed(42) == 1);
  {
    CallStrong value = call_make(0);
    CHECK(call_destroyed(0) == 0 && __objc3_has_value(value));
    CallStrong copied = call_identity(value);
    value = __objc3_none(id);
    CHECK(call_destroyed(0) == 0 && __objc3_has_value(copied));
    copied = __objc3_none(id);
    CHECK(call_destroyed(0) == 1);
  }
  call_make(1);
  CHECK(call_destroyed(1) == 1);
  {
    CallStrong forwarded = call_forward(2);
    CallStrong (*callback)(CallStrong) = call_identity;
    CallStrong result = callback(forwarded);
    call_replace_parameter(forwarded, 3);
    CHECK(call_destroyed(3) == 1 && call_destroyed(2) == 0);
    forwarded = __objc3_none(id);
    result = call_make(4);
    CHECK(call_destroyed(2) == 1 && call_destroyed(4) == 0);
  }
  CHECK(call_destroyed(4) == 1);
  {
    CallStrong none = __objc3_none(id);
    CallStrong result = call_identity(none);
    CHECK(!__objc3_has_value(result));
    none = __objc3_some(id, (id)0);
    result = call_identity(none);
    CHECK(__objc3_has_value(result));
  }
  {
    CallStrong a = call_make(5);
    CallStrong b = call_make(6);
    CallStrong result = call_many(1, a, a, 2, a, a, a, a, b, 3);
    a = __objc3_none(id); b = __objc3_none(id);
    CHECK(call_destroyed(5) == 1 && call_destroyed(6) == 0);
    result = __objc3_none(id);
    CHECK(call_destroyed(6) == 1);
  }
  return 0;
}

static int other_leaves(void) {
  {
    id object = call_object(7);
    CallWeak source = __objc3_some(id __weak, object);
    CallWeak result = call_weak(source);
    object = (id)0;
    CHECK(call_destroyed(7) == 1);
    CHECK(__objc3_has_value(result) && __objc3_has_value(source));
    result = call_weak(result);
    CHECK(__objc3_has_value(result));
    result = __objc3_none(id __weak);
  }
  {
    id object = call_object(8);
    CallUnsafe source = __objc3_some(id __unsafe_unretained, object);
    CallUnsafe result = call_unsafe(source);
    object = (id)0;
    CHECK(call_destroyed(8) == 1 && __objc3_has_value(result));
  }
  for (int success = 0; success != 2; ++success) {
    int key = 9 + success;
    CallOutcome source = success ? __objc3_ok(id, id, call_object(key))
                                 : __objc3_err(id, id, call_object(key));
    CallOutcome result = call_outcome(source, 0);
    CHECK(__objc3_is_ok(result) == (success != 0));
    source = __objc3_ok(id, id, (id)0);
    CHECK(call_destroyed(key) == 0);
    result = call_outcome(result, 1);
    CHECK(__objc3_is_ok(result) != (success != 0));
    CHECK(call_destroyed(key) == 1);
  }
  CHECK(call_destroyed(20) == 1 && call_destroyed(21) == 1);
  {
    id object = call_object(11);
    CallMixed source = __objc3_err(id, id __weak, object);
    CallMixed result = call_mixed(source);
    object = (id)0;
    CHECK(call_destroyed(11) == 1 && !__objc3_is_ok(result));
    source = __objc3_ok(id, id __weak, call_object(12));
    result = call_mixed(source);
    source = __objc3_err(id, id __weak, (id)0);
    CHECK(call_destroyed(12) == 0 && __objc3_is_ok(result));
  }
  CHECK(call_destroyed(12) == 1);
  {
    CallBlock block = __objc3_none(CallAction);
    {
      id object = call_object(13);
      block = call_block(object);
      object = (id)0;
    }
    CallBlock copy = call_block_identity(block);
    block = __objc3_none(CallAction);
    CHECK(call_destroyed(13) == 0 && __objc3_has_value(copy));
    copy = __objc3_none(CallAction);
    CHECK(call_destroyed(13) == 1);
  }
  return 0;
}

#ifdef __cplusplus
static CallStrong mutate(CallStrong &source) {
  source = __objc3_none(id);
  return __objc3_some(id, (id)0);
}
static CallStrong throw_later(CallStrong &source) {
  source = __objc3_none(id);
  throw 43;
}
template <class Value> static Value template_call(Value value) {
  return call_identity(value);
}
struct OtherBase { virtual int other() { return 7; } };
struct Adjusted final : OtherBase, CallBase {
  CallStrong apply(CallStrong value) override { return template_call(value); }
};
static int exceptions_and_members(void) {
  {
    CallStrong original = call_make(30);
    CallDerived derived;
    CallBase *base = &derived;
    CallStrong result = base->apply(original);
    CallStrong (CallDerived::*member)(CallStrong) = &CallDerived::apply;
    result = (derived.*member)(original);
    original = __objc3_none(id);
    CHECK(call_destroyed(30) == 0);
    try { result = call_throw(result); }
    catch (int error) { CHECK(error == 42); }
    CHECK(call_destroyed(31) == 1 && call_destroyed(30) == 0);
    result = CallDerived::make(32);
    CHECK(call_destroyed(30) == 1);
  }
  CHECK(call_destroyed(32) == 1);
  {
    Adjusted adjusted;
    CallBase *base = &adjusted;
    CallStrong result = base->apply(call_make(35));
    CHECK(call_destroyed(35) == 0 && __objc3_has_value(result));
  }
  CHECK(call_destroyed(35) == 1);
  {
    // List initialization sequences argument evaluation left to right.
    struct Ordered {
      Ordered(CallStrong first, CallStrong second) {
        if (!__objc3_has_value(first) || call_destroyed(33)) __builtin_trap();
      }
    };
    CallStrong source = call_make(33);
    Ordered value{source, mutate(source)};
    CHECK(!__objc3_has_value(source) && call_destroyed(33) == 1);
  }
  {
    struct Throwing {
      Throwing(CallStrong first, CallStrong second) { __builtin_trap(); }
    };
    CallStrong source = call_make(34);
    try { Throwing value{source, throw_later(source)}; }
    catch (int error) { CHECK(error == 43); }
    CHECK(call_destroyed(34) == 1);
  }
  return 0;
}
#endif
int main(void) {
  int result;
  if ((result = strong_calls())) return result;
  if ((result = other_leaves())) return result;
#ifdef __cplusplus
  if ((result = exceptions_and_members())) return result;
#endif
  return 0;
}
