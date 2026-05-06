#include "runtime/dispatch/dispatch_errors.h"

#include "runtime/errors/runtime_error.h"

namespace objc3c::runtime {

const char *DispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticCode(status_code);
}

const char *DispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code) {
  return RuntimeDispatchDiagnosticMessage(status_code);
}

objc3_runtime_dispatch_i32_result MakeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  return MakeRuntimeDispatchI32Result(status_code, value);
}

}  // namespace objc3c::runtime
