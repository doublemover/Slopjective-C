#include "ir/objc3_ir_runtime_dispatch_calls.h"

#include <sstream>

#include "ir/objc3_ir_type_model.h"

bool Objc3IRRuntimeDispatchCallRequestOwnsResult(
    const Objc3IRRuntimeDispatchCallRequest &request) {
  return Objc3LoweringStrictOwnerModelIsReady(
      request.result_owner, request.result_owner_model,
      request.strict_no_retired_route, request.strict_no_compatibility);
}

bool Objc3IRDirectDispatchCallRequestOwnsResult(
    const Objc3IRDirectDispatchCallRequest &request) {
  return Objc3LoweringStrictOwnerModelIsReady(
      request.result_owner, request.result_owner_model,
      request.strict_no_retired_route, request.strict_no_compatibility);
}

std::string BuildObjc3IRRuntimeDispatchCall(
    const Objc3IRRuntimeDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  " << request.result_value << " = call i32 @"
       << request.dispatch_symbol << "(";
  if (request.uses_typed_value_dispatch) {
    call << "i32 " << request.expected_return_kind << ", ";
  }
  call << "i32 " << request.receiver;
  if (request.uses_from_class_dispatch) {
    call << ", ptr " << request.lookup_start_class_ptr;
  }
  call << ", ptr " << request.selector_ptr;
  for (const std::string &arg : request.args) {
    call << ", i32 " << arg;
  }
  call << ")";
  return call.str();
}

std::string BuildObjc3IRDirectDispatchCall(
    const Objc3IRDirectDispatchCallRequest &request) {
  std::ostringstream call;
  call << "  ";
  if (request.return_type != ValueType::Void) {
    call << request.result_value << " = ";
  }
  call << "call " << LLVMScalarType(request.return_type) << " "
       << request.callee_symbol << "(";
  for (std::size_t i = 0;
       i < request.explicit_arg_count && i < request.args.size(); ++i) {
    if (i != 0) {
      call << ", ";
    }
    const ValueType arg_type =
        i < request.arg_types.size() ? request.arg_types[i] : ValueType::I32;
    call << LLVMScalarType(arg_type) << " " << request.args[i];
  }
  call << ")";
  return call.str();
}
