#include "carrier-throws-call.h"
#include <objc/runtime.h>
#include <objc/encoding.h>
#include <objc/blocks_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "failed at %d: %s\n", __LINE__, #test); return __LINE__; } } while (0)
#define EXPECT_ERROR(statement) do { \
  int caught = 0, before = throws_created(); \
  @autoreleasepool { \
    __objc3_do { statement; CHECK(0); } \
    __objc3_catch(id<Error> error) { CHECK(error != (id)0); caught = 1; } \
  } \
  CHECK(caught && throws_created() == before + 1 && throws_destroyed() == throws_created()); \
} while (0)

static int tag_result(CallStrong value, int key, int fail) __objc3_throws(id<Error>) {
  return hasValue(__objc3_try(throws_forward(value, key, fail)));
}
static int later_argument(void) __objc3_throws(id<Error>) {
  return __objc3_try(throws_scalar(None(id), 36, 1));
}
static CallStrong argument_failure(void) __objc3_throws(id<Error>) {
  return __objc3_try(throws_value(call_make(35), 37, __objc3_try(later_argument())));
}
#ifdef __cplusplus
template <typename Callback>
static CallStrong template_forward(Callback callback, CallStrong value, int fail)
    __objc3_throws(id<Error>) {
  return __objc3_try(callback(value, 39, fail));
}
#endif
static int functions(void) {
  CallStrong source = call_make(0);
  CallStrong result = __objc3_try_force(throws_forward(source, 1, 0));
  source = None(id);
  CHECK(!call_destroyed(0) && hasValue(result));
  result = None(id);
  CHECK(call_destroyed(0) == 1);
  CallStrong target = call_make(3);
  source = call_make(4);
  EXPECT_ERROR(target = __objc3_try(throws_forward(source, 2, 1)));
  CHECK(call_destroyed(2) == 1 && !call_destroyed(3) && !call_destroyed(4));
  CHECK(hasValue(target));
  ThrowsFunction *callback = throws_forward;
  EXPECT_ERROR(__objc3_try(callback(source, 5, 1)));
  CHECK(call_destroyed(5) == 1 && !call_destroyed(4));
  result = __objc3_try_force(callback(source, 5, 0));
  target = None(id); source = None(id); result = None(id);
  CHECK(call_destroyed(3) == 1 && call_destroyed(4) == 1);
  @autoreleasepool {
    Optional(int) optional = __objc3_try_optional(throws_scalar(None(id), 6, 1));
    CHECK(!hasValue(optional) && call_destroyed(6) == 1);
  }
  CHECK(throws_created() == throws_destroyed());
  __objc3_try_force(throws_value(call_make(8), 9, 0));
  CHECK(call_destroyed(8) == 1);
  result = call_identity(__objc3_try_force(throws_value(call_make(9), 10, 0)));
  CHECK(!call_destroyed(9));
  result = None(id);
  CHECK(call_destroyed(9) == 1);
  source = call_make(10);
  result = __objc3_try_force(throws_many(source, source, source, source, source, source, source, 0));
  EXPECT_ERROR(__objc3_try(throws_many(source, source, source, source, source, source, source, 1)));
  source = None(id); result = None(id);
  CHECK(call_destroyed(10) == 1);
  CHECK(hasValue(__objc3_try_force(throws_forward(call_make(37), 38, 0))));
  CHECK(call_destroyed(37) == 1);
  CHECK(__objc3_try_force(tag_result(None(id), 38, 0)) == 0);
  EXPECT_ERROR(__objc3_try(tag_result(None(id), 38, 1)));
  CHECK(call_destroyed(38) == 1);
  EXPECT_ERROR(__objc3_try(argument_failure()));
  CHECK(call_destroyed(35) == 1 && call_destroyed(36) == 1);
  return 0;
}
static int leaves(void) {
  id object = call_object(7);
  CallWeak weak = Some(id __weak, object);
  CallWeak copied = __objc3_try_force(throws_weak(weak, 0));
  EXPECT_ERROR(copied = __objc3_try(throws_weak(weak, 1)));
  object = (id)0;
  CHECK(call_destroyed(7) == 1 && hasValue(copied));
  copied = __objc3_try_force(throws_weak(copied, 0));
  CallUnsafe unsafe = __objc3_try_force(throws_unsafe(Some(id __unsafe_unretained, (id)0)));
  CHECK(hasValue(unsafe));
  for (int arm = 0; arm != 2; ++arm) {
    int key = 11 + arm;
    CallOutcome outcome = arm ? Ok(id, id, call_object(key)) : Err(id, id, call_object(key));
    CallOutcome copy = __objc3_try_force(throws_outcome(outcome, 0));
    EXPECT_ERROR(copy = __objc3_try(throws_outcome(outcome, 1)));
    CHECK(isOk(copy) == arm && !call_destroyed(key));
    outcome = Ok(id, id, (id)0); copy = Err(id, id, (id)0);
    CHECK(call_destroyed(key) == 1);
  }
  object = call_object(13);
  CallMixed mixed = Err(id, id __weak, object);
  CallMixed mixedCopy = __objc3_try_force(throws_mixed(mixed, 0));
  EXPECT_ERROR(mixedCopy = __objc3_try(throws_mixed(mixed, 1)));
  object = (id)0;
  CHECK(call_destroyed(13) == 1 && !isOk(mixedCopy));
  CallBlock block = call_block(call_object(14));
  CallBlock blockCopy = __objc3_try_force(throws_payload_block(block, 0));
  EXPECT_ERROR(blockCopy = __objc3_try(throws_payload_block(block, 1)));
  block = None(CallAction);
  CHECK(!call_destroyed(14));
  blockCopy = None(CallAction);
  CHECK(call_destroyed(14) == 1);
  return 0;
}
static int runtime_calls(void) {
  ThrowsCalls *receiver = [ThrowsSubclass new];
  id<ThrowsProtocol> protocol = receiver;
  CallStrong source = call_make(15);
  CallStrong result = __objc3_try_force([protocol value:source key:16 fail:0]);
  EXPECT_ERROR(result = __objc3_try([protocol value:source key:16 fail:1]));
  CHECK(call_destroyed(16) == 1 && !call_destroyed(15) && hasValue(result));
  result = __objc3_try_force([receiver alternate:source key:17 fail:0]);
  EXPECT_ERROR(result = __objc3_try([receiver alternate:source key:17 fail:1]));
  CHECK(call_destroyed(17) == 1);
  __objc3_try_force([ThrowsCalls make:18 fail:0]);
  CHECK(call_destroyed(18) == 1);
  EXPECT_ERROR(__objc3_try([ThrowsCalls make:19 fail:1]));
  CHECK(call_destroyed(19) == 1 && call_destroyed(20) == 1);
  ThrowsCalls *nilReceiver = (id)0;
  result = __objc3_try_force([nilReceiver value:source key:21 fail:1]);
  CHECK(!hasValue(result) && !call_destroyed(21));
  result = __objc3_try_force([nilReceiver alternate:source key:21 fail:1]);
  CHECK(!hasValue(result));
  __objc3_try_force([nilReceiver value:call_make(22) key:21 fail:1]);
  CHECK(call_destroyed(22) == 1);
  id object = call_object(23);
  CallWeak weak = __objc3_try_force([protocol weak:Some(id __weak, object) fail:0]);
  EXPECT_ERROR(weak = __objc3_try([protocol weak:weak fail:1]));
  object = (id)0;
  CHECK(call_destroyed(23) == 1 && hasValue(weak));
  ThrowsBlock block = throws_make_block(call_object(24));
  const char *signature = block_getType_np((__bridge const void *)block);
  CHECK(signature && !strncmp(signature, @encode(CallStrong), strlen(@encode(CallStrong))));
  const char *tail = signature;
  for (int argument = 0; argument != 5; ++argument) tail = objc_skip_argspec(tail);
  CHECK(tail[0] == '^' && tail[1] == '@');
  result = __objc3_try_force(block(source, 25, 0));
  EXPECT_ERROR(result = __objc3_try(block(source, 25, 1)));
  CHECK(call_destroyed(25) == 1);
  block = (ThrowsBlock)0;
  CHECK(call_destroyed(24) == 1);
  Method method = class_getInstanceMethod(objc_getClass("ThrowsCalls"), @selector(value:key:fail:));
  CHECK(method && method_getNumberOfArguments(method) == 6);
  char *returnType = method_copyReturnType(method), *errorType = method_copyArgumentType(method, 5);
  CHECK(!strcmp(returnType, @encode(CallStrong)) && !strcmp(errorType, "^@"));
  free(returnType); free(errorType);
  source = None(id); result = None(id);
  CHECK(call_destroyed(15) == 1);
  @autoreleasepool {
    @try { __objc3_try_force([receiver native:call_make(26)]); CHECK(0); }
    @catch (id error) { CHECK(error && call_destroyed(26) == 1); }
  }
  CHECK(throws_created() == throws_destroyed());
  return 0;
}
#ifdef __cplusplus
static int cpp_calls(void) {
  ThrowsDerived derived;
  ThrowsBase *base = &derived;
  CallStrong source = call_make(27);
  CallStrong result = __objc3_try_force(base->apply(source, 28, 0));
  EXPECT_ERROR(result = __objc3_try(base->apply(source, 28, 1)));
  CHECK(call_destroyed(28) == 1 && hasValue(result));
  auto member = &ThrowsDerived::apply;
  result = __objc3_try_force((derived.*member)(source, 29, 0));
  EXPECT_ERROR(result = __objc3_try((derived.*member)(source, 29, 1)));
  CHECK(call_destroyed(29) == 1);
  result = __objc3_try_force(ThrowsDerived::make(source, 30, 0));
  EXPECT_ERROR(result = __objc3_try(ThrowsDerived::make(source, 30, 1)));
  CHECK(call_destroyed(30) == 1);
  source = None(id); result = None(id);
  CHECK(call_destroyed(27) == 1);
  // The result is complete before a local destructor throws. The callee
  // still owns it; the caller must retain its previous assignment target.
  result = call_make(31);
  try { result = __objc3_try_force(throws_return_cleanup(32, 0)); CHECK(0); }
  catch (int error) { CHECK(error == 71 && call_destroyed(32) == 1 && !call_destroyed(31)); }
  @autoreleasepool {
    int before = throws_created();
    try { result = __objc3_try_force(throws_return_cleanup(33, 1)); CHECK(0); }
    catch (int error) { CHECK(error == 71 && !call_destroyed(33) && throws_created() == before + 1); }
  }
  CHECK(throws_created() == throws_destroyed());
  id object = call_object(34);
  try { throws_weak_return_cleanup(object); CHECK(0); }
  catch (int error) { CHECK(error == 71); }
  object = (id)0;
  CHECK(call_destroyed(34) == 1);
  result = None(id);
  CHECK(call_destroyed(31) == 1);
  result = __objc3_try_force(template_forward(throws_forward, None(id), 0));
  CHECK(!hasValue(result));
  EXPECT_ERROR(__objc3_try(template_forward(throws_forward, None(id), 1)));
  CHECK(call_destroyed(39) == 1);
  return 0;
}
#endif
int main(int argc, char **argv) {
  if (argc > 1) {
    if (!strcmp(argv[1], "force-function")) __objc3_try_force(throws_value(None(id), 40, 1));
    if (!strcmp(argv[1], "force-message")) {
      ThrowsCalls *receiver = [ThrowsCalls new];
      __objc3_try_force([receiver value:None(id) key:40 fail:1]);
    }
    if (!strcmp(argv[1], "force-block")) {
      ThrowsBlock block = throws_make_block(call_object(41));
      __objc3_try_force(block(None(id), 40, 1));
    }
    return 99;
  }
  int status = functions(); if (status) return status;
  status = leaves(); if (status) return status;
  status = runtime_calls(); if (status) return status;
#ifdef __cplusplus
  status = cpp_calls(); if (status) return status;
#endif
  return throws_created() != throws_destroyed();
}
