#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "parse/objc3_parser_contract.h"

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_has_invalid_type_suffix;
  ValueType return_type = ValueType::I32;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  bool built = false;
};

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
};

bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);
Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,
                                                                std::vector<std::string> &diagnostics);
void ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,
                                             const std::unordered_map<std::string, FunctionInfo> &surface_functions,
                                             std::vector<std::string> &diagnostics);
void ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,
                            const Objc3SemanticValidationOptions &options,
                            std::vector<std::string> &diagnostics);
