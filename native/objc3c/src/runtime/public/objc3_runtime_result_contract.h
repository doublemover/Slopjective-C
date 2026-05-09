#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

/*
 * Internal result materialization helpers for the runtime public ABI. These are
 * not exported C entrypoints; objc3_runtime_result.h remains the C layout owner.
 */
const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code);
const char *RuntimeDispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code);
objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value);

}  // namespace objc3c::runtime
