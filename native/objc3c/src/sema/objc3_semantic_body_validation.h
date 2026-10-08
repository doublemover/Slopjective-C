#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

void ValidatePureContractSemanticDiagnostics(
    const Objc3ParsedProgram &program,
    const std::unordered_map<std::string, FunctionInfo> &surface_functions,
    std::vector<std::string> &diagnostics);

void ValidateSemanticBodies(
    const Objc3ParsedProgram &program,
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3SemanticValidationOptions &options,
    Objc3SemaLanguageProfile language_profile,
    std::vector<std::string> &diagnostics);

void RefreshSemanticIntegrationSurfaceAfterBodyValidation(
    const Objc3ParsedProgram &program,
    Objc3SemanticIntegrationSurface &surface);
