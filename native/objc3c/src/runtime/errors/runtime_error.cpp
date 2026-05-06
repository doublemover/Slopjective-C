#include "runtime/errors/runtime_error.h"

namespace objc3c::runtime {

const char *RuntimeDispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code) {
  switch (status_code) {
    case OBJC3_RUNTIME_DISPATCH_STATUS_OK:
    case OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER:
      return "";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR:
      return "O3RT001";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS:
      return "O3RT002";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH:
      return "O3RT003";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA:
      return "O3RT004";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE:
      return "O3RT005";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT:
      return "O3RT006";
    case OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT:
      return "O3RT007";
  }
  return "O3RT000";
}

const char *RuntimeDispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code) {
  switch (status_code) {
    case OBJC3_RUNTIME_DISPATCH_STATUS_OK:
    case OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER:
      return "";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR:
      return "runtime dispatch failed: unknown selector";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS:
      return "runtime dispatch failed: unknown receiver class";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH:
      return "runtime dispatch failed: missing class graph";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA:
      return "runtime dispatch failed: malformed metadata";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE:
      return "runtime dispatch failed: unsupported return type";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT:
      return "runtime dispatch failed: unsupported argument layout";
    case OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT:
      return "runtime dispatch failed: category conflict";
  }
  return "runtime dispatch failed: internal dispatch status";
}

objc3_runtime_dispatch_i32_result MakeRuntimeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  objc3_runtime_dispatch_i32_result result{};
  result.status_code = status_code;
  result.value = value;
  result.diagnostic_code = RuntimeDispatchDiagnosticCode(status_code);
  result.diagnostic_message = RuntimeDispatchDiagnosticMessage(status_code);
  return result;
}

}  // namespace objc3c::runtime
