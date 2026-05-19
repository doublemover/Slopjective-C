#include "runtime/dispatch/dispatch_api.h"

#include "runtime/dispatch/dispatch_abort_diagnostics.h"
#include "runtime/dispatch/dispatch_checked_entrypoint.h"
#include "runtime/dispatch/dispatch_status.h"
#include "runtime/public/objc3_runtime_api.h"

extern "C" objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  return objc3c::runtime::ExecuteRuntimeDispatchI32Checked(
      receiver, selector, a0, a1, a2, a3);
}

extern "C" objc3_runtime_dispatch_i32_result
objc3_runtime_dispatch_i32_from_class_checked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  return objc3c::runtime::ExecuteRuntimeDispatchI32FromClassChecked(
      receiver, lookup_start_class_name, selector, a0, a1, a2, a3);
}

extern "C" objc3_runtime_dispatch_typed_result
objc3_runtime_dispatch_typed_checked(int receiver, const char *selector, int a0,
                                     int a1, int a2, int a3) {
  return objc3c::runtime::ExecuteRuntimeDispatchTypedChecked(
      receiver, selector, a0, a1, a2, a3);
}

extern "C" objc3_runtime_dispatch_typed_result
objc3_runtime_dispatch_typed_from_class_checked(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  return objc3c::runtime::ExecuteRuntimeDispatchTypedFromClassChecked(
      receiver, lookup_start_class_name, selector, a0, a1, a2, a3);
}

extern "C" int objc3_runtime_dispatch_i32(int receiver, const char *selector,
                                          int a0, int a1, int a2, int a3) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(receiver, selector, a0, a1, a2, a3);
  if (!objc3c::runtime::RuntimeDispatchStatusCarriesValueResult(
          result.status_code)) {
    objc3c::runtime::AbortRuntimeDispatchFailure(result);
  }
  return result.value;
}

extern "C" int objc3_runtime_dispatch_i32_from_class(
    int receiver, const char *lookup_start_class_name, const char *selector,
    int a0, int a1, int a2, int a3) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_from_class_checked(
          receiver, lookup_start_class_name, selector, a0, a1, a2, a3);
  if (!objc3c::runtime::RuntimeDispatchStatusCarriesValueResult(
          result.status_code)) {
    objc3c::runtime::AbortRuntimeDispatchFailure(result);
  }
  return result.value;
}
