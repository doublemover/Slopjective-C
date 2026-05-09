#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code);
const char *RuntimeDispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code);
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value);

}  // namespace objc3c::runtime
