#pragma once
// ARC-only ordinary physical return-slot witness, valid in C17/C++20/C++23.
// A prepared +1 Block must be abandoned if later return cleanup fails, even
// when the exception is caught inside the same function and it returns again.
#include "runtime.h"
#if !__has_feature(objc_arc)
#error return_failures requires ARC
#endif
static CaptureObject *capture_exception_token;
static int capture_return_locally_caught;
@protocol Error @end
@interface CaptureReturnError : CaptureObject <Error> @end
@implementation CaptureReturnError @end
static id<Error> capture_checked_token;

static CaptureIntBlock capture_retained_return_failure(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_retained_return_failure(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 111;
  defer { @throw capture_exception_token; }
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_autoreleased_return_failure(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 112;
  defer { @throw capture_exception_token; }
  return ^[move handle] { return handle; };
}
static id capture_erased_return_failure(void)
    __attribute__((ns_returns_retained));
static id capture_erased_return_failure(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 113;
  defer { @throw capture_exception_token; }
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_return_caught_inside(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_caught_inside(void) {
  int before = capture_closed;
  @try {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 114;
    defer { @throw capture_exception_token; }
    return ^[move handle] { return handle; };
  } @catch (CaptureObject *exception) {
    CAPTURE_CHECK(exception == capture_exception_token);
    CAPTURE_CHECK(capture_closed == before + 1);
    return (id)0;
  }
}
static CaptureIntBlock capture_return_nested_catches(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_nested_catches(void) {
  int before = capture_closed;
  @try {
    @try {
      int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 115;
      defer { @throw capture_exception_token; }
      return ^[move handle] { return handle; };
    } @catch (CaptureObject *exception) {
      CAPTURE_CHECK(exception == capture_exception_token &&
                    capture_closed == before + 1);
      @throw;
    }
  } @catch (...) {
    CAPTURE_CHECK(capture_closed == before + 1);
    return (id)0;
  }
}
static CaptureIntBlock capture_return_local_handler(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_local_handler(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 116;
  defer {
    @try { @throw capture_exception_token; }
    @catch (CaptureObject *exception) {
      CAPTURE_CHECK(exception == capture_exception_token);
      ++capture_return_locally_caught;
    }
  }
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_autoreleased_return_success(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 117;
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_checked_return_failure(void) throws(id<Error>)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_checked_return_failure(void) throws(id<Error>) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 124;
  defer { throwError(capture_checked_token); }
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_return_checked_caught_inside(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_checked_caught_inside(void) {
  int before = capture_closed;
  handle {
    int resource __attribute__((objc_resource(close=capture_close, invalid=-1))) = 125;
    defer { throwError(capture_checked_token); }
    return ^[move resource] { return resource; };
  } catch (id<Error> error) {
    CAPTURE_CHECK(error == capture_checked_token && capture_closed == before + 1);
    return (id)0;
  }
}
static CaptureIntBlock capture_return_checked_local_handler(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_checked_local_handler(void) {
  int resource __attribute__((objc_resource(close=capture_close, invalid=-1))) = 126;
  defer {
    handle { throwError(capture_checked_token); }
    catch (id<Error> error) {
      CAPTURE_CHECK(error == capture_checked_token);
      ++capture_return_locally_caught;
    }
  }
  return ^[move resource] { return resource; };
}
static void capture_return_failure_suite(void) {
  capture_exception_token = [CaptureObject make:118];
  capture_checked_token = [CaptureReturnError make:127];
  capture_return_locally_caught = 0;
  int before = capture_closed;
  @autoreleasepool {
    @try {
      (void)capture_retained_return_failure();
      CAPTURE_CHECK(0);
    } @catch (CaptureObject *exception) {
      CAPTURE_CHECK(exception == capture_exception_token &&
                    capture_closed == before + 1);
    }
    @try {
      (void)capture_autoreleased_return_failure();
      CAPTURE_CHECK(0);
    } @catch (...) { CAPTURE_CHECK(capture_closed == before + 2); }
    @try {
      (void)capture_erased_return_failure();
      CAPTURE_CHECK(0);
    } @catch (...) { CAPTURE_CHECK(capture_closed == before + 3); }
    CAPTURE_CHECK(capture_return_caught_inside() == (id)0 &&
                  capture_closed == before + 4);
    CAPTURE_CHECK(capture_return_nested_catches() == (id)0 &&
                  capture_closed == before + 5);
    CaptureIntBlock result = capture_return_local_handler();
    CAPTURE_CHECK(result() == 116 && capture_return_locally_caught == 1 &&
                  capture_closed == before + 5);
    result = (id)0;
    CAPTURE_CHECK(capture_closed == before + 6);
    result = capture_autoreleased_return_success();
    CAPTURE_CHECK(result() == 117 && capture_closed == before + 6);
    result = (id)0;
  }
  CAPTURE_CHECK(capture_closed == before + 7);
  handle {
    (void)tryValue(capture_checked_return_failure());
    CAPTURE_CHECK(0);
  } catch (id<Error> error) {
    CAPTURE_CHECK(error == capture_checked_token && capture_closed == before + 8);
  }
  CAPTURE_CHECK(capture_return_checked_caught_inside() == (id)0 &&
                capture_closed == before + 9);
  CaptureIntBlock result = capture_return_checked_local_handler();
  CAPTURE_CHECK(result() == 126 && capture_return_locally_caught == 2 &&
                capture_closed == before + 9);
  result = (id)0;
  CAPTURE_CHECK(capture_closed == before + 10);
  capture_checked_token = (id)0;
  capture_exception_token = (id)0;
}
