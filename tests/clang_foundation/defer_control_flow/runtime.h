#pragma once
#ifndef __cplusplus
#define bool _Bool
#endif
#ifdef __cplusplus
extern "C" {
#endif
id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
void objc_release(__unsafe_unretained id);
void *_Block_copy(const void *);
void _Block_release(const void *);
#ifdef __cplusplus
}
#endif
@protocol Error @end
__attribute__((objc_root_class))
@interface DeferredError <Error> { Class isa; }
+ (id)new;
@end
@implementation DeferredError
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc { object_dispose(self); }
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end
static id<Error> __unsafe_unretained token;
static int events, bad;
static void record(int digit) __attribute__((nothrow));
static void record(int digit) { events = 10 * events + digit; }
static void first(void) __attribute__((nothrow));
static void second(void) __attribute__((nothrow));
static void first(void) { record(1); }
static void second(void) { record(2); }
static void expect(int value) __attribute__((nothrow));
static void expect(int value) { if(events != value) bad = 1; events = 0; }
__attribute__((noreturn)) static void objc_throw(void) { record(3); @throw token; }
static int normal_return(void) { defer { first(); } second(); return 7; }
static void objc_unwind(void) {
  @try { defer { first(); } objc_throw(); }
  @catch(id error) { if(error != token) bad = 2; second(); }
}
static void objc_normal_cleanup(void) {
  @try { defer { objc_throw(); } return; }
  @catch(id error) { if(error != token) bad = 3; second(); }
}
static void objc_outside_registration(void) {
  defer { objc_throw(); }
  @try { return; } @catch(id error) { second(); }
}
static void objc_local_catch(void) {
  defer { @try { objc_throw(); } @catch(id error) { first(); } }
  @throw token;
}
static void checked_cleanup(void) {
  defer { first(); }
  handle { defer { second(); } throwError(token); }
  catch(id<Error> error) { if(error != token)bad=4; record(4); }
  record(5);
}
static void maybe_throw(bool fail) { if(fail)objc_throw(); record(5); }
static void consume(int *values) __attribute__((nothrow));
static void consume(int *values) { if(values[0] != 3)bad=5; first(); }
static void copied_body(bool early, bool fail, int count) {
  defer {
    int values[count];
    values[0] = count;
    goto end;
  label:
    consume(values);
    goto done;
  end:
    goto label;
  done:
    second();
  }
  if(early)return;
  maybe_throw(fail);
}
static void objc_double_exception(void) {
  @try { defer { objc_throw(); } @throw token; }
  @catch(id error) { bad=6; }
}
#ifdef __cplusplus
__attribute__((noreturn)) static void cpp_throw(void) { record(3); throw 42; }
static void cpp_unwind(void) {
  try { defer { first(); } cpp_throw(); }
  catch(int value) { if(value != 42)bad=7; second(); }
}
static void cpp_normal_cleanup(void) {
  try { defer { cpp_throw(); } return; }
  catch(int value) { if(value != 42)bad=8; second(); }
}
static void cpp_outside_registration(void) {
  defer { cpp_throw(); }
  try { return; } catch(int value) { second(); }
}
static void cpp_local_catch(void) {
  defer { try { cpp_throw(); } catch(int value) { first(); } }
  throw 7;
}
static void cpp_double_exception(void) {
  try { defer { cpp_throw(); } throw 7; }
  catch(int value) { bad=9; }
}
#endif

