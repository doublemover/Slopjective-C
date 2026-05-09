#include "runtime/dispatch/typed_dispatch_result.h"

#include "runtime/dispatch/dispatch_status.h"
#include "runtime/dispatch/typed_dispatch_error.h"

namespace objc3c::runtime {

bool RuntimeTypedDispatchResultIsSuccess(
    const RuntimeTypedDispatchResult &result) {
  return RuntimeDispatchStatusIsSuccess(result.status_code);
}

RuntimeTypedDispatchResult NormalizeRuntimeTypedDispatchResult(
    RuntimeTypedDispatchResult result) {
  if (!RuntimeTypedDispatchResultIsSuccess(result)) {
    result.status_code =
        RuntimeTypedDispatchStrictFailureStatus(result.status_code);
    result.value = 0;
    return result;
  }
  if (result.return_kind == RuntimeMethodReturnKind::Unsupported) {
    return RuntimeTypedDispatchFailure(
        OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
        result.return_kind);
  }
  return result;
}

RuntimeTypedDispatchResult RuntimeTypedDispatchSuccess(
    RuntimeMethodReturnKind return_kind, int value) {
  RuntimeTypedDispatchResult result;
  result.status_code = OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  result.return_kind = return_kind;
  result.value = value;
  return NormalizeRuntimeTypedDispatchResult(result);
}

RuntimeTypedDispatchResult RuntimeTypedDispatchFailure(
    objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind) {
  RuntimeTypedDispatchResult result;
  result.status_code = RuntimeTypedDispatchStrictFailureStatus(status_code);
  result.return_kind = return_kind;
  result.value = 0;
  return result;
}

}  // namespace objc3c::runtime
