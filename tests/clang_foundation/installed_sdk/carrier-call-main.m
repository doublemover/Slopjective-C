#include "carrier-call.h"
#define CHECK(test) do { if (!(test)) return __LINE__; } while (0)

static int strong_calls(void) {
  int present = hasValue(call_make(42));
  CHECK(present && call_destroyed(42) == 1);
  {
    CallStrong value = call_make(0);
    CHECK(call_destroyed(0) == 0 && hasValue(value));
    CallStrong copied = call_identity(value);
    value = None(id);
    CHECK(call_destroyed(0) == 0 && hasValue(copied));
    copied = None(id);
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
    forwarded = None(id);
    result = call_make(4);
    CHECK(call_destroyed(2) == 1 && call_destroyed(4) == 0);
  }
  CHECK(call_destroyed(4) == 1);
  {
    CallStrong none = None(id);
    CallStrong result = call_identity(none);
    CHECK(!hasValue(result));
    none = Some(id, (id)0);
    result = call_identity(none);
    CHECK(hasValue(result));
  }
  {
    CallStrong a = call_make(5);
    CallStrong b = call_make(6);
    CallStrong result = call_many(1, a, a, 2, a, a, a, a, b, 3);
    a = None(id); b = None(id);
    CHECK(call_destroyed(5) == 1 && call_destroyed(6) == 0);
    result = None(id);
    CHECK(call_destroyed(6) == 1);
  }
  return 0;
}

static int other_leaves(void) {
  {
    id object = call_object(7);
    CallWeak source = Some(id __weak, object);
    CallWeak result = call_weak(source);
    object = (id)0;
    CHECK(call_destroyed(7) == 1);
    CHECK(hasValue(result) && hasValue(source));
    result = call_weak(result);
    CHECK(hasValue(result));
    result = None(id __weak);
  }
  {
    id object = call_object(8);
    CallUnsafe source = Some(id __unsafe_unretained, object);
    CallUnsafe result = call_unsafe(source);
    object = (id)0;
    CHECK(call_destroyed(8) == 1 && hasValue(result));
  }
  for (int success = 0; success != 2; ++success) {
    int key = 9 + success;
    CallOutcome source = success ? Ok(id, id, call_object(key))
                                 : Err(id, id, call_object(key));
    CallOutcome result = call_outcome(source, 0);
    CHECK(isOk(result) == (success != 0));
    source = Ok(id, id, (id)0);
    CHECK(call_destroyed(key) == 0);
    result = call_outcome(result, 1);
    CHECK(isOk(result) != (success != 0));
    CHECK(call_destroyed(key) == 1);
  }
  CHECK(call_destroyed(20) == 1 && call_destroyed(21) == 1);
  {
    id object = call_object(11);
    CallMixed source = Err(id, id __weak, object);
    CallMixed result = call_mixed(source);
    object = (id)0;
    CHECK(call_destroyed(11) == 1 && !isOk(result));
    source = Ok(id, id __weak, call_object(12));
    result = call_mixed(source);
    source = Err(id, id __weak, (id)0);
    CHECK(call_destroyed(12) == 0 && isOk(result));
  }
  CHECK(call_destroyed(12) == 1);
  {
    CallBlock block = None(CallAction);
    {
      id object = call_object(13);
      block = call_block(object);
      object = (id)0;
    }
    CallBlock copy = call_block_identity(block);
    block = None(CallAction);
    CHECK(call_destroyed(13) == 0 && hasValue(copy));
    copy = None(CallAction);
    CHECK(call_destroyed(13) == 1);
  }
  return 0;
}

#ifdef __cplusplus
static CallStrong mutate(CallStrong &source) {
  source = None(id);
  return Some(id, (id)0);
}
static CallStrong throw_later(CallStrong &source) {
  source = None(id);
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
    original = None(id);
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
    CHECK(call_destroyed(35) == 0 && hasValue(result));
  }
  CHECK(call_destroyed(35) == 1);
  {
    // List initialization sequences argument evaluation left to right.
    struct Ordered {
      Ordered(CallStrong first, CallStrong second) {
        if (!hasValue(first) || call_destroyed(33)) __builtin_trap();
      }
    };
    CallStrong source = call_make(33);
    Ordered value{source, mutate(source)};
    CHECK(!hasValue(source) && call_destroyed(33) == 1);
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
