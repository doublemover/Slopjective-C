#pragma once

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

}  // namespace objc3c::runtime
