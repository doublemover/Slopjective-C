#pragma once

#include <cstddef>

#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_sema_pass_manager_contract_flow.h"
#include "sema/objc3_semantic_passes.h"

inline Objc3TypeSystemTypeSemanticModelSummary
BuildObjc3FrontendArtifactTypeSemanticModelSummary(
    const Objc3FrontendPipelineResult &pipeline_result,
    std::size_t max_message_send_args) {
  return BuildTypeSystemTypeSemanticModelSummary(
      pipeline_result.program.ast, pipeline_result.integration_surface,
      max_message_send_args);
}

inline bool Objc3FrontendArtifactSemaParitySurfaceReady(
    const Objc3FrontendPipelineResult &pipeline_result) {
  return IsReadyObjc3SemaParityContractSurface(
      pipeline_result.sema_parity_surface);
}
