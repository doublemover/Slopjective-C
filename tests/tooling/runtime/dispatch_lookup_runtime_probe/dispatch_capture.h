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

inline FromClassDispatchCapture CaptureFromClassDispatchResults() {
  const objc3_runtime_dispatch_typed_result typed_super =
      objc3_runtime_dispatch_typed_from_class_checked(
          kWidgetInstanceReceiver, kRootObjectClassName, kRootValueSelector, 0,
          0, 0, 0);
  const objc3_runtime_dispatch_i32_result i32_super =
      objc3_runtime_dispatch_i32_from_class_checked(
          kWidgetInstanceReceiver, kRootObjectClassName, kRootValueSelector, 0,
          0, 0, 0);
  const objc3_runtime_dispatch_typed_result typed_self =
      objc3_runtime_dispatch_typed_from_class_checked(
          kWidgetInstanceReceiver, kWidgetClassName, kWidgetValueSelector, 0, 0,
          0, 0);
  const objc3_runtime_dispatch_i32_result i32_self =
      objc3_runtime_dispatch_i32_from_class_checked(
          kWidgetInstanceReceiver, kWidgetClassName, kWidgetValueSelector, 0, 0,
          0, 0);
  const objc3_runtime_dispatch_typed_result null_start =
      objc3_runtime_dispatch_typed_from_class_checked(
          kWidgetInstanceReceiver, nullptr, kRootValueSelector, 0, 0, 0, 0);
  const objc3_runtime_dispatch_typed_result empty_start =
      objc3_runtime_dispatch_typed_from_class_checked(
          kWidgetInstanceReceiver, "", kRootValueSelector, 0, 0, 0, 0);
  const objc3_runtime_dispatch_typed_result missing_start =
      objc3_runtime_dispatch_typed_from_class_checked(
          kWidgetInstanceReceiver, kMissingClassName, kRootValueSelector, 0, 0,
          0, 0);
  const objc3_runtime_dispatch_typed_result unreachable_start =
      objc3_runtime_dispatch_typed_from_class_checked(
          kRootObjectInstanceReceiver, kWidgetClassName, kWidgetValueSelector,
          0, 0, 0, 0);

  FromClassDispatchCapture capture;
  capture.typed_super_status = typed_super.status_code;
  capture.typed_super_value = typed_super.i32_value;
  capture.typed_super_return_kind = typed_super.return_kind;
  capture.i32_super_status = i32_super.status_code;
  capture.i32_super_value = i32_super.value;
  capture.i32_super_return_kind = i32_super.return_kind;
  capture.typed_self_status = typed_self.status_code;
  capture.typed_self_value = typed_self.i32_value;
  capture.i32_self_status = i32_self.status_code;
  capture.i32_self_value = i32_self.value;
  capture.null_lookup_start_status = null_start.status_code;
  capture.empty_lookup_start_status = empty_start.status_code;
  capture.missing_lookup_start_status = missing_start.status_code;
  capture.unreachable_lookup_start_status = unreachable_start.status_code;
  return capture;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_DISPATCH_CAPTURE_H_
