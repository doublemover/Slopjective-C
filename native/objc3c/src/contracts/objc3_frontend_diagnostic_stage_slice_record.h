#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "contracts/objc3_frontend_diagnostic_stage_kind.h"
#include "contracts/objc3_frontend_diagnostics_bus_contract_id.h"

struct Objc3FrontendDiagnosticStageSlice {
  Objc3FrontendDiagnosticStage stage = Objc3FrontendDiagnosticStage::kLexer;
  std::string_view contract_id = Objc3FrontendDiagnosticsBusContractId();
  const std::vector<std::string> *diagnostics = nullptr;
  std::string_view owner_contract_id = kObjc3DiagnosticOwnerContractId;
  std::string_view owner_model = kObjc3DiagnosticNoFallbackOwnerModel;
  bool fallback_allowed = false;
  bool compatibility_shim_allowed = false;
  bool recovery_counts_as_success = false;
};
