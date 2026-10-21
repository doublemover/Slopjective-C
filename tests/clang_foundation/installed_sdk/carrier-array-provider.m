#include "carrier-array-call.h"
#include <objc/runtime.h>
#include <objc/encoding.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "weak block failure %d: %s\n", __LINE__, #x); return __LINE__; } } while (0)
ArrayWeakBlock array_weak_identity(ArrayWeakBlock value) { return value; }
ArrayUnsafeBlock array_unsafe_identity(ArrayUnsafeBlock value) { return value; }
static int weak_state(ArrayWeakBlock value, int invoke) {
  match(value) {
    case .Some(let block): { if (invoke && block) block(); return block == (CallAction)0; }
    case .None: { return -1; }
  }
  return -2;
}
static ArrayWeakBlock optionalized(ArrayWeakBlock value) throws(id<Error>) { return value; }
__attribute__((objc_root_class))
@interface ArrayWeakBlockCalls { Class isa; }
+ (ArrayWeakBlock)echo:(ArrayWeakBlock)value;
@end
@implementation ArrayWeakBlockCalls
+ (ArrayWeakBlock)echo:(ArrayWeakBlock)value { return value; }
@end
static CallAction make_action(int key) {
  id object = call_object(key);
  return ^{ if (!object) __builtin_trap(); };
}
static int global_count;
static CallAction global = ^{ ++global_count; };
static int reverse_destruction(void);
int array_weak_blocks(void) {
  CHECK(reverse_destruction() == 0);
  CHECK(strcmp(@encode(ArrayWeakBlock), @encode(ArrayUnsafeBlock)) != 0);
  CHECK(objc_sizeof_type(@encode(ArrayWeakBlock)) == sizeof(ArrayWeakBlock));
  {
    ArrayWeakBlock value = Some(CallAction __weak, global), copy = array_weak_identity(value);
    copy = [ArrayWeakBlockCalls echo:copy];
    CHECK(weak_state(copy, 1) == 0 && global_count == 1);
    ArrayWeakBlock values[2] = {value, copy};
    CHECK(weak_state(values[1], 1) == 0 && global_count == 2);
    typedef Result(CallAction __weak, CallAction __unsafe_unretained) MixedBlock;
    MixedBlock result = Ok(CallAction __weak, CallAction __unsafe_unretained, global);
    MixedBlock results[2] = {result, Err(CallAction __weak, CallAction __unsafe_unretained, global)};
    CHECK(isOk(results[0]) && !isOk(results[1]));
  }
  ArrayWeakBlock weak = None(CallAction __weak);
  ArrayUnsafeBlock unsafe = None(CallAction __unsafe_unretained);
  {
    id object = call_object(40);
    CallAction action = ^{ if (!object) __builtin_trap(); };
    weak = Some(CallAction __weak, action);
    unsafe = array_unsafe_identity(Some(CallAction __unsafe_unretained, action));
    CHECK(weak_state(weak, 1) == 0 && !call_destroyed(40));
    action = (CallAction)0; object = (id)0;
  }
  CHECK(call_destroyed(40) == 1 && weak_state(weak, 0) == 1 && hasValue(unsafe));
  unsafe = None(CallAction __unsafe_unretained);
  {
    CallAction action = make_action(41);
    weak = array_weak_identity(Some(CallAction __weak, action));
    int (^captured)(void) = ^{ return weak_state(weak, 0); };
    CHECK(captured() == 0);
    action = (CallAction)0;
    CHECK(call_destroyed(41) == 1 && captured() == 1);
  }
  {
    ArrayWeakBlock value = Some(CallAction __weak, global);
    Optional(ArrayWeakBlock) nested = tryOptional(optionalized(value));
    CHECK(hasValue(nested));
#ifdef __cplusplus
    ArrayWeakBlock moved = moveValue(value);
    CHECK(weak_state(value, 0) == 1 && weak_state(moved, 0) == 0);
#endif
  }
  return 0;
}

ArrayWeakState array_capture_weak(ArrayWeakBlock value) {
  return ^{ return weak_state(value, 0); };
}
int array_provider(int key) {
  CallStrong values[2][2] = {{call_make(key)}, {None(id)}};
  return hasValue(values[0][0]);
}

static int array_order;
__attribute__((objc_root_class))
@interface ArrayOrderProbe { Class isa; int key; }
+ (id)newWithKey:(int)value;
@end
@implementation ArrayOrderProbe
+ (id)newWithKey:(int)value {
  ArrayOrderProbe *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (void)dealloc { array_order = 10 * array_order + key; object_dispose(self); }
@end
static CallStrong order_value(int key) {
  return Some(id, [ArrayOrderProbe newWithKey:key]);
}
static CallStrong order_failure(void) { @throw [ArrayOrderProbe newWithKey:9]; }
static int reverse_destruction(void) {
  array_order = 0;
  { CallStrong values[2][2] = {{order_value(1), order_value(2)}, {order_value(3), order_value(4)}}; }
  CHECK(array_order == 4321);
  array_order = 0;
  @autoreleasepool {
    @try { CallStrong values[3] = {order_value(1), order_value(2), order_failure()}; CHECK(0); }
    @catch (id error) { CHECK(error != (id)0 && array_order == 21); }
  }
  CHECK(array_order == 219);
  return 0;
}