// Scalar resource ownership uses the same registered action ordering under
// ARC and MRC. Close callbacks exercise both value and address signatures.
static void close_resource(int value) { record(value); }
static int close_resource_status(int value) { record(value); return -1; }
static void close_resource_address(int *value) { record(*value); *value=9; }
static void close_resource_pointer(void *value) { record(*(int *)value); }
static void close_resource_pointer_address(void **value) {
  record(*(int *)*value); *value=(void *)0;
}
static int *resource_observed;
static void resource_normal(void) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  int newer __attribute__((objc_resource(close=close_resource_status, invalid=-1))) = 2;
}
static void resource_invalid(void) {
  int absent __attribute__((objc_resource(close=close_resource, invalid=-1))) = -1;
  void *nothing __attribute__((objc_resource(close=close_resource_pointer, invalid=0))) = (void *)0;
}
static void resource_final_value(void) {
  int value __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  value=3;
}
static void resource_defer_actions(void) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  defer { second(); }
  int newer __attribute__((objc_resource(close=close_resource, invalid=0))) = 3;
}
static void resource_invalidated(void) {
  defer { if(*resource_observed != -1)bad=50; second(); }
  @resource(close_resource_address, invalid: -1) int value = 3;
  resource_observed=&value;
}
static void resource_pointer(void) {
  int digit=3;
  void *value __attribute__((objc_resource(close=close_resource_pointer_address, invalid=0))) = &digit;
}
static void resource_storage(int count) {
  int *value __attribute__((objc_resource(close=close_resource_pointer, invalid=0))) = (int *)0;
  int digits[count]; digits[0]=3;
  value=digits;
}
static void resource_backward(void) {
  int count=0;
again:;
  @resource(close_resource, invalid: 0) int value = 3;
  if(count++ == 0)goto again;
  second();
}
static void resource_forward(void) {
  {
    @resource(close_resource, invalid: 0) int value = 3;
    goto after;
  }
after: second();
}
static void resource_loop(void) {
  for(int index=0; index!=2; ++index) {
    @resource(close_resource, invalid: 0) int value = 3;
    if(index == 0)continue;
    break;
  }
}
static int resource_acquire(bool fail) {
  second();
  if(fail)@throw token;
  return 3;
}
static void resource_initialization(bool fail) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  int newer __attribute__((objc_resource(close=close_resource, invalid=0))) = resource_acquire(fail);
}
static void resource_checked(void) {
  handle {
    int value __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
    throwError(token);
  } catch(id<Error> error) { if(error != token)bad=51; second(); }
}
static void close_resource_throwing(int value) { record(value); @throw token; }
static void resource_throwing_close(void) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  int newer __attribute__((objc_resource(close=close_resource_throwing, invalid=0))) = 3;
}
static int (^resource_block)(void);
static void resource_byref(bool keep) {
  __block @resource(close_resource_address, invalid: -1) int value = 3;
#if __has_feature(objc_arc)
  resource_block=^{ return value; };
#else
  resource_block=(__typeof__(resource_block))_Block_copy((const void *)^{ return value; });
#endif
  value=4;
  if(!keep) {
#if !__has_feature(objc_arc)
    _Block_release((const void *)resource_block);
#endif
    resource_block=(id)0;
  }
}
#ifdef __cplusplus
struct ResourceAction { ~ResourceAction() { second(); } };
static void resource_cpp_actions(bool fail) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  ResourceAction action;
  int newer __attribute__((objc_resource(close=close_resource, invalid=0))) = 3;
  if(fail)throw 42;
}
#endif
static void resource_take_operation(void) {
  @resource(close_resource, invalid: 0) int value = 3;
  int taken = take(value);
  if(taken != 3 || value != 0)bad=60;
  second(); record(taken);
}
static void resource_discard_operation(void) {
  @resource(close_resource_address, invalid: -1) int value = 3;
  discard(value);
  if(value != -1)bad=61;
  second();
}
static int resource_replacement(int *value) {
  if(*value != -1)bad=62;
  second(); return 3;
}
static void resource_reset_operation(void) {
  int value __attribute__((objc_resource(close=close_resource_address, invalid=-1))) = 1;
  reset(value, resource_replacement(&value));
  if(value != 3)bad=63;
}
static int resource_replacement_throw(void) { second(); @throw token; }
static void resource_reset_rhs_exception(void) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 4;
  int value __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  reset(value, resource_replacement_throw());
  bad=64;
}
static void resource_immediate_close_exception(bool reset) {
  int older __attribute__((objc_resource(close=close_resource, invalid=0))) = 1;
  int value __attribute__((objc_resource(close=close_resource_throwing, invalid=0))) = 3;
  if(reset)reset(value, 4);
  else discard(value);
  bad=65;
}
#ifdef __cplusplus
template<int Value> static int resource_template_take(void) {
  int value __attribute__((objc_resource(close=close_resource, invalid=0))) = Value;
  return take(value);
}
#endif
// Plain cleanup is unconditional and preserves the callback's final mutation.
static int cleanup_zero_calls;
static void cleanup_zero(int value) {
  if(value != 0)bad=70;
  ++cleanup_zero_calls;
}
static void cleanup_value_actions(bool fail) {
  @cleanup(close_resource) int older = 1;
  defer { second(); }
  @cleanup(close_resource_status) int newer = 0;
  newer = 3;
  if(fail)@throw token;
}
static void cleanup_unconditional(void) {
  @cleanup(cleanup_zero) int value = 0;
}
static void cleanup_address_final(void) {
  defer { if(*resource_observed != 9)bad=71; second(); }
  @cleanup(close_resource_address) int value = 3;
  resource_observed = &value;
}
static void cleanup_initialization(bool fail) {
  @cleanup(close_resource) int older = 1;
  @cleanup(close_resource) int newer = resource_acquire(fail);
}
static void cleanup_checked(void) {
  handle {
    @cleanup(close_resource) int value = 1;
    throwError(token);
  } catch(id<Error> error) { if(error != token)bad=72; second(); }
}
static void cleanup_throwing_callback(void) {
  @cleanup(close_resource) int older = 1;
  @cleanup(close_resource_throwing) int newer = 3;
}
static void cleanup_byref(bool keep) {
  __block @cleanup(close_resource_address) int value = 3;
#if __has_feature(objc_arc)
  resource_block=^{ return value; };
#else
  resource_block=(__typeof__(resource_block))_Block_copy((const void *)^{ return value; });
#endif
  value=4;
  if(!keep) {
#if !__has_feature(objc_arc)
    _Block_release((const void *)resource_block);
#endif
    resource_block=(id)0;
  }
}
#ifdef __cplusplus
struct CleanupUncopyable {
  CleanupUncopyable() = default;
  CleanupUncopyable(const CleanupUncopyable &) = delete;
  ~CleanupUncopyable() { second(); }
};
static void cleanup_reference(CleanupUncopyable &) { record(3); }
static void cleanup_cpp_actions(bool fail) {
  defer { first(); }
  @cleanup(cleanup_reference) CleanupUncopyable value;
  if(fail)throw 42;
}
#endif
static void cleanup_local_jump(void) {
  void *target[5];
  @cleanup(close_resource) int value = 1;
  defer { second(); }
  if(__builtin_setjmp(target) == 0) __builtin_longjmp(target, 1);
  record(3);
}
static void cleanup_ordering(void) {
  cleanup_local_jump(); expect(321);
  cleanup_unconditional();
  if(cleanup_zero_calls != 1)bad=73;
  cleanup_value_actions(0); expect(321);
  @try { cleanup_value_actions(1); }
  @catch(id error) { if(error != token)bad=74; record(4); }
  expect(3214);
  cleanup_address_final(); expect(32);
  cleanup_initialization(0); expect(231);
  @try { cleanup_initialization(1); }
  @catch(id error) { if(error != token)bad=75; record(4); }
  expect(214);
  cleanup_checked(); expect(12);
  @try { cleanup_throwing_callback(); }
  @catch(id error) { if(error != token)bad=76; second(); }
  expect(312);
  cleanup_byref(0); expect(4);
  cleanup_byref(1); expect(4);
  if(resource_block() != 9)bad=77;
#if !__has_feature(objc_arc)
  _Block_release((const void *)resource_block);
#endif
  resource_block=(id)0;
#ifdef __cplusplus
  cleanup_cpp_actions(0); expect(321);
  try { cleanup_cpp_actions(1); }
  catch(int value) { if(value != 42)bad=78; record(4); }
  expect(3214);
#endif
}
static void resource_ordering(void) {
  cleanup_ordering();
  resource_take_operation(); expect(23);
  resource_discard_operation(); expect(32);
  resource_reset_operation(); expect(123);
  @try { resource_reset_rhs_exception(); }
  @catch(id error) { if(error != token)bad=66; record(5); }
  expect(1245);
  for(int reset=0; reset!=2; ++reset) {
    @try { resource_immediate_close_exception(reset); }
    @catch(id error) { if(error != token)bad=67; second(); }
    expect(312);
  }
#ifdef __cplusplus
  if(resource_template_take<3>() != 3)bad=68;
  expect(0);
#endif
  resource_normal(); expect(21);
  resource_invalid(); expect(0);
  resource_final_value(); expect(3);
  resource_defer_actions(); expect(321);
  resource_invalidated(); expect(32);
  resource_pointer(); expect(3);
  resource_storage(7); expect(3);
  resource_backward(); expect(323);
  resource_forward(); expect(32);
  resource_loop(); expect(33);
  resource_initialization(0); expect(231);
  @try { resource_initialization(1); }
  @catch(id error) { if(error != token)bad=52; record(4); }
  expect(214);
  resource_checked(); expect(12);
  @try { resource_throwing_close(); }
  @catch(id error) { if(error != token)bad=53; second(); }
  expect(312);
  resource_byref(0); expect(4);
  resource_byref(1); expect(4);
  if(resource_block() != -1)bad=54;
#if !__has_feature(objc_arc)
  _Block_release((const void *)resource_block);
#endif
  resource_block=(id)0;
#ifdef __cplusplus
  resource_cpp_actions(0); expect(321);
  try { resource_cpp_actions(1); }
  catch(int value) { if(value != 42)bad=55; record(4); }
  expect(3214);
#endif
}

