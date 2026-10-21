#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "carrier-direct-call.h"
#include <objc/runtime.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "direct test failed at %d: %s\n", __LINE__, #test); return __LINE__; } } while (0)
#define EXPECT_ERROR(statement) do { \
  int caught = 0, before = throws_created(); \
  @autoreleasepool { \
    handle { statement; CHECK(0); } \
    catch(id<Error> error) { CHECK(error); caught = 1; } \
  } \
  CHECK(caught && throws_created() == before + 1 && throws_destroyed() == throws_created()); \
} while (0)
static pthread_barrier_t start;
static void *initialize_thread(void *unused) {
  pthread_barrier_wait(&start);
  @autoreleasepool {
    for (int i = 0; i != 32; ++i) {
      CallStrong value = tryForce([DirectCalls echo:None(id) fail:0]);
      if (hasValue(value)) return (void *)1;
    }
  }
  return (void *)0;
}
int main(int argc, char **argv) {
  if (argc > 1) {
    if (!strcmp(argv[1], "force-class")) tryForce([DirectCalls echo:None(id) fail:1]);
    if (!strcmp(argv[1], "force-instance")) {
      DirectCalls *receiver = [DirectCalls new];
      tryForce([receiver echo:None(id) key:40 fail:1]);
    }
    return 99;
  }
  CHECK(direct_initialized() == 0);
  CHECK(!class_getClassMethod(objc_getClass("DirectCalls"), sel_registerName("echo:fail:")));
  CHECK(!class_getInstanceMethod(objc_getClass("DirectCalls"), sel_registerName("echo:key:fail:")));
  pthread_t threads[4];
  CHECK(!pthread_barrier_init(&start, 0, 4));
  for (int i = 0; i != 4; ++i) CHECK(!pthread_create(&threads[i], 0, initialize_thread, 0));
  for (int i = 0; i != 4; ++i) { void *result = 0; CHECK(!pthread_join(threads[i], &result) && !result); }
  CHECK(!pthread_barrier_destroy(&start) && direct_initialized() == 1);
  CallStrong source = call_make(0);
  CallStrong result = tryForce([DirectCalls echo:source fail:0]);
  source = None(id);
  CHECK(!call_destroyed(0) && hasValue(result));
  result = None(id); CHECK(call_destroyed(0) == 1);
  DirectCalls *receiver = [DirectCalls new];
  CHECK([receiver command]);
  source = call_make(1);
  result = tryForce([receiver echo:source key:2 fail:0]);
  source = None(id);
  CHECK(!call_destroyed(1) && hasValue(result));
  EXPECT_ERROR(result = tryValue([receiver echo:result key:2 fail:1]));
  CHECK(call_destroyed(2) == 1 && !call_destroyed(1) && hasValue(result));
  EXPECT_ERROR(tryValue([DirectCalls echo:None(id) fail:1]));
  result = None(id); CHECK(call_destroyed(1) == 2);
  DirectCalls *missing = (id)0;
  result = tryForce([missing echo:call_make(3) key:4 fail:1]);
  CHECK(!hasValue(result) && call_destroyed(3) == 1 && !call_destroyed(4));
  tryForce([missing echo:call_make(5) key:4 fail:1]); CHECK(call_destroyed(5) == 1);
  id object = call_object(6);
  CallWeak weak = Some(id __weak, object);
  CallWeak copy = tryForce([receiver weak:weak fail:0]);
  EXPECT_ERROR(copy = tryValue([receiver weak:weak fail:1]));
  object = (id)0; CHECK(call_destroyed(6) == 1 && hasValue(copy));
  copy = tryForce([missing weak:copy fail:1]); CHECK(!hasValue(copy));
  object = call_object(7);
  CallMixed mixed = Err(id, id __weak, object);
  CallMixed mixedCopy = tryForce([receiver mixed:mixed fail:0]);
  EXPECT_ERROR(mixedCopy = tryValue([receiver mixed:mixed fail:1]));
  object = (id)0; CHECK(call_destroyed(7) == 1 && !isOk(mixedCopy));
  mixedCopy = tryForce([missing mixed:Ok(id, id __weak, call_object(8)) fail:1]);
  CHECK(!isOk(mixedCopy) && call_destroyed(8) == 1);
  tryForce([missing mixed:mixedCopy fail:1]);
  CallBlock block = call_block(call_object(9));
  CallBlock blockCopy = tryForce([receiver block:block fail:0]);
  EXPECT_ERROR(blockCopy = tryValue([receiver block:block fail:1]));
  block = None(CallAction); CHECK(!call_destroyed(9));
  blockCopy = None(CallAction); CHECK(call_destroyed(9) == 1);
  @autoreleasepool {
    @try { tryForce([receiver native:call_make(10)]); CHECK(0); }
    @catch(id error) { CHECK(error && call_destroyed(10) == 1 && call_destroyed(11) == 1); }
  }
  CHECK(call_destroyed(12) == 1);
#ifdef __cplusplus
  result = call_make(13);
  try { result = tryForce([receiver cleanup:14]); CHECK(0); }
  catch (int error) { CHECK(error == 71 && call_destroyed(14) == 1 && !call_destroyed(13)); }
  result = None(id); CHECK(call_destroyed(13) == 1);
#endif
  CHECK(direct_initialized() == 1 && throws_created() == throws_destroyed());
  return 0;
}
