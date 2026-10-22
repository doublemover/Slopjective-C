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
