#include "runtime/dispatch/dispatch_api.h"

#include "runtime/dispatch/dispatch_abort_diagnostics.h"
#include "runtime/dispatch/dispatch_checked_entrypoint.h"
#include "runtime/dispatch/dispatch_status.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"

namespace {

int ProjectRuntimeTypedDispatchValueOrAbort(
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    const objc3_runtime_dispatch_typed_result &result) {
  if (!objc3c::runtime::RuntimeDispatchStatusCarriesValueResult(
          result.status_code)) {
    objc3c::runtime::AbortRuntimeDispatchFailure(
        objc3c::runtime::MakeRuntimeDispatchI32TypedResult(
            result.status_code, 0, result.return_kind));
  }
  if (result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER) {
    return 0;
  }
  if (result.return_kind != expected_return_kind) {
    objc3c::runtime::AbortRuntimeDispatchFailure(
        objc3c::runtime::MakeRuntimeDispatchI32TypedResult(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0,
            result.return_kind));
  }
  switch (expected_return_kind) {
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32:
      return result.i32_value;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL:
      return result.bool_value;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID:
      return 0;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE:
      return result.object_reference;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE:
      return result.class_reference;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE:
      return result.selector_reference;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE:
      return result.protocol_reference;
    case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED:
      break;
  }
  objc3c::runtime::AbortRuntimeDispatchFailure(
      objc3c::runtime::MakeRuntimeDispatchI32TypedResult(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0,
          expected_return_kind));
  return 0;
}

}  // namespace

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

extern "C" int objc3_runtime_dispatch_typed_value(
    objc3_runtime_dispatch_return_kind_code expected_return_kind,
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  return ProjectRuntimeTypedDispatchValueOrAbort(
      expected_return_kind,
      objc3_runtime_dispatch_typed_checked(receiver, selector, a0, a1, a2,
                                           a3));
}

extern "C" int objc3_runtime_dispatch_typed_value_from_class(
    objc3_runtime_dispatch_return_kind_code expected_return_kind, int receiver,
    const char *lookup_start_class_name, const char *selector, int a0, int a1,
    int a2, int a3) {
  return ProjectRuntimeTypedDispatchValueOrAbort(
      expected_return_kind,
      objc3_runtime_dispatch_typed_from_class_checked(
          receiver, lookup_start_class_name, selector, a0, a1, a2, a3));
}

extern "C" objc3_runtime_dispatch_i32_result
objc3_runtime_cache_aware_dispatch_i32_checked(
    int receiver,
    const objc3_runtime_cache_aware_dispatch_descriptor *descriptor,
    int a0,
    int a1,
    int a2,
    int a3) {
  return objc3c::runtime::ExecuteRuntimeCacheAwareDispatchI32Checked(
      receiver, descriptor, a0, a1, a2, a3);
}

extern "C" int objc3_runtime_prepare_cache_aware_dispatch_descriptor(
    objc3_runtime_cache_aware_dispatch_descriptor *descriptor,
    const char *selector,
    const char *source_path,
    uint32_t source_line,
    uint32_t source_column) {
  return objc3c::runtime::PrepareRuntimeCacheAwareDispatchDescriptor(
      descriptor, selector, source_path, source_line, source_column);
}

extern "C" void objc3_runtime_abort_dispatch_status_i32(int status_code) {
  objc3c::runtime::AbortRuntimeDispatchFailure(
      objc3c::runtime::MakeRuntimeDispatchI32TypedResult(
          static_cast<objc3_runtime_dispatch_status_code>(status_code), 0,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32));
}
