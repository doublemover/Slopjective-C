#pragma once

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_frontend_diagnostic_stage_kind.h"
#include "contracts/objc3_frontend_diagnostic_stage_slice_record.h"
#include "contracts/objc3_native_contract_ids.h"

inline bool Objc3FrontendDiagnosticStageSliceIsValid(
    const Objc3FrontendDiagnosticStageSlice &slice) {
  return Objc3FrontendDiagnosticStageIsKnown(slice.stage) &&
         Objc3ContractIdMatches(
             slice.contract_id,
             Objc3NativeContractId::kFrontendDiagnosticsBusV1) &&
         slice.diagnostics != nullptr;
}
