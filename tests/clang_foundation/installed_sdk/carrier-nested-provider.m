#include "carrier-nested-call.h"
#include <objc/runtime.h>
NestedStrong nested_make(int key) { return __objc3_some(CallStrong, call_make(key)); }
NestedStrong nested_identity(NestedStrong value) { return value; }
NestedMixed nested_mixed(NestedMixed value) { return value; }
NestedDeep nested_deep(NestedDeep value) { return value; }
NestedStrong nested_throwing(NestedStrong value, int key, int fail) __objc3_throws(id<Error>) {
  CallStrong inner = __objc3_try(throws_value(call_make(key), key, fail));
  if (!__objc3_has_value(value)) return __objc3_some(CallStrong, inner);
  return value;
}
NestedStrong nested_forward(NestedStrong value, int key, int fail) __objc3_throws(id<Error>) {
  return __objc3_try(nested_throwing(value, key, fail));
}
int nested_state(NestedStrong value) {
  __objc3_match_stmt(value) {
    case __objc3_case_some(let child): {
      __objc3_match_stmt(child) {
        case __objc3_case_some(let object): { return object ? 0 : 1; }
        case __objc3_case_none: { return 2; }
      }
    }
    case __objc3_case_none: { return 3; }
  }
  return 4;
}
int nested_weak_state(NestedWeak value) {
  __objc3_match_stmt(value) {
    case __objc3_case_some(let child): {
      __objc3_match_stmt(child) {
        case __objc3_case_some(let object): { return object ? 0 : 1; }
        case __objc3_case_none: { return 2; }
      }
    }
    case __objc3_case_none: { return 3; }
  }
  return 4;
}
@implementation NestedCalls
+ (id)new { return class_createInstance(self, 0); }
- (NestedStrong)echo:(NestedStrong)value { return value; }
- (NestedMixed)mixed:(NestedMixed)value { return value; }
- (NestedStrong)direct:(NestedStrong)value { return value; }
- (NestedStrong)throwing:(NestedStrong)value key:(int)key fail:(int)fail __objc3_throws(id<Error>) {
  return __objc3_try(nested_forward(value, key, fail));
}
- (NestedStrong)native:(NestedStrong)value { value = nested_make(30); @throw call_object(31); }
- (void)dealloc { object_dispose(self); }
@end
#ifdef __cplusplus
NestedStrong NestedDerived::apply(NestedStrong value) { return value; }
struct NestedExit {
  int mode;
  ~NestedExit() noexcept(false) {
    if (mode) throw 42;
  }
};
NestedStrong nested_cleanup(int key, int fail) __objc3_throws(id<Error>) {
  NestedExit cleanup{fail};
  return nested_make(key);
}
#endif
