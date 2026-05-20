#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"
#include "runtime/public/objc3_runtime_dispatch_status.h"

namespace objc3c::runtime {

inline bool RuntimeDispatchResultCarriesValue(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

inline int RuntimeDispatchResultValueOrZero(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return RuntimeDispatchResultCarriesValue(status_code) ? value : 0;
}

inline bool RuntimeDispatchTypedResultCarriesValue(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         result.return_kind != OBJC3_RUNTIME_DISPATCH_RETURN_KIND_VOID &&
         result.return_kind != OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED;
}

}  // namespace objc3c::runtime
