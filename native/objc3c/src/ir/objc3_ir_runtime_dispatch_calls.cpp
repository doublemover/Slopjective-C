#include "ir/objc3_ir_runtime_dispatch_calls.h"

#include <sstream>

bool Objc3IRRuntimeDispatchCallRequestOwnsResult(
    const Objc3IRRuntimeDispatchCallRequest &request) {
  return Objc3LoweringStrictOwnerModelIsReady(
      request.result_owner, request.result_owner_model,
      request.strict_no_fallback, request.strict_no_compatibility);
}

bool Objc3IRDirectDispatchCallRequestOwnsResult(
    const Objc3IRDirectDispatchCallRequest &request) {
  return Objc3LoweringStrictOwnerModelIsReady(
      request.result_owner, request.result_owner_model,
      request.strict_no_fallback, request.strict_no_compatibility);
}

std::string BuildObjc3IRRuntimeDispatchCall(
    const Objc3IRRuntimeDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  " << request.result_value << " = call i32 @"
       << request.dispatch_symbol << "(i32 " << request.receiver << ", ptr "
       << request.selector_ptr;
  for (const std::string &arg : request.args) {
    call << ", i32 " << arg;
  }
  call << ")";
  return call.str();
}

std::string BuildObjc3IRDirectDispatchCall(
    const Objc3IRDirectDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  " << request.result_value << " = call i32 "
       << request.callee_symbol << "(";
  for (std::size_t i = 0;
       i < request.explicit_arg_count && i < request.args.size(); ++i) {
    if (i != 0) {
      call << ", ";
    }
    call << "i32 " << request.args[i];
  }
  call << ")";
  return call.str();
}
