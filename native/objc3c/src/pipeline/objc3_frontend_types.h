#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "parse/objc3_parser_contract.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_manager_contract.h"

inline constexpr std::uint8_t kObjc3DefaultLanguageVersion = 3u;

enum class Objc3FrontendCompatibilityMode : std::uint8_t {
  kCanonical = 0u,
  kLegacy = 1u,
};

struct Objc3FrontendOptions {
  std::uint8_t language_version = kObjc3DefaultLanguageVersion;
  Objc3FrontendCompatibilityMode compatibility_mode = Objc3FrontendCompatibilityMode::kCanonical;
  bool migration_assist = false;
  Objc3LoweringContract lowering;
};

struct Objc3FrontendMigrationHints {
  std::size_t legacy_yes_count = 0;
  std::size_t legacy_no_count = 0;
  std::size_t legacy_null_count = 0;

  std::size_t legacy_total() const { return legacy_yes_count + legacy_no_count + legacy_null_count; }
};

struct Objc3FrontendPipelineResult {
  Objc3ParsedProgram program;
  Objc3FrontendDiagnosticsBus stage_diagnostics;
  Objc3FrontendMigrationHints migration_hints;
  Objc3SemanticIntegrationSurface integration_surface;
  std::array<std::size_t, 3> sema_diagnostics_after_pass = {0, 0, 0};
  Objc3SemaParityContractSurface sema_parity_surface;
};
