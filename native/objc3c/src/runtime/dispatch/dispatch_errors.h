#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

const char *DispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code);
const char *DispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code);
objc3_runtime_dispatch_i32_result MakeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value);

}  // namespace objc3c::runtime
