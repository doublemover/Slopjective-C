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
    handle { statement; CHECK(0); } \
    catch(id<Error> error) { CHECK(error != (id)0); caught = 1; } \
  } \
  CHECK(caught && throws_created() == before + 1 && throws_destroyed() == throws_created()); \
} while (0)

static int tag_result(CallStrong value, int key, int fail) throws(id<Error>) {
  return hasValue(tryValue(throws_forward(value, key, fail)));
}
static int later_argument(void) throws(id<Error>) {
  return tryValue(throws_scalar(None(id), 36, 1));
}
static CallStrong argument_failure(void) throws(id<Error>) {
  return tryValue(throws_value(call_make(35), 37, tryValue(later_argument())));
}
#ifdef __cplusplus
template <typename Callback>
static CallStrong template_forward(Callback callback, CallStrong value, int fail)
    throws(id<Error>) {
  return tryValue(callback(value, 39, fail));
}
#endif
static int functions(void) {
  CallStrong source = call_make(0);
  CallStrong result = tryForce(throws_forward(source, 1, 0));
  source = None(id);
  CHECK(!call_destroyed(0) && hasValue(result));
  result = None(id);
  CHECK(call_destroyed(0) == 1);
  CallStrong target = call_make(3);
  source = call_make(4);
  EXPECT_ERROR(target = tryValue(throws_forward(source, 2, 1)));
  CHECK(call_destroyed(2) == 1 && !call_destroyed(3) && !call_destroyed(4));
  CHECK(hasValue(target));
  ThrowsFunction *callback = throws_forward;
  EXPECT_ERROR(tryValue(callback(source, 5, 1)));
  CHECK(call_destroyed(5) == 1 && !call_destroyed(4));
  result = tryForce(callback(source, 5, 0));
  target = None(id); source = None(id); result = None(id);
  CHECK(call_destroyed(3) == 1 && call_destroyed(4) == 1);
  @autoreleasepool {
    Optional(int) optional = tryOptional(throws_scalar(None(id), 6, 1));
    CHECK(!hasValue(optional) && call_destroyed(6) == 1);
  }
  CHECK(throws_created() == throws_destroyed());
  tryForce(throws_value(call_make(8), 9, 0));
  CHECK(call_destroyed(8) == 1);
  result = call_identity(tryForce(throws_value(call_make(9), 10, 0)));
  CHECK(!call_destroyed(9));
  result = None(id);
  CHECK(call_destroyed(9) == 1);
  source = call_make(10);
  result = tryForce(throws_many(source, source, source, source, source, source, source, 0));
  EXPECT_ERROR(tryValue(throws_many(source, source, source, source, source, source, source, 1)));
  source = None(id); result = None(id);
  CHECK(call_destroyed(10) == 1);
  CHECK(hasValue(tryForce(throws_forward(call_make(37), 38, 0))));
  CHECK(call_destroyed(37) == 1);
  CHECK(tryForce(tag_result(None(id), 38, 0)) == 0);
  EXPECT_ERROR(tryValue(tag_result(None(id), 38, 1)));
  CHECK(call_destroyed(38) == 1);
  EXPECT_ERROR(tryValue(argument_failure()));
  CHECK(call_destroyed(35) == 1 && call_destroyed(36) == 1);
  return 0;
}
static int effect_widening(void) {
  typedef CallStrong WideMany(int, CallStrong, CallStrong, int, CallStrong,
                             CallStrong, CallStrong, CallStrong, CallStrong, int)
      throws(id<Error>);
  WideMany *wide = call_many;
  CallStrong source = call_make(52);
  CallStrong result = tryForce(wide(1, source, source, 2, source, source,
                                         source, source, source, 3));
  source = None(id);
  CHECK(!call_destroyed(52) && hasValue(result));
  result = None(id);
  CHECK(call_destroyed(52) == 1);

  typedef CallStrong (^WideBlock)(CallStrong) throws(id<Error>);
  CallStrong (^plain)(CallStrong) = ^CallStrong(CallStrong value) {
    return call_identity(value);
  };
  WideBlock block = plain;
  source = call_make(53);
  result = tryForce(block(source));
  source = None(id);
  CHECK(!call_destroyed(53) && hasValue(result));
  result = None(id);
  CHECK(call_destroyed(53) == 1);
#ifdef __cplusplus
  struct LocalWiden {
    CallStrong apply(CallStrong value) { return call_identity(value); }
  } receiver;
  typedef CallStrong (LocalWiden::*WideMember)(CallStrong) throws(id<Error>);
  WideMember member = &LocalWiden::apply;
  source = call_make(54);
  result = tryForce((receiver.*member)(source));
  source = None(id);
  CHECK(!call_destroyed(54) && hasValue(result));
  result = None(id);
  CHECK(call_destroyed(54) == 1);
#endif
  return 0;
}
static int leaves(void) {
  id object = call_object(7);
  CallWeak weak = Some(id __weak, object);
  CallWeak copied = tryForce(throws_weak(weak, 0));
  EXPECT_ERROR(copied = tryValue(throws_weak(weak, 1)));
  object = (id)0;
  CHECK(call_destroyed(7) == 1 && hasValue(copied));
  copied = tryForce(throws_weak(copied, 0));
  CallUnsafe unsafe = tryForce(throws_unsafe(Some(id __unsafe_unretained, (id)0)));
  CHECK(hasValue(unsafe));
  for (int arm = 0; arm != 2; ++arm) {
    int key = 11 + arm;
    CallOutcome outcome = arm ? Ok(id, id, call_object(key)) : Err(id, id, call_object(key));
    CallOutcome copy = tryForce(throws_outcome(outcome, 0));
    EXPECT_ERROR(copy = tryValue(throws_outcome(outcome, 1)));
    CHECK(isOk(copy) == arm && !call_destroyed(key));
    outcome = Ok(id, id, (id)0); copy = Err(id, id, (id)0);
    CHECK(call_destroyed(key) == 1);
  }
  object = call_object(13);
  CallMixed mixed = Err(id, id __weak, object);
  CallMixed mixedCopy = tryForce(throws_mixed(mixed, 0));
  EXPECT_ERROR(mixedCopy = tryValue(throws_mixed(mixed, 1)));
  object = (id)0;
  CHECK(call_destroyed(13) == 1 && !isOk(mixedCopy));
  CallBlock block = call_block(call_object(14));
  CallBlock blockCopy = tryForce(throws_payload_block(block, 0));
  EXPECT_ERROR(blockCopy = tryValue(throws_payload_block(block, 1)));
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
  CallStrong result = tryForce([protocol value:source key:16 fail:0]);
  EXPECT_ERROR(result = tryValue([protocol value:source key:16 fail:1]));
  CHECK(call_destroyed(16) == 1 && !call_destroyed(15) && hasValue(result));
  result = tryForce([receiver alternate:source key:17 fail:0]);
  EXPECT_ERROR(result = tryValue([receiver alternate:source key:17 fail:1]));
  CHECK(call_destroyed(17) == 1);
  tryForce([ThrowsCalls make:18 fail:0]);
  CHECK(call_destroyed(18) == 1);
  EXPECT_ERROR(tryValue([ThrowsCalls make:19 fail:1]));
  CHECK(call_destroyed(19) == 1 && call_destroyed(20) == 1);
  ThrowsCalls *nilReceiver = (id)0;
  result = tryForce([nilReceiver value:source key:21 fail:1]);
  CHECK(!hasValue(result) && !call_destroyed(21));
  result = tryForce([nilReceiver alternate:source key:21 fail:1]);
  CHECK(!hasValue(result));
  tryForce([nilReceiver value:call_make(22) key:21 fail:1]);
  CHECK(call_destroyed(22) == 1);
  id object = call_object(23);
  CallWeak weak = tryForce([protocol weak:Some(id __weak, object) fail:0]);
  EXPECT_ERROR(weak = tryValue([protocol weak:weak fail:1]));
  object = (id)0;
  CHECK(call_destroyed(23) == 1 && hasValue(weak));
  ThrowsBlock block = throws_make_block(call_object(24));
  const char *signature = block_getType_np((__bridge const void *)block);
  CHECK(signature && !strncmp(signature, @encode(CallStrong), strlen(@encode(CallStrong))));
  const char *tail = signature;
  for (int argument = 0; argument != 5; ++argument) tail = objc_skip_argspec(tail);
  CHECK(tail[0] == '^' && tail[1] == '@');
  result = tryForce(block(source, 25, 0));
  EXPECT_ERROR(result = tryValue(block(source, 25, 1)));
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
    @try { tryForce([receiver native:call_make(26)]); CHECK(0); }
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
  CallStrong result = tryForce(base->apply(source, 28, 0));
  EXPECT_ERROR(result = tryValue(base->apply(source, 28, 1)));
  CHECK(call_destroyed(28) == 1 && hasValue(result));
  auto member = &ThrowsDerived::apply;
  result = tryForce((derived.*member)(source, 29, 0));
  EXPECT_ERROR(result = tryValue((derived.*member)(source, 29, 1)));
  CHECK(call_destroyed(29) == 1);
  result = tryForce(ThrowsDerived::make(source, 30, 0));
  EXPECT_ERROR(result = tryValue(ThrowsDerived::make(source, 30, 1)));
  CHECK(call_destroyed(30) == 1);
  source = None(id); result = None(id);
  CHECK(call_destroyed(27) == 1);
  // The result is complete before a local destructor throws. The callee
  // still owns it; the caller must retain its previous assignment target.
  result = call_make(31);
  try { result = tryForce(throws_return_cleanup(32, 0)); CHECK(0); }
  catch (int error) { CHECK(error == 71 && call_destroyed(32) == 1 && !call_destroyed(31)); }
  @autoreleasepool {
    int before = throws_created();
    try { result = tryForce(throws_return_cleanup(33, 1)); CHECK(0); }
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
  result = tryForce(template_forward(throws_forward, None(id), 0));
  CHECK(!hasValue(result));
  EXPECT_ERROR(tryValue(template_forward(throws_forward, None(id), 1)));
  CHECK(call_destroyed(39) == 1);
  return 0;
}
#endif
int main(int argc, char **argv) {
  if (argc > 1) {
    if (!strcmp(argv[1], "force-function")) tryForce(throws_value(None(id), 40, 1));
    if (!strcmp(argv[1], "force-message")) {
      ThrowsCalls *receiver = [ThrowsCalls new];
      tryForce([receiver value:None(id) key:40 fail:1]);
    }
    if (!strcmp(argv[1], "force-block")) {
      ThrowsBlock block = throws_make_block(call_object(41));
      tryForce(block(None(id), 40, 1));
    }
    return 99;
  }
  int status = functions(); if (status) return status;
  status = leaves(); if (status) return status;
  status = effect_widening(); if (status) return status;
  status = runtime_calls(); if (status) return status;
#ifdef __cplusplus
  status = cpp_calls(); if (status) return status;
#endif
  return throws_created() != throws_destroyed();
}
