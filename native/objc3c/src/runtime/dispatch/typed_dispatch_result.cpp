#include "runtime/dispatch/typed_dispatch_result.h"

namespace objc3c::runtime {

namespace {

bool RuntimeDispatchStatusIsOk(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

objc3_runtime_dispatch_status_code RuntimeStrictFailureStatus(
    objc3_runtime_dispatch_status_code status_code) {
  if (RuntimeDispatchStatusIsOk(status_code)) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
  }
  return status_code;
}

}  // namespace

RuntimeTypedDispatchResultContract RuntimeTypedDispatchContractForStatus(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchStatusIsOk(status_code)
             ? RuntimeTypedDispatchResultContract::ValueResult
             : RuntimeTypedDispatchResultContract::StrictErrorResult;
}

const char *RuntimeTypedDispatchContractName(
    RuntimeTypedDispatchResultContract contract) {
  switch (contract) {
    case RuntimeTypedDispatchResultContract::ValueResult:
      return "typed-dispatch-value-result";
    case RuntimeTypedDispatchResultContract::StrictErrorResult:
      return "typed-dispatch-strict-error-result";
  }
  return "typed-dispatch-strict-error-result";
}

bool RuntimeTypedDispatchResultIsSuccess(
    const RuntimeTypedDispatchResult &result) {
  return RuntimeDispatchStatusIsOk(result.status_code);
}

RuntimeTypedDispatchResult NormalizeRuntimeTypedDispatchResult(
    RuntimeTypedDispatchResult result) {
  if (!RuntimeTypedDispatchResultIsSuccess(result)) {
    result.status_code = RuntimeStrictFailureStatus(result.status_code);
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
  result.status_code = RuntimeStrictFailureStatus(status_code);
  result.return_kind = return_kind;
  result.value = 0;
  return result;
}

}  // namespace objc3c::runtime
