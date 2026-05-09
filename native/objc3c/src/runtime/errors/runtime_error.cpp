#include "runtime/errors/runtime_error.h"

namespace objc3c::runtime {

namespace {

struct RuntimeDispatchDiagnostic {
  objc3_runtime_dispatch_status_code status_code;
  const char *code;
  const char *message;
};

constexpr RuntimeDispatchDiagnostic kRuntimeDispatchDiagnostics[] = {
    {OBJC3_RUNTIME_DISPATCH_STATUS_OK, "", ""},
    {OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER, "O3RT008",
     "runtime dispatch failed: nil receiver has no value dispatch result"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, "O3RT001",
     "runtime dispatch failed: unknown selector"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS, "O3RT002",
     "runtime dispatch failed: unknown receiver class"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH, "O3RT003",
     "runtime dispatch failed: missing class graph"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, "O3RT004",
     "runtime dispatch failed: malformed metadata"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, "O3RT005",
     "runtime dispatch failed: unsupported return type"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT, "O3RT006",
     "runtime dispatch failed: unsupported argument layout"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT, "O3RT007",
     "runtime dispatch failed: category conflict"}};

const RuntimeDispatchDiagnostic &RuntimeDispatchDiagnosticForStatus(
    objc3_runtime_dispatch_status_code status_code) {
  for (const RuntimeDispatchDiagnostic &diagnostic :
       kRuntimeDispatchDiagnostics) {
    if (diagnostic.status_code == status_code) {
      return diagnostic;
    }
  }
  static constexpr RuntimeDispatchDiagnostic kInternalDispatchDiagnostic = {
      OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, "O3RT000",
      "runtime dispatch failed: internal dispatch status"};
  return kInternalDispatchDiagnostic;
}

}  // namespace

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
  const RuntimeDispatchDiagnostic &diagnostic =
      RuntimeDispatchDiagnosticForStatus(status_code);
  objc3_runtime_dispatch_i32_result result{};
  result.status_code = status_code;
  result.value =
      status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK ? value : 0;
  result.diagnostic_code = diagnostic.code;
  result.diagnostic_message = diagnostic.message;
  return result;
}

}  // namespace objc3c::runtime
