#include "runtime/public/objc3_runtime_result_materialization_contract.h"

#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"
#include "runtime/public/objc3_runtime_result_ownership.h"
#include "runtime/public/objc3_runtime_result_value_contract.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  const RuntimeDispatchDiagnosticRecord &diagnostic =
      RuntimeDispatchDiagnosticForStatus(status_code);
  objc3_runtime_dispatch_i32_result result{};
  result.status_code = status_code;
  result.value = RuntimeDispatchResultValueOrZero(status_code, value);
  result.diagnostic_code = diagnostic.code;
  result.diagnostic_message = diagnostic.message;
  result.diagnostic_owner_model = RuntimeResultDiagnosticOwnerModel();
  result.fail_closed_ownership_model = RuntimeResultFailClosedOwnershipModel();
  result.retired_route_path_allowed = RuntimeResultRetiredRoutePathAllowed();
  return result;
}

}  // namespace objc3c::runtime
