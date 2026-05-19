#pragma once

#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

struct RuntimeTypedDispatchResult {
  objc3_runtime_dispatch_status_code status_code =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  int value = 0;
};

bool RuntimeTypedDispatchResultIsSuccess(
    const RuntimeTypedDispatchResult &result);
RuntimeTypedDispatchResult NormalizeRuntimeTypedDispatchResult(
    RuntimeTypedDispatchResult result);
RuntimeTypedDispatchResult RuntimeTypedDispatchSuccess(
    RuntimeMethodReturnKind return_kind, int value);
RuntimeTypedDispatchResult RuntimeTypedDispatchFailure(
    objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind);

}  // namespace objc3c::runtime
