#pragma once

#include <string>

#include "pipeline/results/phase_parse_contract_result.h"
#include "pipeline/results/phase_semantic_lowering_result.h"
#include "pipeline/results/phase_toolchain_runtime_result.h"

struct Objc3ParseLoweringReadinessSurface
    : Objc3ParseContractPhaseResult,
      Objc3ToolchainRuntimePhaseResult,
      Objc3SemanticLoweringPhaseResult {
  std::string failure_reason;
};
