#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_CAPTURE_H_

#include "probe_state.h"
#include "runtime_assertion_helpers.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline DispatchCapture CaptureDispatchResults() {
  const objc3_runtime_dispatch_i32_result dispatch =
      objc3_runtime_dispatch_i32_checked(
          kDispatchReceiver, kCopySelector, kDispatchArg0, kDispatchArg1,
          kDispatchArg2, kDispatchArg3);

  DispatchCapture capture;
  capture.dispatch_result = dispatch.value;
  capture.expected_dispatch_result = ExpectedCopyDispatchResult();
  capture.nil_dispatch_result = objc3_runtime_dispatch_i32(
      kNilDispatchReceiver, kCopySelector, kDispatchArg0, kDispatchArg1,
      kDispatchArg2, kDispatchArg3);
  return capture;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_CAPTURE_H_
