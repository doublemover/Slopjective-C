#include "carrier-call.h"
#include <objc/runtime.h>
static int destroyed[64];
__attribute__((objc_root_class))
@interface CallProbe { Class isa; @public int key; }
+ (id)newWithKey:(int)value;
@end
@implementation CallProbe
+ (id)newWithKey:(int)value {
  CallProbe *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (void)dealloc { ++destroyed[key]; object_dispose(self); }
@end

id call_object(int key) { return [CallProbe newWithKey:key]; }
int call_destroyed(int key) { return destroyed[key]; }
CallStrong call_make(int key) {
  id object = call_object(key);
  CallStrong local = Some(id, object);
  return local;
}
CallStrong call_identity(CallStrong value) {
  if (!hasValue(value)) return None(id);
  CallStrong copied = value;
  return copied;
}
CallStrong call_forward(int key) { return call_identity(call_make(key)); }
void call_replace_parameter(CallStrong value, int key) {
  value = call_make(key);
  if (!hasValue(value)) __builtin_trap();
}
CallWeak call_weak(CallWeak value) { return value; }
CallUnsafe call_unsafe(CallUnsafe value) { return value; }
CallOutcome call_outcome(CallOutcome value, int reverse) {
  if (!reverse) return value;
  if (isOk(value)) return Err(id, id, call_object(20));
  return Ok(id, id, call_object(21));
}
CallMixed call_mixed(CallMixed value) { return value; }
CallBlock call_block(id object) {
  CallAction action = ^{ if (!object) __builtin_trap(); };
  return Some(CallAction, action);
}
CallBlock call_block_identity(CallBlock value) { return value; }
CallStrong call_many(int first, CallStrong a, CallStrong b, int second,
                     CallStrong c, CallStrong d, CallStrong e, CallStrong f,
                     CallStrong g, int last) {
  if (first != 1 || second != 2 || last != 3 ||
      !hasValue(a) || !hasValue(b) ||
      !hasValue(c) || !hasValue(d) ||
      !hasValue(e) || !hasValue(f)) __builtin_trap();
  return g;
}
#ifdef __cplusplus
CallStrong call_throw(CallStrong value) {
  value = call_make(31);
  throw 42;
}
CallStrong CallDerived::apply(CallStrong value) { return call_identity(value); }
CallStrong CallDerived::make(int key) { return call_forward(key); }
#endif
