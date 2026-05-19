#include "runtime/public/objc3_runtime_result_materialization_contract.h"

#include "runtime/dispatch/typed_dispatch_contract.h"
#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"
#include "runtime/public/objc3_runtime_result_ownership.h"
#include "runtime/public/objc3_runtime_result_value_contract.h"

namespace objc3c::runtime {
namespace {

bool RuntimeDispatchStatusCarriesTypedValue(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Envelope(
    objc3_runtime_dispatch_status_code status_code, int value,
    objc3_runtime_dispatch_return_kind_code return_kind) {
  const RuntimeDispatchDiagnosticRecord &diagnostic =
      RuntimeDispatchDiagnosticForStatus(status_code);
  objc3_runtime_dispatch_i32_result result{};
  result.abi_version = OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION;
  result.result_size =
      static_cast<uint32_t>(sizeof(objc3_runtime_dispatch_i32_result));
  result.status_code = status_code;
  result.return_kind = return_kind;
  result.value = RuntimeDispatchResultValueOrZero(status_code, value);
  result.diagnostic_code = diagnostic.code;
  result.diagnostic_message = diagnostic.message;
  result.result_contract = RuntimeTypedDispatchContractName(
      RuntimeTypedDispatchContractForStatus(status_code));
  result.diagnostic_owner_model = RuntimeResultDiagnosticOwnerModel();
  result.fail_closed_ownership_model = RuntimeResultFailClosedOwnershipModel();
  result.retired_route_path_allowed = RuntimeResultRetiredRoutePathAllowed();
  return result;
}

}  // namespace

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return MakeRuntimeDispatchI32TypedResult(
      status_code, value, OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32);
}

objc3_runtime_dispatch_typed_result MakeRuntimeDispatchTypedResult(
    objc3_runtime_dispatch_status_code status_code, int value,
    objc3_runtime_dispatch_return_kind_code return_kind) {
  const RuntimeDispatchDiagnosticRecord &diagnostic =
      RuntimeDispatchDiagnosticForStatus(status_code);
  objc3_runtime_dispatch_typed_result result{};
  result.abi_version = OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION;
  result.result_size =
      static_cast<uint32_t>(sizeof(objc3_runtime_dispatch_typed_result));
  result.status_code = status_code;
  result.return_kind = return_kind;
  result.return_kind_name = RuntimeDispatchReturnKindName(return_kind);
  if (RuntimeDispatchStatusCarriesTypedValue(status_code)) {
    switch (return_kind) {
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32:
        result.i32_value = value;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL:
        result.bool_value = value != 0 ? 1 : 0;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_OBJECT_REFERENCE:
        result.object_reference = value;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_CLASS_REFERENCE:
        result.class_reference = value;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_SELECTOR_REFERENCE:
        result.selector_reference = value;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_PROTOCOL_REFERENCE:
        result.protocol_reference = value;
        break;
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID:
      case OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED:
        break;
    }
  }
  result.diagnostic_code = diagnostic.code;
  result.diagnostic_message = diagnostic.message;
  result.result_contract = RuntimeTypedDispatchContractName(
      RuntimeTypedDispatchContractForStatus(status_code));
  result.diagnostic_owner_model = RuntimeResultDiagnosticOwnerModel();
  result.fail_closed_ownership_model = RuntimeResultFailClosedOwnershipModel();
  result.retired_route_path_allowed = RuntimeResultRetiredRoutePathAllowed();
  return result;
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32TypedResult(
    objc3_runtime_dispatch_status_code status_code, int value,
    objc3_runtime_dispatch_return_kind_code return_kind) {
  if (RuntimeDispatchStatusCarriesTypedValue(status_code) &&
      return_kind != OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32) {
    return MakeRuntimeDispatchI32Envelope(
        OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0,
        return_kind);
  }
  return MakeRuntimeDispatchI32Envelope(status_code, value, return_kind);
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32ResultFromTypedResult(
    const objc3_runtime_dispatch_typed_result &typed_result) {
  int value = 0;
  if (typed_result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
      typed_result.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32) {
    value = typed_result.i32_value;
  }
  return MakeRuntimeDispatchI32TypedResult(
      typed_result.status_code, value, typed_result.return_kind);
}

}  // namespace objc3c::runtime
