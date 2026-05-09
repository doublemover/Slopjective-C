#include "runtime/dispatch/typed_dispatch_result.h"

namespace objc3c::runtime {

RuntimeTypedDispatchResult RuntimeTypedDispatchSuccess(
    RuntimeMethodReturnKind return_kind, int value) {
  RuntimeTypedDispatchResult result;
  result.status_code = OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  result.return_kind = return_kind;
  result.value = value;
  return result;
}

RuntimeTypedDispatchResult RuntimeTypedDispatchFailure(
    objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind) {
  RuntimeTypedDispatchResult result;
  result.status_code = status_code;
  result.return_kind = return_kind;
  result.value = 0;
  return result;
}

}  // namespace objc3c::runtime
