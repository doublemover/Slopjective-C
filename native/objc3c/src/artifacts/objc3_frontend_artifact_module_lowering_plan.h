#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_module_semantic_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactModuleLoweringPlan {
  Objc3NamespaceCollisionShadowingLoweringContract
      namespace_collision_shadowing_lowering_contract;
  std::string namespace_collision_shadowing_lowering_replay_key;
  Objc3PublicPrivateApiPartitionLoweringContract
      public_private_api_partition_lowering_contract;
  std::string public_private_api_partition_lowering_replay_key;
  Objc3IncrementalModuleCacheInvalidationLoweringContract
      incremental_module_cache_invalidation_lowering_contract;
  std::string incremental_module_cache_invalidation_lowering_replay_key;
  Objc3CrossModuleConformanceLoweringContract
      cross_module_conformance_lowering_contract;
  std::string cross_module_conformance_lowering_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactModuleLoweringPlan
BuildObjc3FrontendArtifactModuleLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
