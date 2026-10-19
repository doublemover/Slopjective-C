#include "carrier-capture-call.h"
CapturedState capture_strong(int key) {
  NestedStrong value = nested_make(key);
  return ^{ return nested_state(value); };
}
CapturedState capture_weak(id object) {
  NestedWeak value = Some(CallWeak, Some(id __weak, object));
  return ^{ return nested_weak_state(value); };
}
CapturedState capture_block_payload(int key) {
  NestedBlock value = Some(CallBlock, call_block(call_object(key)));
  return ^int {
    match(value) {
      case .Some(let child): {
        match(child) {
          case .Some(let action): { action(); return 1; }
          case .None: { return 0; }
        }
      }
      case .None: { return 0; }
    }
    return -1;
  };
}
CapturedState capture_binding(int key) {
  match(nested_make(key)) {
    case .Some(let child): { return ^int { return hasValue(child); }; }
    case .None: { return ^{ return 0; }; }
  }
  return (CapturedState)0;
}
CapturedState capture_nested_block(int key) {
  NestedStrong value = nested_make(key);
  CapturedState (^factory)(void) = ^CapturedState { return ^{ return nested_state(value); }; };
  return factory();
}
CapturedValue capture_value(int key) {
  NestedStrong value = nested_make(key);
  return ^NestedStrong { return value; };
}
CapturedThrowing capture_throwing(int key) {
  NestedStrong value = nested_make(key);
  return ^NestedStrong(int replacement, int fail) __objc3_throws(id<Error>) {
    return __objc3_try(nested_forward(value, replacement, fail));
  };
}
static int run_noescape(int (^body)(void) __attribute__((noescape))) { return body(); }
int capture_noescape(int key) {
  NestedStrong value = nested_make(key);
  int result = run_noescape(^{ return nested_state(value); });
  if (call_destroyed(key)) __builtin_trap();
  return result;
}
id capture_native_object(int key) { return call_object(key); }
void capture_unwind_bridge(int key) {
#ifdef __cplusplus
  // This search frame creates the C++ adapter, but its int handler cannot
  // catch the Objective-C object. The outer .m handler owns the final catch.
  try { capture_native_throw(key); }
  catch (int error) { __builtin_trap(); }
#else
  capture_native_throw(key);
#endif
}
#ifdef __cplusplus
static int copies;
struct alignas(8) ThrowsCopy {
  int mode, objective_c;
  ThrowsCopy(int mode, int objective_c) : mode(mode), objective_c(objective_c) {}
  ThrowsCopy(const ThrowsCopy &other) : mode(other.mode), objective_c(other.objective_c) {
    if (++copies == mode) {
      if (objective_c) @throw call_object(32);
      throw 42;
    }
  }
  ~ThrowsCopy() {}
};
CapturedState capture_partial(int key, int mode, int objective_c) {
  copies = 0;
  NestedStrong value = nested_make(key);
  ThrowsCopy other(mode, objective_c);
  return ^{ int state = nested_state(value); return state + other.mode - mode; };
}
#endif
