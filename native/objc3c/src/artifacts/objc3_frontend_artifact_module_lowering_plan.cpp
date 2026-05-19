#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"

#include <utility>

namespace {

using objc3::artifacts::frontend::BuildCrossModuleConformanceLoweringContract;
using objc3::artifacts::frontend::
    BuildIncrementalModuleCacheInvalidationLoweringContract;
using objc3::artifacts::frontend::
    BuildNamespaceCollisionShadowingLoweringContract;
using objc3::artifacts::frontend::
    BuildPublicPrivateApiPartitionLoweringContract;
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

Objc3FrontendArtifactModuleLoweringPlan
BuildObjc3FrontendArtifactModuleLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactModuleLoweringPlan plan;
  plan.namespace_collision_shadowing_lowering_contract =
      BuildNamespaceCollisionShadowingLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NamespaceCollisionShadowingLoweringContract(
          plan.namespace_collision_shadowing_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid namespace collision shadowing "
        "lowering contract");
  }
  plan.namespace_collision_shadowing_lowering_replay_key =
      Objc3NamespaceCollisionShadowingLoweringReplayKey(
          plan.namespace_collision_shadowing_lowering_contract);
  plan.public_private_api_partition_lowering_contract =
      BuildPublicPrivateApiPartitionLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PublicPrivateApiPartitionLoweringContract(
          plan.public_private_api_partition_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid public-private API partition "
        "lowering contract");
  }
  plan.public_private_api_partition_lowering_replay_key =
      Objc3PublicPrivateApiPartitionLoweringReplayKey(
          plan.public_private_api_partition_lowering_contract);
  plan.incremental_module_cache_invalidation_lowering_contract =
      BuildIncrementalModuleCacheInvalidationLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
          plan.incremental_module_cache_invalidation_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid incremental module cache "
        "invalidation lowering contract");
  }
  plan.incremental_module_cache_invalidation_lowering_replay_key =
      Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
          plan.incremental_module_cache_invalidation_lowering_contract);
  plan.cross_module_conformance_lowering_contract =
      BuildCrossModuleConformanceLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3CrossModuleConformanceLoweringContract(
          plan.cross_module_conformance_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid cross-module conformance lowering "
        "contract");
  }
  plan.cross_module_conformance_lowering_replay_key =
      Objc3CrossModuleConformanceLoweringReplayKey(
          plan.cross_module_conformance_lowering_contract);
  return plan;
}
