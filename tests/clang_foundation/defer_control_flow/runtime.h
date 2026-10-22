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
