#include "provider.h"
#include <assert.h>
#include <objc/runtime.h>
#include <objc/objc-arc.h>

static int errors_created, errors_destroyed;
#if __has_feature(objc_arc)
static id<Error> __weak last_error;
#endif
__attribute__((objc_root_class))
@interface TestError <Error> { Class isa; }
+ (instancetype)new;
@end
@implementation TestError
+ (instancetype)new { ++errors_created; return class_createInstance(self, 0); }
- (void)dealloc {
  ++errors_destroyed;
#if !__has_feature(objc_arc)
  object_dispose(self);
#endif
}
@end

static Optional(int) input(int *calls, int present) {
  ++*calls;
  return present ? Some(int, 42) : None(int);
}

int main(void) {
  assert(OBJC3_LANGUAGE_REVISION >= 202610);
  assert(OBJC3_STDLIB_VERSION_MAJOR == 2);
  assert(objc3_has_feature(OBJC3_FEATURE_OPTIONALS));
  assert(objc3_has_feature(OBJC3_FEATURE_RESULT));
  assert(objc3_has_feature(OBJC3_FEATURE_THROWS));
  assert(!objc3_has_feature((objc3_language_feature)999));
  assert(stdlib_records() == 42 && stdlib_tasks() == 1);
  __block int factory_calls = 0;
  int input_calls = 0;
  int (^factory)(void) = ^int { ++factory_calls; return 71; };
  Result(int, int) good = okOr(input(&input_calls, 1), factory);
  assert(input_calls == 1 && factory_calls == 0 && isOk(good));
  Result(int, int) bad = okOr(input(&input_calls, 0), factory);
  assert(input_calls == 2 && factory_calls == 1 && !isOk(bad));
  match(good) { case .Ok(let value): { assert(value == 42); } case .Err(let error): { (void)error; assert(0); } }
  match(bad) { case .Ok(let value): { (void)value; assert(0); } case .Err(let error): { assert(error == 71); } }
  Result(int, int) zero = okOr(Some(int, 0), factory);
  assert(isOk(zero) && factory_calls == 1);
  match(zero) {
    case .Ok(let value): { assert(value == 0); }
    case .Err(let error): { (void)error; assert(0); }
  }
  assert(isOk(stdlib_optional(Some(int, 42), factory)));
  assert(!isOk(stdlib_optional(None(int), factory)) && factory_calls == 2);

  __block int status_calls = 0, success_calls = 0, error_calls = 0;
  __block int status_factory_constructions = 0;
  int (^status_error)(int) = ^int(int code) { ++error_calls; return code + 100; };
  Result(int, int) status_good = resultFromStatus(
      (++status_calls, 0), (++success_calls, 42),
      (++status_factory_constructions, status_error));
  assert(isOk(status_good) && status_calls == 1 && success_calls == 1);
  assert(error_calls == 0 && status_factory_constructions == 0);
  Result(int, int) status_bad = resultFromStatus(
      (++status_calls, -7), (++success_calls, 99),
      (++status_factory_constructions, status_error));
  assert(!isOk(status_bad) && status_calls == 2 && success_calls == 1);
  assert(error_calls == 1 && status_factory_constructions == 1);
  match(status_bad) {
    case .Ok(let value): { (void)value; assert(0); }
    case .Err(let error): { assert(error == 93); }
  }
  assert(isOk(stdlib_status(0, 42, status_error)));
  assert(!isOk(stdlib_status(9, 0, status_error)) && error_calls == 2);

  __block int factory_constructions = 0;
  Result(int, int) lazy = okOr(Some(int, 1), (++factory_constructions, factory));
  assert(isOk(lazy) && factory_constructions == 0);
  lazy = okOr(None(int), (++factory_constructions, factory));
  assert(!isOk(lazy) && factory_constructions == 1 && factory_calls == 3);
  @autoreleasepool {
    id<Error> (^error_factory)(void) = ^id<Error> {
      ++factory_calls;
      #if __has_feature(objc_arc)
      id<Error> error = [TestError new];
      last_error = error;
      return error;
#else
      return objc_autorelease([TestError new]);
#endif
    };
    assert(tryForce(orThrow(input(&input_calls, 1), error_factory)) == 42);
    assert(input_calls == 3 && factory_calls == 3 && errors_created == 0);
    assert(tryForce(stdlib_throwing(Some(int, 17), error_factory)) == 17);
    int caught = 0;
    handle {
      (void)tryValue(stdlib_throwing(input(&input_calls, 0), error_factory));
      assert(0);
    } catch(id<Error> error) {
      assert(error != (id)0 && errors_created == 1 && errors_destroyed == 0);
#if __has_feature(objc_arc)
      assert(error == last_error);
#endif
      caught = 1;
    }
    assert(caught && input_calls == 4 && factory_calls == 4);
  }
#if __has_feature(objc_arc)
  assert(errors_created == errors_destroyed && last_error == (id)0);
  id __weak observed = (id)0;
  @autoreleasepool {
    id object = [TestError new];
    observed = object;
    Optional(id) value = Some(id, object);
    id returned = tryForce(orThrow(value, ^id<Error> { __builtin_trap(); }));
    Result(id, id<Error>) result = okOr(value, ^id<Error> { __builtin_trap(); });
    object = (id)0;
    value = None(id);
    assert(observed == returned && isOk(result));
    returned = (id)0;
    match(result) {
      case .Ok(let payload): { assert(payload == observed); }
      case .Err(let error): { (void)error; assert(0); }
    }
  }
  assert(observed == (id)0 && errors_created == errors_destroyed);
#endif
  @autoreleasepool {
    __block int bridged_status = 0;
    __block int bridge_factory_calls = 0;
    id<Error> (^bridge_error)(int) = ^id<Error>(int code) {
      bridged_status = code;
      ++bridge_factory_calls;
#if __has_feature(objc_arc)
      return [TestError new];
#else
      return objc_autorelease([TestError new]);
#endif
    };
    tryForce(stdlib_status_throwing(0, bridge_error));
    assert(bridge_factory_calls == 0);
    int caught_status = 0;
    handle {
      tryValue(stdlib_status_throwing(-17, bridge_error));
      assert(0);
    } catch(id<Error> error) {
      assert(error != (id)0 && bridged_status == -17 && bridge_factory_calls == 1);
      caught_status = 1;
    }
    assert(caught_status);
  }
#if __has_feature(objc_arc)
  assert(errors_created == errors_destroyed);
#endif
  return 0;
}
