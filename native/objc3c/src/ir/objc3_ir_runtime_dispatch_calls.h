#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3IRRuntimeDispatchCallRequest {
  std::string result_value;
  std::string dispatch_symbol;
  std::string receiver;
  std::string selector_ptr;
  std::vector<std::string> args;
};

struct Objc3IRDirectDispatchCallRequest {
  std::string result_value;
  std::string callee_symbol;
  std::vector<std::string> args;
  std::size_t explicit_arg_count = 0;
};

std::string BuildObjc3IRRuntimeDispatchCall(
    const Objc3IRRuntimeDispatchCallRequest &request);
std::string BuildObjc3IRDirectDispatchCall(
    const Objc3IRDirectDispatchCallRequest &request);
