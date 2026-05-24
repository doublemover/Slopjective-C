#include "ir/objc3_ir_runtime_dispatch_calls.h"

#include <sstream>
#include <vector>

#include "ir/objc3_ir_type_model.h"
#include "lower/contracts/runtime_dispatch_strict_abi_entrypoint_contracts.h"

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

bool Objc3IRCacheAwareDispatchCallRequestOwnsResult(
    const Objc3IRCacheAwareDispatchCallRequest &request) {
  return !request.result_value.empty() && !request.result_envelope_value.empty() &&
         !request.prepare_status_value.empty() &&
         !request.prepare_status_ok_value.empty() &&
         !request.status_value.empty() && !request.status_ok_value.empty() &&
         !request.descriptor_ptr.empty() &&
         Objc3LoweringStrictOwnerModelIsReady(
             kObjc3IRRuntimeDispatchResultOwner,
             kObjc3LoweringNoRetiredRouteOwnerModel,
             request.strict_no_retired_route,
             request.strict_no_compatibility);
}

const char *Objc3IRRuntimeDispatchI32ResultType() {
  return "{ i32, i32, i32, i32, i32, ptr, ptr, ptr, ptr, ptr }";
}

const char *Objc3IRCacheAwareDispatchDescriptorType() {
  return "{ i32, i32, ptr, i64, i64, i64, i64, i64, i64, ptr, i32, i32 }";
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
  if (!request.throws_error_slot_ptr.empty()) {
    call << ", ptr " << request.throws_error_slot_ptr;
  }
  call << ")";
  return call.str();
}

std::vector<std::string> BuildObjc3IRCacheAwareDispatchCall(
    const Objc3IRCacheAwareDispatchCallRequest &request,
    const std::string &strict_failure_label,
    const std::string &value_label) {
  const std::string descriptor_type = Objc3IRCacheAwareDispatchDescriptorType();
  const std::string result_type = Objc3IRRuntimeDispatchI32ResultType();
  std::vector<std::string> lines;
  lines.reserve(32);
  lines.push_back("  ; semantic-optimization.cache-aware-dispatch: "
                  "runtime-owned helper, strict checked status envelope");
  lines.push_back("  ; source-map.cache-aware-dispatch: line=" +
                  std::to_string(request.source_line) + ";column=" +
                  std::to_string(request.source_column) +
                  ";debug-visible=true;preserves-source-map=true");
  lines.push_back("  " + request.descriptor_ptr + " = alloca " +
                  descriptor_type + ", align 8");
  lines.push_back("  " + request.prepare_status_value + " = call i32 @" +
                  kObjc3RuntimePrepareCacheAwareDispatchDescriptorSymbol +
                  "(ptr " + request.descriptor_ptr + ", ptr " +
                  request.selector_ptr + ", ptr " + request.source_path_ptr +
                  ", i32 " + std::to_string(request.source_line) + ", i32 " +
                  std::to_string(request.source_column) + ")");
  lines.push_back("  " + request.prepare_status_ok_value + " = icmp sge i32 " +
                  request.prepare_status_value + ", 0");
  const std::string dispatch_label = value_label + ".dispatch";
  const std::string prepare_failure_label = strict_failure_label + ".prepare";
  lines.push_back("  br i1 " + request.prepare_status_ok_value + ", label %" +
                  dispatch_label + ", label %" + prepare_failure_label);
  lines.push_back(prepare_failure_label + ":");
  lines.push_back("  call void @" +
                  std::string(kObjc3RuntimeAbortDispatchStatusI32Symbol) +
                  "(i32 " + request.prepare_status_value + ")");
  lines.push_back("  unreachable");
  lines.push_back(dispatch_label + ":");

  std::ostringstream call;
  call << "  " << request.result_envelope_value << " = call " << result_type
       << " @" << kObjc3RuntimeCacheAwareDispatchI32CheckedSymbol << "(i32 "
       << request.receiver << ", ptr " << request.descriptor_ptr;
  for (const std::string &arg : request.args) {
    call << ", i32 " << arg;
  }
  call << ")";
  lines.push_back(call.str());
  lines.push_back("  " + request.status_value + " = extractvalue " +
                  result_type + " " + request.result_envelope_value + ", 2");
  lines.push_back("  " + request.status_ok_value + " = icmp sge i32 " +
                  request.status_value + ", 0");
  lines.push_back("  br i1 " + request.status_ok_value + ", label %" +
                  value_label + ", label %" + strict_failure_label);
  lines.push_back(strict_failure_label + ":");
  lines.push_back("  call void @" +
                  std::string(kObjc3RuntimeAbortDispatchStatusI32Symbol) +
                  "(i32 " + request.status_value + ")");
  lines.push_back("  unreachable");
  lines.push_back(value_label + ":");
  lines.push_back("  " + request.result_value + " = extractvalue " +
                  result_type + " " + request.result_envelope_value + ", 4");
  return lines;
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
  if (!request.throws_error_slot_ptr.empty()) {
    if (request.explicit_arg_count > 0) {
      call << ", ";
    }
    call << "ptr " << request.throws_error_slot_ptr;
  }
  call << ")";
  return call.str();
}
