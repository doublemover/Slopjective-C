#include "runtime/dispatch/dispatch_result_state.h"

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/typed_dispatch_contract.h"
#include "runtime/public/objc3_runtime_result_contract.h"
#include "runtime/state/runtime_state_records.h"

#include <mutex>

namespace objc3c::runtime {

void StoreDispatchResultContractUnlocked(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind) {
  state.last_dispatch_status_code = status_code;
  state.last_dispatch_return_kind = RuntimeMethodReturnKindName(return_kind);
  state.last_dispatch_diagnostic_code =
      RuntimeDispatchDiagnosticCode(status_code);
  state.last_dispatch_diagnostic_message =
      RuntimeDispatchDiagnosticMessage(status_code);
  state.last_dispatch_result_contract =
      RuntimeTypedDispatchContractName(
          RuntimeTypedDispatchContractForStatus(status_code));
}

void RecordTypedDispatchSuccess(RuntimeState &state,
                                RuntimeMethodReturnKind return_kind) {
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.live_dispatch_count;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_resolved_live_method = true;
  StoreDispatchResultContractUnlocked(
      state, OBJC3_RUNTIME_DISPATCH_STATUS_OK, return_kind);
}

void RecordPostResolutionStrictDispatchFailure(
    RuntimeState &state, objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind, const char *dispatch_path) {
  std::lock_guard<std::mutex> lock(state.mutex);
  ++state.strict_dispatch_error_count;
  state.last_dispatch_strict_error = true;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_path = dispatch_path != nullptr ? dispatch_path : "";
  state.last_dispatch_implementation_kind = "strict-dispatch-error";
  StoreDispatchResultContractUnlocked(state, status_code, return_kind);
}

}  // namespace objc3c::runtime
