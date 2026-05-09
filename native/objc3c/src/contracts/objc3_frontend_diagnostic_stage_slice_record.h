#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "contracts/objc3_frontend_diagnostic_stage_kind.h"
#include "contracts/objc3_frontend_diagnostics_bus_contract_id.h"

struct Objc3FrontendDiagnosticStageSlice {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view contract_id = Objc3FrontendDiagnosticsBusContractId();
  const std::vector<std::string> *diagnostics = nullptr;
};
