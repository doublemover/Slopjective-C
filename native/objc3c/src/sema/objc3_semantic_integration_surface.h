#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(
    const Objc3ParsedProgram &program,
    Objc3SemaLanguageProfile language_profile,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics);
