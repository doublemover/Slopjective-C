#include "runtime/public/objc3_runtime_result_code_contract.h"
#include "runtime/public/objc3_runtime_result_message_contract.h"

#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"

namespace objc3c::runtime {

const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticForStatus(status_code).code;
}

const char *RuntimeDispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticForStatus(status_code).message;
}

}  // namespace objc3c::runtime
