#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "lower/contracts/type_system_generic_lowering_contract_records.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactTypeSystemLoweringPlan {
  Objc3LightweightGenericsConstraintLoweringContract
      lightweight_generic_constraint_lowering_contract;
  std::string lightweight_generic_constraint_lowering_replay_key;
  Objc3NullabilityFlowWarningPrecisionLoweringContract
      nullability_flow_warning_precision_lowering_contract;
  std::string nullability_flow_warning_precision_lowering_replay_key;
  Objc3ProtocolQualifiedObjectTypeLoweringContract
      protocol_qualified_object_type_lowering_contract;
  std::string protocol_qualified_object_type_lowering_replay_key;
  Objc3VarianceBridgeCastLoweringContract
      variance_bridge_cast_lowering_contract;
  std::string variance_bridge_cast_lowering_replay_key;
  Objc3GenericMetadataAbiLoweringContract
      generic_metadata_abi_lowering_contract;
  std::string generic_metadata_abi_lowering_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactTypeSystemLoweringPlan
BuildObjc3FrontendArtifactTypeSystemLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
