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

#if __has_feature(objc_arc)
static int objc_error_out(id<Error> __autoreleasing *error, int success) {
  if (success) return 1;
  *error = [TestError new];
  return 0;
}
#endif

static Optional(int) input(int *calls, int present) {
  ++*calls;
  return present ? Some(int, 42) : None(int);
}

static int status_output(int *output, int value) { *output = value; return 0; }

static int error_out_api(int *value, int *error, int success) {
  *value = success ? 42 : -1; *error = success ? 0 : 17; return success;
}

#ifdef __cplusplus
static int lazy_factory_copies, lazy_factory_calls;
struct LazyErrorFactory {
  LazyErrorFactory() = default;
  LazyErrorFactory(const LazyErrorFactory &) { ++lazy_factory_copies; }
  int operator()() const { ++lazy_factory_calls; return 71; }
};
static int error_out_copies;
struct ErrorOutProbe {
  int value = 17;
  ErrorOutProbe() = default;
  ErrorOutProbe(const ErrorOutProbe &other) : value(other.value) { ++error_out_copies; }
};
#endif

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
  int status_out = 0;
  Result(int, int) output_result = resultFromStatus(status_output(&status_out, 42),
                                                  status_out, status_error);
  match(output_result) {
    case .Ok(let value): { assert(value == 42 && status_out == 42); }
    case .Err(let error): { (void)error; assert(0); }
  }
  assert(isOk(stdlib_status(0, 42, status_error)));
  assert(!isOk(stdlib_status(9, 0, status_error)) && error_calls == 2);

#ifdef __cplusplus
  LazyErrorFactory lazy_factory;
  Result(int, int) uncaptured_factory = okOr(Some(int, 42), lazy_factory);
  assert(isOk(uncaptured_factory) && lazy_factory_copies == 0 && lazy_factory_calls == 0);
  uncaptured_factory = okOr(None(int), lazy_factory);
  assert(!isOk(uncaptured_factory) && lazy_factory_copies == 0 && lazy_factory_calls == 1);
  ErrorOutProbe probe;
  Result(int, int) no_error_read = resultFromErrorOut(1, 42, probe,
      ^int(const ErrorOutProbe &raw) { return raw.value; });
  assert(isOk(no_error_read) && error_out_copies == 0);
#endif
  int raw_error = 0, raw_value = 0;
  int before_error_calls = error_calls;
  Result(int, int) out_good = resultFromErrorOut(
      error_out_api(&raw_value, &raw_error, 1), raw_value, raw_error, status_error);
  match(out_good) {
    case .Ok(let value): { assert(value == 42 && error_calls == before_error_calls); }
    case .Err(let error): { (void)error; assert(0); }
  }
  Result(int, int) out_bad = resultFromErrorOut(
      error_out_api(&raw_value, &raw_error, 0), raw_value, raw_error, status_error);
  match(out_bad) {
    case .Ok(let value): { (void)value; assert(0); }
    case .Err(let error): { assert(error == 117 && error_calls == before_error_calls + 1); }
  }
  assert(isOk(stdlib_error_out(1, 17, status_error)));
  assert(!isOk(stdlib_error_out(0, 17, status_error)));
  // Failure with a missing error still invokes the explicit domain adapter.
  assert(!isOk(resultFromErrorOut(0, 42, 0, status_error)));

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
#if __has_feature(objc_arc)
  // Managed native carrier ownership is qualified under ARC. MRC consumers
  // above exercise scalar bridges and the explicit throwing error boundary.
  @autoreleasepool {
    __block int object_status_calls = 0, object_factory_calls = 0;
    id<Error> (^status_object_error)(int) = ^id<Error>(int code) {
      assert(code == -17);
      ++object_factory_calls;
      return [TestError new];
    };
    Result(int, id<Error>) status_good_object = resultFromStatus(
        (++object_status_calls, 0), 42, status_object_error);
    assert(isOk(status_good_object) && object_status_calls == 1 && object_factory_calls == 0);
    Result(int, id<Error>) status_bad_object = resultFromStatus(
        (++object_status_calls, -17), 42, status_object_error);
    assert(!isOk(status_bad_object) && object_status_calls == 2 && object_factory_calls == 1);
    match(status_bad_object) {
      case .Ok(let value): { (void)value; assert(0); }
      case .Err(let error): { assert(error != (id)0); }
    }
    id<Error> __autoreleasing raw_error_object = (id)0;
    id<Error> (^normalize_error)(id<Error>) = ^id<Error>(id<Error> raw) { return raw; };
    Result(int, id<Error>) object_good = resultFromErrorOut(
        objc_error_out(&raw_error_object, 1), 42, raw_error_object, normalize_error);
    assert(isOk(object_good) && raw_error_object == (id)0);
    Result(int, id<Error>) object_bad = resultFromErrorOut(
        objc_error_out(&raw_error_object, 0), 42, raw_error_object, normalize_error);
    match(object_bad) {
      case .Ok(let value): { (void)value; assert(0); }
      case .Err(let error): { assert(error == raw_error_object && error != (id)0); }
    }
  }
  assert(errors_created == errors_destroyed);
#endif
  return 0;
}
