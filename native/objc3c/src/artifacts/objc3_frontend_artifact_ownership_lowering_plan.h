#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

struct Objc3FrontendArtifactOwnershipAwareLoweringPlan {
  Objc3OwnershipQualifierLoweringContract
      ownership_qualifier_lowering_contract;
  std::string ownership_qualifier_lowering_replay_key;
  Objc3RetainReleaseOperationLoweringContract
      retain_release_operation_lowering_contract;
  std::string retain_release_operation_lowering_replay_key;
  Objc3AutoreleasePoolScopeLoweringContract
      autoreleasepool_scope_lowering_contract;
  std::string autoreleasepool_scope_lowering_replay_key;
  Objc3WeakUnownedSemanticsLoweringContract
      weak_unowned_semantics_lowering_contract;
  std::string weak_unowned_semantics_lowering_replay_key;
  Objc3ArcDiagnosticsFixitLoweringContract
      arc_diagnostics_fixit_lowering_contract;
  std::string arc_diagnostics_fixit_lowering_replay_key;
  Objc3OwnershipAwareLoweringBehaviorScaffold
      ownership_aware_lowering_behavior_scaffold;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactOwnershipAwareLoweringPlan
BuildObjc3FrontendArtifactOwnershipAwareLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result,
    bool metadata_only_ir_emission_mode);
