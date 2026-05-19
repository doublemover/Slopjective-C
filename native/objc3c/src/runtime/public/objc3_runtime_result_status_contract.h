#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

inline objc3_runtime_dispatch_status_code RuntimeDispatchResultStatus(
    const objc3_runtime_dispatch_i32_result &result) {
  return result.status_code;
}

inline objc3_runtime_dispatch_status_code RuntimeDispatchResultStatus(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.status_code;
}

}  // namespace objc3c::runtime
