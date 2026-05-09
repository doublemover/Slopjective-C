#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "lower/contracts/lowering_ownership_contracts.h"

struct Objc3IRRuntimeDispatchCallRequest {
  std::string result_value;
  std::string result_owner = kObjc3IRRuntimeDispatchResultOwner;
  std::string result_owner_model = kObjc3LoweringNoFallbackOwnerModel;
  std::string dispatch_symbol;
  std::string receiver;
  std::string selector_ptr;
  std::vector<std::string> args;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
};

struct Objc3IRDirectDispatchCallRequest {
  std::string result_value;
  std::string result_owner = kObjc3IRDirectDispatchResultOwner;
  std::string result_owner_model = kObjc3LoweringNoFallbackOwnerModel;
  std::string callee_symbol;
  std::vector<std::string> args;
  std::size_t explicit_arg_count = 0;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
};

bool Objc3IRRuntimeDispatchCallRequestOwnsResult(
    const Objc3IRRuntimeDispatchCallRequest &request);
bool Objc3IRDirectDispatchCallRequestOwnsResult(
    const Objc3IRDirectDispatchCallRequest &request);
std::string BuildObjc3IRRuntimeDispatchCall(
    const Objc3IRRuntimeDispatchCallRequest &request);
std::string BuildObjc3IRDirectDispatchCall(
    const Objc3IRDirectDispatchCallRequest &request);
