#include "carrier-capture-call.h"
#include <stdio.h>
#include <string.h>
#ifdef __cplusplus
#include <exception>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"capture failure %d: %s\n",__LINE__,#x); return __LINE__; } } while (0)
static int values(void) {
  CapturedState block = capture_strong(0);
  CHECK(block() == 0 && !call_destroyed(0));
  block = (CapturedState)0; CHECK(call_destroyed(0) == 1);
  id object = call_object(1);
  block = capture_weak(object); CHECK(block() == 0);
  object = (id)0; CHECK(call_destroyed(1) == 1 && block() == 1);
  block = (CapturedState)0;
  block = capture_binding(2); CHECK(block() && !call_destroyed(2));
  block = (CapturedState)0; CHECK(call_destroyed(2) == 1);
  block = capture_nested_block(3); CHECK(block() == 0 && !call_destroyed(3));
  block = (CapturedState)0; CHECK(call_destroyed(3) == 1);
  block = capture_block_payload(4); CHECK(block() == 1 && !call_destroyed(4));
  block = (CapturedState)0; CHECK(call_destroyed(4) == 1);
  {
    NestedStrong source = nested_make(5);
    block = ^{ return nested_state(source); };
    source = __objc3_none(CallStrong);
    CHECK(block() == 0 && !call_destroyed(5));
  }
  // Stack-literal storage lives to the end of its enclosing scope; only the
  // heap copy survives here. Its release must destroy the selected capture.
  block = (CapturedState)0; CHECK(call_destroyed(5) == 1);
  {
    CapturedValue factory = capture_value(6);
    NestedStrong value = factory();
    factory = (CapturedValue)0;
    CHECK(nested_state(value) == 0 && !call_destroyed(6));
    value = __objc3_none(CallStrong); CHECK(call_destroyed(6) == 1);
  }
  CHECK(capture_noescape(7) == 0 && call_destroyed(7) == 1);
  return 0;
}
static int errors(void) {
  CapturedThrowing block = capture_throwing(8);
  {
    NestedStrong value = __objc3_try_force(block(9, 0));
    CHECK(nested_state(value) == 0 && !call_destroyed(8) && call_destroyed(9) == 1);
  }
  CHECK(!call_destroyed(8));
  @autoreleasepool {
    int caught = 0;
    __objc3_do { NestedStrong value = __objc3_try(block(10, 1)); }
    __objc3_catch(id<Error> error) { CHECK(error != (id)0); caught = 1; }
    CHECK(caught && !call_destroyed(8) && call_destroyed(10) == 2);
  }
  block = (CapturedThrowing)0; CHECK(call_destroyed(8) == 1);
  @autoreleasepool {
    @try { CapturedState local = capture_strong(11); @throw call_object(12); }
    @catch (id error) { CHECK(error != (id)0 && call_destroyed(11) == 1); }
  }
  CHECK(call_destroyed(12) == 1 && throws_created() == throws_destroyed());
  @autoreleasepool { CHECK(capture_native_catch(35) == 1); }
  CHECK(call_destroyed(35) == 1);
#ifdef __cplusplus
  CHECK(std::uncaught_exceptions() == 0);
  try { CapturedState local = capture_partial(30, 1, 0); return 100; }
  catch (int error) { CHECK(error == 42 && call_destroyed(30) == 1); }
  try { CapturedState local = capture_partial(31, 2, 0); return 101; }
  catch (int error) { CHECK(error == 42 && call_destroyed(31) == 1); }
  @autoreleasepool {
    @try { CapturedState local = capture_partial(33, 2, 1); return 102; }
    @catch (id error) { CHECK(error != (id)0 && call_destroyed(33) == 1); }
  }
  CHECK(call_destroyed(32) == 1);
  try { CapturedState local = capture_strong(34); throw 43; }
  catch (int error) { CHECK(error == 43 && call_destroyed(34) == 1); }
  CHECK(std::uncaught_exceptions() == 0);
#endif
  return 0;
}
int main(int argc, char **argv) {
  if (argc == 2 && !strcmp(argv[1], "force-capture")) {
    CapturedThrowing block = capture_throwing(42);
    NestedStrong value = __objc3_try_force(block(43, 1));
    return nested_state(value);
  }
  int result = values();
  return result ? result : errors();
}