#if __has_feature(objc_arc)
__attribute__((objc_root_class))
@interface DeferredProbe { Class isa; int digit; }
+ (id)newWithDigit:(int)value;
@end
@implementation DeferredProbe
+ (id)newWithDigit:(int)value {
  DeferredProbe *result = class_createInstance(self, 0);
  result->digit = value;
  return result;
}
- (void)dealloc { record(digit); object_dispose(self); }
@end
static __weak DeferredProbe *observed;
static void observe(DeferredProbe *value) { observed = value; }
static void assert_alive(void) { if(!observed)bad=21; first(); }
static void arc_normal(void) {
  defer { assert_alive(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3];
  observe(value);
}
static void arc_precise_return(void) {
  defer { assert_alive(); }
  DeferredProbe *value __attribute__((objc_precise_lifetime)) =
      [DeferredProbe newWithDigit:3];
  observe(value);
  return;
}
static void arc_before(void) {
  DeferredProbe *value = [DeferredProbe newWithDigit:3];
  observe(value);
  defer { assert_alive(); }
}
static void arc_multiple(void) {
  defer { first(); }
  DeferredProbe *a = [DeferredProbe newWithDigit:3]; observe(a);
  defer { assert_alive(); second(); }
  DeferredProbe *b = [DeferredProbe newWithDigit:4]; observe(b);
}
static void arc_nested(void) {
  defer { first(); }
  DeferredProbe *a = [DeferredProbe newWithDigit:3]; observe(a);
  { defer { second(); }
    DeferredProbe *b = [DeferredProbe newWithDigit:4]; observe(b); }
  record(5);
}
static void arc_backward_retained(void) {
  int count = 0;
  defer { first(); }
again:;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(count++ == 0)goto again;
  second();
}
static void arc_backward_exiting(void) {
  int count = 0;
again:;
  defer { first(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(count++ == 0)goto again;
  second();
}
static void arc_forward_retained(void) {
  defer { first(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  goto after;
after: second();
}
static void arc_forward_exiting(void) {
  { defer { first(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    goto after; }
after: second();
}
static void arc_sibling(bool left) {
  if(left) {
    defer { first(); }
    DeferredProbe *a = [DeferredProbe newWithDigit:3]; observe(a);
    goto after;
  } else {
    defer { second(); }
    DeferredProbe *b = [DeferredProbe newWithDigit:4]; observe(b);
    goto after;
  }
after: record(5);
}
static void arc_loop(void) {
  for(int i=0;i<2;++i) {
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    if(i == 0)continue;
    break;
  }
  second();
}
static void arc_assignment(void) {
  defer { first(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  value = [DeferredProbe newWithDigit:4]; observe(value);
}
static void arc_explicit_release(void) {
  defer { first(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  value = (id)0;
}
static void arc_native_throw(void) {
  @try {
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    record(5); @throw token;
  } @catch(id error) { if(error != token)bad=22; second(); }
}
static void arc_throwing_action(void) {
  @try {
    defer { assert_alive(); @throw token; }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  } @catch(id error) { if(error != token)bad=23; second(); }
}
static void arc_older_action_after_throw(void) {
  @try {
    defer { assert_alive(); }
    defer { second(); @throw token; }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  } @catch(id error) { if(error != token)bad=24; record(4); }
}
static void arc_checked(void) {
  handle {
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    throwError(token);
  } catch(id<Error> error) { if(error != token)bad=25; second(); }
}
static void arc_block(void) {
  defer { assert_alive(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  DeferredProbe *captured = value;
  void (^block)(void) = ^{ observe(captured); };
  value = (id)0;
  captured = (id)0;
  block();
}
#ifdef __cplusplus
static void arc_cpp_unwind(void) {
  try {
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    throw 42;
  } catch(int error) { if(error != 42)bad=26; second(); }
}
#endif
static void (^saved_block)(void);
static void arc_array(bool fail) {
  defer { assert_alive(); }
  DeferredProbe *values[2] = {[DeferredProbe newWithDigit:3],
                             [DeferredProbe newWithDigit:4]};
  observe(values[1]);
  if(fail)@throw token;
}
static void arc_byref(bool fail) {
  defer { assert_alive(); }
  __block DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  saved_block = ^{ observe(value); };
  saved_block(); saved_block = (id)0;
  if(fail)@throw token;
}
static void arc_carrier(bool fail) {
  defer { assert_alive(); }
  DeferredProbe *source = [DeferredProbe newWithDigit:3]; observe(source);
  __objc3_optional(DeferredProbe *) value = __objc3_some(DeferredProbe *, source);
  source = (id)0;
  (void)&value;
  if(fail)@throw token;
}
struct DeferredHolder { DeferredProbe *value; };
static void arc_record(bool fail) {
  defer { assert_alive(); }
  struct DeferredHolder holder = {[DeferredProbe newWithDigit:3]};
  observe(holder.value);
  if(fail)@throw token;
}
struct DeferredCarrierHolder { __objc3_optional(DeferredProbe *) value; };
static void arc_nested_carrier(bool fail) {
  defer { assert_alive(); }
  DeferredProbe *source = [DeferredProbe newWithDigit:3]; observe(source);
  struct DeferredCarrierHolder holder = {__objc3_some(DeferredProbe *, source)};
  source = (id)0;
  (void)&holder;
  if(fail)@throw token;
}
static void arc_carrier_array(bool fail) {
  defer { assert_alive(); }
  DeferredProbe *source = [DeferredProbe newWithDigit:3]; observe(source);
  __objc3_optional(DeferredProbe *) values[2] = {
      __objc3_none(DeferredProbe *), __objc3_some(DeferredProbe *, source)};
  source = (id)0;
  (void)&values;
  if(fail)@throw token;
}
static void release_marker(int *value) { record(*value); }
static void arc_cleanup_attribute(void) {
  defer { assert_alive(); }
  int marker __attribute__((cleanup(release_marker))) = 2;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
}
static void close_resource_observed(int value) {
  if(!observed)bad=56;
  record(value);
}
static void arc_cleanup_value_only(bool fail) {
  @cleanup(close_resource_observed) int marker = 1;
  DeferredProbe *value=[DeferredProbe newWithDigit:3]; observe(value);
  if(fail)@throw token;
}
static void arc_resource_only(bool fail) {
  int resource __attribute__((objc_resource(close=close_resource_observed, invalid=0))) = 1;
  DeferredProbe *value=[DeferredProbe newWithDigit:3]; observe(value);
  if(fail)@throw token;
}
// Cleanup attributes register source actions in their own right. None of
// these scopes contains a defer to supply an incidental release-queue owner.
static void release_observed_marker(int *value) {
  if(!observed)bad=36;
  record(*value);
}
static void arc_cleanup_only(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(fail)@throw token;
}
static void arc_cleanup_only_multiple(void) {
  int first_marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *first_value = [DeferredProbe newWithDigit:3];
  int last_marker __attribute__((cleanup(release_observed_marker))) = 2;
  DeferredProbe *last_value = [DeferredProbe newWithDigit:4]; observe(last_value);
  (void)first_value;
}
static void arc_cleanup_only_mutated(void) {
  int marker __attribute__((cleanup(release_observed_marker))) = 2;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  marker = 1;
}
static void arc_cleanup_only_backward(void) {
  int count = 0;
again:;
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(count++ == 0)goto again;
  second();
}
static void arc_cleanup_only_forward(void) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  goto after;
after: second();
}
static void arc_cleanup_only_nested(void) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *outer = [DeferredProbe newWithDigit:3];
  {
    int inner_marker __attribute__((cleanup(release_observed_marker))) = 2;
    DeferredProbe *inner = [DeferredProbe newWithDigit:4]; observe(inner);
  }
  observe(outer);
  record(5);
}
static void arc_cleanup_only_loop(void) {
  for(int index=0; index!=2; ++index) {
    int marker __attribute__((cleanup(release_observed_marker))) = 1;
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    if(index == 0)continue;
    break;
  }
}
static void arc_cleanup_only_array(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *values[2] = {[DeferredProbe newWithDigit:3],
                             [DeferredProbe newWithDigit:4]};
  observe(values[1]);
  if(fail)@throw token;
}
static void arc_cleanup_only_checked(void) {
  handle {
    int marker __attribute__((cleanup(release_observed_marker))) = 1;
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
    throwError(token);
  } catch(id<Error> error) { if(error != token)bad=37; second(); }
}
static void arc_cleanup_only_byref_carrier(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *source = [DeferredProbe newWithDigit:3];
  observe(source);
  __block __objc3_optional(DeferredProbe *) value =
      __objc3_some(DeferredProbe *, source);
  source = (id)0;
  saved_block = ^{ if(!__objc3_has_value(value))bad=38; };
  saved_block(); saved_block = (id)0;
  // Copying the block transfers the payload out of the stack cell. The
  // forwarding-cell owner must survive until the marker has run.
  if(fail)@throw token;
}
static void arc_cleanup_only_byref_record(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  __block struct DeferredHolder value = {[DeferredProbe newWithDigit:3]};
  saved_block = ^{ observe(value.value); };
  saved_block(); saved_block = (id)0;
  if(fail)@throw token;
}
static void arc_defer_byref_record(bool fail) {
  defer { assert_alive(); }
  __block struct DeferredHolder value = {[DeferredProbe newWithDigit:3]};
  saved_block = ^{ observe(value.value); };
  saved_block(); saved_block = (id)0;
  if(fail)@throw token;
}
static void release_observed_probe(DeferredProbe *__strong *value) {
  (void)value;
  if(!observed)bad=45;
  first();
}
static void arc_cleanup_for_initializer(void) {
  int count = 0;
  for(DeferredProbe *first_value __attribute__((cleanup(release_observed_probe))) =
          [DeferredProbe newWithDigit:3],
      *last_value = [DeferredProbe newWithDigit:4]; count != 1; ++count) {
    (void)first_value;
    observe(last_value);
    second();
  }
}
static void arc_copied_body(void) {
  defer { assert_alive(); }
  defer {
    int local = 0;
  again:
    if(local++ == 0)goto again;
    second();
  }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
}
#ifdef __cplusplus
struct DeferredAction { int digit; ~DeferredAction() { record(digit); } };
class ImplicitPrivateOwner {
  DeferredProbe *value;
public:
  explicit ImplicitPrivateOwner(bool fail) : value([DeferredProbe newWithDigit:3]) {
    second();
    if(fail)throw 42;
  }
  void visit() const { observe(value); }
};
static void arc_cleanup_nonaggregate(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  ImplicitPrivateOwner value(false);
  value.visit();
  if(fail)throw 42;
}
static void arc_defer_nonaggregate(void) {
  defer { assert_alive(); }
  ImplicitPrivateOwner value(false);
  value.visit();
}
static void arc_cleanup_failed_constructor(void) {
  int marker __attribute__((cleanup(release_marker))) = 1;
  ImplicitPrivateOwner value(true);
  value.visit();
}
static void arc_cleanup_byref_nonaggregate(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  __block ImplicitPrivateOwner value(false);
  saved_block = ^{ value.visit(); };
  saved_block(); saved_block = (id)0;
  if(fail)throw 42;
}
static void arc_cleanup_lambda_capture(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredProbe *source = [DeferredProbe newWithDigit:3];
  auto closure = [source] { observe(source); };
  source = (id)0;
  second();
  closure();
  if(fail)throw 42;
}
static void arc_defer_lambda_capture(void) {
  defer { assert_alive(); }
  DeferredProbe *source = [DeferredProbe newWithDigit:3];
  auto closure = [source] { observe(source); };
  source = (id)0;
  second();
  closure();
}
static void arc_cleanup_if_initializer(bool flag) {
  if(DeferredProbe *first_value __attribute__((cleanup(release_observed_probe))) =
         [DeferredProbe newWithDigit:3],
     *last_value = [DeferredProbe newWithDigit:4]; flag) {
    (void)first_value; observe(last_value); second();
  } else {
    observe(last_value); second();
  }
}
static void arc_cleanup_switch_initializer(int choice) {
  switch(DeferredProbe *first_value __attribute__((cleanup(release_observed_probe))) =
             [DeferredProbe newWithDigit:3],
         *last_value = [DeferredProbe newWithDigit:4]; choice) {
  case 1: (void)first_value; observe(last_value); second(); break;
  default: observe(last_value); second(); break;
  }
}
static void arc_cleanup_folded_switch_initializer(void) {
  switch(DeferredProbe *first_value __attribute__((cleanup(release_observed_probe))) =
             [DeferredProbe newWithDigit:3],
         *last_value = [DeferredProbe newWithDigit:4]; 1) {
  case 1: (void)first_value; observe(last_value); second(); break;
  }
}
static void arc_cleanup_range_initializer(void) {
  int values[2] = {0, 1};
  for(DeferredProbe *first_value __attribute__((cleanup(release_observed_probe))) =
          [DeferredProbe newWithDigit:3],
      *last_value = [DeferredProbe newWithDigit:4]; int value : values) {
    (void)first_value;
    observe(last_value);
    if(value == 0)continue;
    second();
  }
}
struct CleanupTruth {
  DeferredProbe *value;
  explicit operator bool() const { return value != (id)0; }
};
static void release_truth(CleanupTruth *value) {
  if(value->value) { observe(value->value); first(); }
}
static void arc_cleanup_while_condition(void) {
  int count = 0;
  while(CleanupTruth value __attribute__((cleanup(release_truth))) =
            {count++ < 2 ? [DeferredProbe newWithDigit:3] : (id)0}) {
    if(count == 1)continue;
    second();
  }
}
static void arc_cleanup_for_condition(void) {
  int count = 0;
  for(; CleanupTruth value __attribute__((cleanup(release_truth))) =
            {count++ < 2 ? [DeferredProbe newWithDigit:3] : (id)0};) {
    if(count == 1)continue;
    second();
  }
}
static void arc_cleanup_only_cpp_action(bool fail) {
  int marker __attribute__((cleanup(release_observed_marker))) = 1;
  DeferredAction action{2};
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(fail)throw 42;
}
static void arc_cpp_action(bool fail) {
  defer { assert_alive(); }
  DeferredAction action{2};
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(fail)throw 42;
}
static void arc_cpp_partial_exit(void) {
  int count = 0;
again:;
  DeferredAction action{2};
  defer { first(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  if(count++ == 0)goto again;
  record(4);
}
#endif
static int *deferred_values;
static void arc_vla(int count) {
  defer { if(deferred_values[0] != 42)bad=35; first(); }
  int values[count]; values[0] = 42; deferred_values = values;
}
@interface DeferredProbe (Actions)
+ (void)exerciseOrdering;
@end
@implementation DeferredProbe (Actions)
+ (void)exerciseOrdering {
  defer { assert_alive(); }
  DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
}
@end
static void arc_callable_block(void) {
  void (^body)(void) = ^{
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  };
  body();
}
#ifdef __cplusplus
template<class T> static void arc_template(void) {
  defer { assert_alive(); }
  T value = [DeferredProbe newWithDigit:3]; observe(value);
}
static void arc_lambda(void) {
  [] {
    defer { assert_alive(); }
    DeferredProbe *value = [DeferredProbe newWithDigit:3]; observe(value);
  }();
}
#endif
static void arc_ordering(void) {
  arc_vla(7); expect(1);
  [DeferredProbe exerciseOrdering]; expect(13);
  arc_callable_block(); expect(13);
#ifdef __cplusplus
  arc_template<DeferredProbe *>(); expect(13);
  arc_lambda(); expect(13);
#endif
  arc_array(0); expect(143);
  @try { arc_array(1); } @catch(id error) { if(error != token)bad=28; second(); }
  expect(1432);
  arc_byref(0); expect(13);
  @try { arc_byref(1); } @catch(id error) { if(error != token)bad=29; second(); }
  expect(132);
  arc_carrier(0); expect(13);
  @try { arc_carrier(1); } @catch(id error) { if(error != token)bad=30; second(); }
  expect(132);
  arc_record(0); expect(13);
  @try { arc_record(1); } @catch(id error) { if(error != token)bad=31; second(); }
  expect(132);
  arc_nested_carrier(0); expect(13);
  @try { arc_nested_carrier(1); } @catch(id error) { if(error != token)bad=32; second(); }
  expect(132);
  arc_carrier_array(0); expect(13);
  @try { arc_carrier_array(1); } @catch(id error) { if(error != token)bad=33; second(); }
  expect(132);
  arc_cleanup_attribute(); expect(213);
  arc_cleanup_value_only(0); expect(13);
  @try { arc_cleanup_value_only(1); }
  @catch(id error) { if(error != token)bad=79; second(); }
  expect(132);
  arc_resource_only(0); expect(13);
  @try { arc_resource_only(1); }
  @catch(id error) { if(error != token)bad=57; second(); }
  expect(132);
  arc_cleanup_only(0); expect(13);
  @try { arc_cleanup_only(1); } @catch(id error) { if(error != token)bad=39; second(); }
  expect(132);
  arc_cleanup_only_multiple(); expect(2143);
  arc_cleanup_only_mutated(); expect(13);
  arc_cleanup_only_backward(); expect(13213);
  arc_cleanup_only_forward(); expect(213);
  arc_cleanup_only_nested(); expect(24513);
  arc_cleanup_only_loop(); expect(1313);
  arc_cleanup_only_array(0); expect(143);
  @try { arc_cleanup_only_array(1); } @catch(id error) { if(error != token)bad=40; second(); }
  expect(1432);
  arc_cleanup_only_checked(); expect(132);
  arc_cleanup_only_byref_carrier(0); expect(13);
  @try { arc_cleanup_only_byref_carrier(1); } @catch(id error) { if(error != token)bad=41; second(); }
  expect(132);
  arc_cleanup_only_byref_record(0); expect(13);
  @try { arc_cleanup_only_byref_record(1); } @catch(id error) { if(error != token)bad=42; second(); }
  expect(132);
  arc_defer_byref_record(0); expect(13);
  @try { arc_defer_byref_record(1); } @catch(id error) { if(error != token)bad=43; second(); }
  expect(132);
  arc_cleanup_for_initializer(); expect(2143);
  arc_copied_body(); expect(213);
#ifdef __cplusplus
  arc_cleanup_nonaggregate(0); expect(213);
  try { arc_cleanup_nonaggregate(1); }
  catch(int value) { if(value != 42)bad=46; record(4); }
  expect(2134);
  arc_defer_nonaggregate(); expect(213);
  try { arc_cleanup_failed_constructor(); }
  catch(int value) { if(value != 42)bad=47; record(4); }
  expect(2314);
  arc_cleanup_byref_nonaggregate(0); expect(213);
  try { arc_cleanup_byref_nonaggregate(1); }
  catch(int value) { if(value != 42)bad=48; record(4); }
  expect(2134);
  arc_cleanup_lambda_capture(0); expect(213);
  try { arc_cleanup_lambda_capture(1); }
  catch(int value) { if(value != 42)bad=49; record(4); }
  expect(2134);
  arc_defer_lambda_capture(); expect(213);
  arc_cleanup_if_initializer(0); expect(2143);
  arc_cleanup_if_initializer(1); expect(2143);
  arc_cleanup_switch_initializer(1); expect(2143);
  arc_cleanup_switch_initializer(0); expect(2143);
  arc_cleanup_folded_switch_initializer(); expect(2143);
  arc_cleanup_range_initializer(); expect(2143);
  arc_cleanup_while_condition(); expect(13213);
  arc_cleanup_for_condition(); expect(13213);
  arc_cleanup_only_cpp_action(0); expect(213);
  try { arc_cleanup_only_cpp_action(1); }
  catch(int value) { if(value != 42)bad=44; record(4); }
  expect(2134);
  arc_cpp_action(0); expect(213);
  try { arc_cpp_action(1); } catch(int value) { if(value != 42)bad=34; record(4); }
  expect(2134);
  arc_cpp_partial_exit(); expect(1234123);
#endif
  arc_normal(); expect(13);
  arc_precise_return(); expect(13);
  arc_before(); expect(13);
  arc_multiple(); expect(12143);
  arc_nested(); expect(24513);
  arc_backward_retained(); expect(3213);
  arc_backward_exiting(); expect(13213);
  arc_forward_retained(); expect(213);
  arc_forward_exiting(); expect(132);
  arc_sibling(1); expect(135);
  arc_sibling(0); expect(245);
  arc_loop(); expect(13132);
  arc_assignment(); expect(314);
  arc_explicit_release(); expect(31);
  arc_native_throw(); expect(5132);
  arc_throwing_action(); expect(132);
  arc_older_action_after_throw(); expect(2134);
  arc_checked(); expect(132);
  arc_block(); expect(13);
#ifdef __cplusplus
  arc_cpp_unwind(); expect(132);
#endif
}
#endif
