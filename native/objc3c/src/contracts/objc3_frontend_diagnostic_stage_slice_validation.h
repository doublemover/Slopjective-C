#pragma once

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_frontend_diagnostics_bus_contract_id.h"
#include "contracts/objc3_frontend_diagnostic_stage_known.h"
#include "contracts/objc3_frontend_diagnostic_stage_slice_record.h"
#include "contracts/objc3_native_contract_ids.h"

inline bool Objc3FrontendDiagnosticStageSliceIsValid(
    const Objc3FrontendDiagnosticStageSlice &slice) {
  return Objc3FrontendDiagnosticStageIsKnown(slice.stage) &&
         Objc3DiagnosticStageIsHardCutover(slice.stage) &&
         Objc3ContractIdMatches(
             slice.contract_id,
             objc3c::contracts::kObjc3FrontendDiagnosticsBusContract) &&
         slice.diagnostics != nullptr &&
         slice.owner_contract_id == kObjc3DiagnosticOwnerContractId &&
         slice.owner_model == kObjc3DiagnosticNoRetiredRouteOwnerModel &&
         !slice.retired_route_allowed && !slice.compatibility_gate_allowed &&
         !slice.recovery_counts_as_success;
}
