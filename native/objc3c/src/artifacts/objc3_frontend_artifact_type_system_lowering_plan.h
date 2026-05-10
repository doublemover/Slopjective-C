#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactTypeSystemLoweringPlan {
  objc3::artifacts::frontend::
      Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
      lightweight_generic_constraint_lowering_contract;
  std::string lightweight_generic_constraint_lowering_replay_key;
  objc3::artifacts::frontend::
      Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
      nullability_flow_warning_precision_lowering_contract;
  std::string nullability_flow_warning_precision_lowering_replay_key;
  objc3::artifacts::frontend::
      Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
      protocol_qualified_object_type_lowering_contract;
  std::string protocol_qualified_object_type_lowering_replay_key;
  objc3::artifacts::frontend::
      Objc3FrontendVarianceBridgeCastLoweringContractRecord
      variance_bridge_cast_lowering_contract;
  std::string variance_bridge_cast_lowering_replay_key;
  objc3::artifacts::frontend::
      Objc3FrontendGenericMetadataAbiLoweringContractRecord
      generic_metadata_abi_lowering_contract;
  std::string generic_metadata_abi_lowering_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactTypeSystemLoweringPlan
BuildObjc3FrontendArtifactTypeSystemLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
