#include "runtime/public/objc3_runtime_result_builder.h"

#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_i32_result BuildRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  const RuntimeDispatchDiagnosticRecord &diagnostic =
      RuntimeDispatchDiagnosticForStatus(status_code);
  objc3_runtime_dispatch_i32_result result{};
  result.status_code = status_code;
  result.value = RuntimeDispatchStatusCarriesValue(status_code) ? value : 0;
  result.diagnostic_code = diagnostic.code;
  result.diagnostic_message = diagnostic.message;
  result.diagnostic_owner_model = RuntimeDispatchDiagnosticOwnerModel();
  result.fail_closed_ownership_model = RuntimeDispatchFailClosedOwnershipModel();
  result.fallback_path_allowed = 0;
  return result;
}

}  // namespace objc3c::runtime
