#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "parse/objc3_parser_contract.h"
#include "sema/objc3_sema_contract.h"

struct Objc3FrontendOptions {
  Objc3LoweringContract lowering;
};

struct Objc3FrontendPipelineResult {
  Objc3ParsedProgram program;
  Objc3FrontendDiagnosticsBus stage_diagnostics;
  Objc3SemanticIntegrationSurface integration_surface;
  std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};
};
