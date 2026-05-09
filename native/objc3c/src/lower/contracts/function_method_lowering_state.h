#pragma once

#include "ast/objc3_ast_declarations.h"

#include <cstddef>
#include <string>

struct Objc3CallableLoweringState {
  std::string owner_symbol;
  std::string callable_name;
  std::string signature_replay_key;
  bool method = false;
  bool class_method = false;
  bool has_body = false;
  bool async_declared = false;
  bool throws_declared = false;
  bool runtime_dispatch_required = false;
  bool arc_sensitive = false;
  std::size_t parameter_count = 0;
  std::string replay_key;
};

Objc3CallableLoweringState Objc3BuildFunctionLoweringState(
    const FunctionDecl &function);
Objc3CallableLoweringState Objc3BuildMethodLoweringState(
    const Objc3MethodDecl &method);
bool Objc3CallableLoweringStateRequiresRuntimeHelpers(
    const Objc3CallableLoweringState &state);
std::string Objc3CallableLoweringStateReplayKey(
    const Objc3CallableLoweringState &state);
