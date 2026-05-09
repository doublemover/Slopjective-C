#pragma once

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

struct RuntimeDispatchDiagnosticRecord {
  objc3_runtime_dispatch_status_code status_code;
  const char *code;
  const char *message;
};

const RuntimeDispatchDiagnosticRecord &RuntimeDispatchDiagnosticForStatus(
    objc3_runtime_dispatch_status_code status_code);
bool RuntimeDispatchStatusCarriesValue(
    objc3_runtime_dispatch_status_code status_code);
const char *RuntimeDispatchDiagnosticOwnerModel();
const char *RuntimeDispatchFailClosedOwnershipModel();

}  // namespace objc3c::runtime
