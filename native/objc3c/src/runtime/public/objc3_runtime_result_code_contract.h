#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

namespace objc3c::runtime {

const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code);

}  // namespace objc3c::runtime
