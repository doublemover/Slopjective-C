#include "runtime/public/objc3_runtime_result_materialization_contract.h"

#include "runtime/dispatch/typed_dispatch_contract.h"
#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"
#include "runtime/public/objc3_runtime_result_ownership.h"
#include "runtime/public/objc3_runtime_result_value_contract.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return MakeRuntimeDispatchI32TypedResult(
      status_code, value, OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED);
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32TypedResult(
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

}  // namespace objc3c::runtime
