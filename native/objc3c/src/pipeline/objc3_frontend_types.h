#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_parser_contract.h"
#include "sema/objc3_semantic_passes.h"

struct Objc3FrontendOptions {
  Objc3LoweringContract lowering;
};

struct Objc3FrontendStageDiagnostics {
  std::vector<std::string> lexer;
  std::vector<std::string> parser;
  std::vector<std::string> semantic;
};

struct Objc3FrontendPipelineResult {
  Objc3ParsedProgram program;
  Objc3FrontendStageDiagnostics stage_diagnostics;
  Objc3SemanticIntegrationSurface integration_surface;
};
