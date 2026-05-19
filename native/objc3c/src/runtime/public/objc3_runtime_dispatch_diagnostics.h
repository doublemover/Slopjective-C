#pragma once

#include "runtime/public/objc3_runtime_dispatch_status.h"

namespace objc3c::runtime {

struct RuntimeDispatchDiagnosticRecord {
  objc3_runtime_dispatch_status_code status_code;
  const char *code;
  const char *message;
};

const RuntimeDispatchDiagnosticRecord &RuntimeDispatchDiagnosticForStatus(
    objc3_runtime_dispatch_status_code status_code);

}  // namespace objc3c::runtime
