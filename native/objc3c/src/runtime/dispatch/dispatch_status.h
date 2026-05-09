#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeStrictDispatchStatus(
    bool resolved, bool ambiguous,
    objc3_runtime_dispatch_status_code unresolved_status);
bool RuntimeDispatchStatusIsSuccess(
    objc3_runtime_dispatch_status_code status_code);

}  // namespace objc3c::runtime
