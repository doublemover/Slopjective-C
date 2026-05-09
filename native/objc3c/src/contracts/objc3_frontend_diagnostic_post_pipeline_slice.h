#pragma once

#include <string>
#include <vector>

#include "contracts/objc3_frontend_diagnostic_stage_slice_record.h"

inline Objc3FrontendDiagnosticStageSlice PostPipelineDiagnosticSlice(
    const std::vector<std::string> &post_pipeline_diagnostics) {
  return Objc3FrontendDiagnosticStageSlice{
      Objc3FrontendDiagnosticStage::kPostPipeline,
      Objc3FrontendDiagnosticsBusContractId(),
      &post_pipeline_diagnostics,
      kObjc3DiagnosticOwnerContractId,
      kObjc3DiagnosticNoFallbackOwnerModel,
      false,
      false,
      false};
}
