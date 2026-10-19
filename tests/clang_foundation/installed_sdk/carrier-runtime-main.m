#include "carrier-runtime-call.h"
#include <objc/runtime.h>
#include <objc/encoding.h>
#include <objc/blocks_runtime.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) return __LINE__; } while (0)
static int metadata(void) {
  const char *strong = @encode(CallStrong), *weak = @encode(CallWeak);
  CHECK(strstr(strong, "__objc3_v12_optional_S40=") != 0);
  CHECK(strcmp(strong, weak) && strcmp(strong, @encode(CallUnsafe)));
  CHECK(objc_sizeof_type(strong) == sizeof(CallStrong));
  CHECK(objc_alignof_type(strong) == __alignof__(CallStrong));
  CHECK(objc_sizeof_type(@encode(CallMixed)) == sizeof(CallMixed));
  CHECK(objc_alignof_type(@encode(CallMixed)) == __alignof__(CallMixed));
  CHECK(*objc_skip_typespec(strong) == '\0');
  Method method = class_getInstanceMethod(objc_getClass("RuntimeCalls"), @selector(echo:));
  CHECK(method && method_getNumberOfArguments(method) == 3);
  char *result = method_copyReturnType(method), *parameter = method_copyArgumentType(method, 2);
  CHECK(!strcmp(result, strong) && !strcmp(parameter, strong));
  free(result); free(parameter);
  RuntimeStrongBlock block = runtime_make_block(40);
  const char *signature = block_getType_np((__bridge const void *)block);
  CHECK(signature && !strncmp(signature, strong, strlen(strong)));
  const char *arguments = objc_skip_argspec(signature);
  CHECK(arguments[0] == '@' && arguments[1] == '?');
  arguments = objc_skip_argspec(arguments);
  CHECK(!strncmp(arguments, strong, strlen(strong)));
  block = (RuntimeStrongBlock)0;
  CHECK(call_destroyed(40) == 1);
  return 0;
}
static int messages(void) {
  RuntimeCalls *receiver = [RuntimeSubclass new];
  id<RuntimeCarrierProtocol> protocol = receiver;
  {
    CallStrong source = [receiver make:0];
    CallStrong result = [protocol echo:source];
    result = [receiver direct:result];
    [receiver replace:source key:1];
    CHECK(call_destroyed(1) == 1 && call_destroyed(0) == 0);
    source = None(id);
    CHECK(call_destroyed(0) == 0);
    result = None(id);
    CHECK(call_destroyed(0) == 1);
  }
  [RuntimeCalls classMake:18];
  CHECK(call_destroyed(18) == 1);
  Class nilClass = (Class)0;
  CallStrong missingClass = [nilClass classMake:19];
  CHECK(!hasValue(missingClass) && !call_destroyed(19));
  [receiver make:2];
  CHECK(call_destroyed(2) == 1);
  int present = hasValue([receiver make:3]);
  CHECK(present && call_destroyed(3) == 1);
  {
    RuntimeCalls *nilReceiver = (id)0;
    CallStrong source = call_make(4);
    CallStrong result = [nilReceiver echo:source];
    CHECK(!hasValue(result) && !call_destroyed(4));
    [nilReceiver echo:source];
    result = [nilReceiver direct:source];
    CHECK(!hasValue(result));
    [nilReceiver direct:source];
    CallWeak weak = [nilReceiver echoWeak:None(id __weak)];
    CHECK(!hasValue(weak));
    CallMixed mixed = [nilReceiver echoMixed:Ok(id, id __weak, call_object(5))];
    CHECK(!isOk(mixed) && call_destroyed(5) == 1);
  }
  CHECK(call_destroyed(4) == 1);
  {
    id object = call_object(6);
    CallWeak source = Some(id __weak, object);
    CallWeak result = [protocol echoWeak:source];
    object = (id)0;
    CHECK(call_destroyed(6) == 1 && hasValue(result));
    result = [receiver echoWeak:result];
    CallUnsafe unsafe = [receiver echoUnsafe:Some(id __unsafe_unretained, (id)0)];
    CHECK(hasValue(unsafe));
  }
  {
    CallStrong source = call_make(7);
    CallStrong other = call_make(8);
    CallStrong result = [receiver many:source b:source c:source d:source
                                  e:source f:source g:other last:7];
    source = None(id); other = None(id);
    CHECK(call_destroyed(7) == 1 && !call_destroyed(8));
  }
  CHECK(call_destroyed(8) == 1);
  {
    CallBlock source = call_block(call_object(9));
    CallBlock result = [receiver echoBlock:source];
    source = None(CallAction);
    CHECK(!call_destroyed(9));
  }
  CHECK(call_destroyed(9) == 1);
  for (int arm = 0; arm != 2; ++arm) {
    int key = 10 + arm;
    CallOutcome source = arm ? Ok(id, id, call_object(key))
                             : Err(id, id, call_object(key));
    CallOutcome result = [receiver echoOutcome:source];
    source = Ok(id, id, (id)0);
    CHECK(isOk(result) == (arm != 0) && !call_destroyed(key));
  }
  CHECK(call_destroyed(10) == 1 && call_destroyed(11) == 1);
  return 0;
}
static int blocks(void) {
  RuntimeStrongBlock block = runtime_make_block(12);
  {
    CallStrong source = call_make(13);
    CallStrong result = block(source);
    source = None(id);
    CHECK(!call_destroyed(13));
    result = block(None(id));
    CHECK(call_destroyed(13) == 1 && !call_destroyed(12));
    block = (RuntimeStrongBlock)0;
    CHECK(!call_destroyed(12));
  }
  CHECK(call_destroyed(12) == 1);
  {
    CallWeak (^echo)(CallWeak) = ^CallWeak(CallWeak value) { return value; };
    id object = call_object(14);
    CallWeak result = echo(Some(id __weak, object));
    object = (id)0;
    CHECK(call_destroyed(14) == 1 && hasValue(result));
    CallUnsafe (^unsafe)(CallUnsafe) = ^CallUnsafe(CallUnsafe value) { return value; };
    CallUnsafe borrowed = unsafe(Some(id __unsafe_unretained, (id)0));
    CHECK(hasValue(borrowed));
  }
  {
    CallBlock (^echo)(CallBlock) = ^CallBlock(CallBlock value) { return value; };
    CallBlock result = echo(call_block(call_object(15)));
    CHECK(hasValue(result) && !call_destroyed(15));
  }
  CHECK(call_destroyed(15) == 1);
  {
    CallMixed (^echo)(CallMixed) = ^CallMixed(CallMixed value) { return value; };
    CallMixed result = echo(Ok(id, id __weak, call_object(16)));
    CHECK(isOk(result) && !call_destroyed(16));
  }
  CHECK(call_destroyed(16) == 1);
  return 0;
}
static int exceptions(void) {
  RuntimeCalls *receiver = [RuntimeCalls new];
  CallStrong source = call_make(17);
  CallStrong result = source;
  @autoreleasepool {
    @try { result = [receiver throwNative:source]; }
    @catch (id error) { CHECK(error != (id)0 && call_destroyed(30) == 1); }
  }
  CHECK(call_destroyed(31) == 1 && !call_destroyed(17));
#ifdef __cplusplus
  try { result = [receiver throwCpp:source]; }
  catch (int error) { CHECK(error == 42); }
  CHECK(call_destroyed(32) == 1 && !call_destroyed(17));
  CallStrong (^throwing)(CallStrong) = ^CallStrong(CallStrong value) {
    value = call_make(33);
    throw 43;
  };
  try { result = throwing(source); }
  catch (int error) { CHECK(error == 43); }
  CHECK(call_destroyed(33) == 1 && !call_destroyed(17));
#endif
  return 0;
}
int main(void) {
  int result;
  if ((result = metadata())) return result;
  if ((result = messages())) return result;
  if ((result = blocks())) return result;
  if ((result = exceptions())) return result;
  return call_destroyed(17) == 1 ? 0 : 200;
}
