#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "lower/contracts/lowering_ownership_contracts.h"

struct Objc3IRRuntimeDispatchCallRequest {
  std::string result_value;
  std::string result_owner = kObjc3IRRuntimeDispatchResultOwner;
  std::string result_owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  std::string dispatch_symbol;
  std::string receiver;
  std::string lookup_start_class_ptr;
  std::string selector_ptr;
  std::vector<std::string> args;
  int expected_return_kind = 0;
  bool uses_typed_value_dispatch = false;
  bool uses_from_class_dispatch = false;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
};

struct Objc3IRDirectDispatchCallRequest {
  std::string result_value;
  std::string result_owner = kObjc3IRDirectDispatchResultOwner;
  std::string result_owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  std::string callee_symbol;
  std::vector<std::string> args;
  std::vector<ValueType> arg_types;
  ValueType return_type = ValueType::I32;
  std::size_t explicit_arg_count = 0;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
};

struct Objc3IRCacheAwareDispatchCallRequest {
  std::string result_value;
  std::string result_envelope_value;
  std::string prepare_status_value;
  std::string prepare_status_ok_value;
  std::string status_value;
  std::string status_ok_value;
  std::string descriptor_ptr;
  std::string selector_ptr;
  std::string source_path_ptr = "null";
  std::string receiver;
  std::vector<std::string> args;
  unsigned source_line = 1;
  unsigned source_column = 1;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
};

bool Objc3IRRuntimeDispatchCallRequestOwnsResult(
    const Objc3IRRuntimeDispatchCallRequest &request);
bool Objc3IRDirectDispatchCallRequestOwnsResult(
    const Objc3IRDirectDispatchCallRequest &request);
bool Objc3IRCacheAwareDispatchCallRequestOwnsResult(
    const Objc3IRCacheAwareDispatchCallRequest &request);
const char *Objc3IRRuntimeDispatchI32ResultType();
const char *Objc3IRCacheAwareDispatchDescriptorType();
std::string BuildObjc3IRRuntimeDispatchCall(
    const Objc3IRRuntimeDispatchCallRequest &request);
std::string BuildObjc3IRDirectDispatchCall(
    const Objc3IRDirectDispatchCallRequest &request);
std::vector<std::string> BuildObjc3IRCacheAwareDispatchCall(
    const Objc3IRCacheAwareDispatchCallRequest &request,
    const std::string &strict_failure_label,
    const std::string &value_label);
