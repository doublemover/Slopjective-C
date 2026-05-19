#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceTerminalSemanticFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context) {
  const auto &pipeline_result = context.pipeline_result;
  const auto &core_lowering_plan = context.core_lowering_plan;
  const auto &type_system_type_semantic_model_summary =
      context.type_system_type_semantic_model_summary;

  manifest
      << ",\"objc_error_handling_error_semantic_model\":"
      << BuildErrorHandlingErrorSemanticModelSummaryJson(
             pipeline_result.error_handling_error_semantic_model_summary)
      << ",\"objc_error_handling_try_do_catch_semantics\":"
      << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
             pipeline_result.error_handling_try_do_catch_semantic_summary)
      << ",\"objc_error_handling_error_bridge_legality\":"
      << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
             pipeline_result.error_handling_error_bridge_legality_summary)
      << ",\"objc_control_flow_control_flow_semantic_model\":"
      << BuildControlFlowControlFlowSemanticModelSummaryJson(
             pipeline_result.control_flow_control_flow_semantic_model_summary)
      << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
      << BuildControlFlowControlFlowSafetyLoweringContractJson(
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_contract,
             pipeline_result.control_flow_control_flow_semantic_model_summary,
             pipeline_result.control_flow_control_flow_semantic_model_summary
                 .replay_key,
             core_lowering_plan
                 .control_flow_control_flow_safety_lowering_replay_key)
      << ",\"objc_type_system_type_semantic_model\":"
      << BuildTypeSystemTypeSemanticModelSummaryJson(
             type_system_type_semantic_model_summary);
}

}  // namespace objc3::artifacts::frontend
