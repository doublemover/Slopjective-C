#pragma once

#include <string>
#include <vector>

#include "parse/objc3_parser_contract_types.h"
#include "sema/objc3_sema_contract.h"

struct Objc3SemanticBodyValidationContext {
  const Objc3Program *ast = nullptr;
  const Objc3SemanticIntegrationSurface *surface = nullptr;
  const Objc3SemanticValidationOptions *options = nullptr;
  std::vector<std::string> *diagnostics = nullptr;

  bool IsValid() const {
    return ast != nullptr && surface != nullptr && options != nullptr &&
           diagnostics != nullptr;
  }
};

Objc3SemanticBodyValidationContext BuildObjc3SemanticBodyValidationContext(
    const Objc3ParsedProgram &program,
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3SemanticValidationOptions &options,
    std::vector<std::string> &diagnostics);
