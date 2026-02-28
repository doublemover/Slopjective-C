#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/objc3_ast.h"
#include "lower/objc3_lowering_contract.h"

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_has_invalid_type_suffix;
  ValueType return_type = ValueType::I32;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3FrontendOptions {
  Objc3LoweringContract lowering;
};

struct Objc3FrontendStageDiagnostics {
  std::vector<std::string> lexer;
  std::vector<std::string> parser;
  std::vector<std::string> semantic;
};

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  bool built = false;
};

struct Objc3FrontendPipelineResult {
  Objc3Program program;
  Objc3FrontendStageDiagnostics stage_diagnostics;
  Objc3SemanticIntegrationSurface integration_surface;
};
