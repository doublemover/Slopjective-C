#include "artifacts/objc3_frontend_artifact_type_system_lowering_plan.h"

#include <utility>

#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "lower/contracts/type_system_generic_lowering_validation_contracts.h"

namespace {

using ::IsValidObjc3GenericMetadataAbiLoweringContract;
using ::IsValidObjc3LightweightGenericsConstraintLoweringContract;
using ::IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract;
using ::IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract;
using ::IsValidObjc3VarianceBridgeCastLoweringContract;
using ::Objc3GenericMetadataAbiLoweringReplayKey;
using ::Objc3LightweightGenericsConstraintLoweringReplayKey;
using ::Objc3NullabilityFlowWarningPrecisionLoweringReplayKey;
using ::Objc3ProtocolQualifiedObjectTypeLoweringReplayKey;
using ::Objc3VarianceBridgeCastLoweringReplayKey;
using objc3::artifacts::frontend::BuildGenericMetadataAbiLoweringContract;
using objc3::artifacts::frontend::
    BuildLightweightGenericsConstraintLoweringContract;
using objc3::artifacts::frontend::
    BuildNullabilityFlowWarningPrecisionLoweringContract;
using objc3::artifacts::frontend::
    BuildProtocolQualifiedObjectTypeLoweringContract;
using objc3::artifacts::frontend::BuildVarianceBridgeCastLoweringContract;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

}  // namespace

Objc3FrontendArtifactTypeSystemLoweringPlan
BuildObjc3FrontendArtifactTypeSystemLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactTypeSystemLoweringPlan plan;
  plan.lightweight_generic_constraint_lowering_contract =
      BuildLightweightGenericsConstraintLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3LightweightGenericsConstraintLoweringContract(
          plan.lightweight_generic_constraint_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid lightweight generics constraint "
        "lowering contract");
  }
  plan.lightweight_generic_constraint_lowering_replay_key =
      Objc3LightweightGenericsConstraintLoweringReplayKey(
          plan.lightweight_generic_constraint_lowering_contract);
  plan.nullability_flow_warning_precision_lowering_contract =
      BuildNullabilityFlowWarningPrecisionLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
          plan.nullability_flow_warning_precision_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid nullability-flow warning-precision "
        "lowering contract");
  }
  plan.nullability_flow_warning_precision_lowering_replay_key =
      Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
          plan.nullability_flow_warning_precision_lowering_contract);
  plan.protocol_qualified_object_type_lowering_contract =
      BuildProtocolQualifiedObjectTypeLoweringContract(
          pipeline_result.sema_parity_surface);
  plan.protocol_qualified_object_type_lowering_replay_key =
      Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
          plan.protocol_qualified_object_type_lowering_contract);
  if (!IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
          plan.protocol_qualified_object_type_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid protocol-qualified object type "
        "lowering contract (" +
            plan.protocol_qualified_object_type_lowering_replay_key + ")");
  }
  plan.variance_bridge_cast_lowering_contract =
      BuildVarianceBridgeCastLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3VarianceBridgeCastLoweringContract(
          plan.variance_bridge_cast_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid variance/bridged-cast lowering "
        "contract");
  }
  plan.variance_bridge_cast_lowering_replay_key =
      Objc3VarianceBridgeCastLoweringReplayKey(
          plan.variance_bridge_cast_lowering_contract);
  plan.generic_metadata_abi_lowering_contract =
      BuildGenericMetadataAbiLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3GenericMetadataAbiLoweringContract(
          plan.generic_metadata_abi_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid generic metadata ABI lowering "
        "contract");
  }
  plan.generic_metadata_abi_lowering_replay_key =
      Objc3GenericMetadataAbiLoweringReplayKey(
          plan.generic_metadata_abi_lowering_contract);
  return plan;
}
