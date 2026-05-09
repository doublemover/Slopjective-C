#include "runtime/public/objc3_runtime_result_contract.h"

#include "runtime/public/objc3_runtime_dispatch_diagnostics.h"
#include "runtime/public/objc3_runtime_result_builder.h"

namespace objc3c::runtime {

const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticForStatus(status_code).code;
}

const char *RuntimeDispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticForStatus(status_code).message;
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return BuildRuntimeDispatchI32Result(status_code, value);
}

}  // namespace objc3c::runtime
