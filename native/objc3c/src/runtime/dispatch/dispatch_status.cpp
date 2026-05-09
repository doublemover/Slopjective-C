#include "runtime/dispatch/dispatch_status.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeStrictDispatchStatus(
    bool resolved, bool ambiguous,
    objc3_runtime_dispatch_status_code unresolved_status) {
  if (resolved) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  }
  if (ambiguous) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
  }
  return unresolved_status;
}

bool RuntimeDispatchStatusIsSuccess(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

}  // namespace objc3c::runtime
