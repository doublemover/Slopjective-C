#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"

namespace objc3c::runtime {

struct RuntimeDispatchDiagnosticRecord {
  objc3_runtime_dispatch_status_code status_code;
  const char *code;
  const char *message;
};

const RuntimeDispatchDiagnosticRecord &RuntimeDispatchDiagnosticForStatus(
    objc3_runtime_dispatch_status_code status_code);
const char *RuntimeDispatchReturnKindName(
    objc3_runtime_dispatch_return_kind_code return_kind);

}  // namespace objc3c::runtime
