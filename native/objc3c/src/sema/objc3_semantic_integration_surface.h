#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract.h"

Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(
    const Objc3ParsedProgram &program,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled,
    std::vector<std::string> &diagnostics);
