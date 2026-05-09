#include "runtime/dispatch/typed_dispatch_error.h"

#include "runtime/dispatch/dispatch_status.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeTypedDispatchStrictFailureStatus(
    objc3_runtime_dispatch_status_code status_code) {
  if (RuntimeDispatchStatusIsSuccess(status_code)) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
  }
  return status_code;
}

}  // namespace objc3c::runtime
