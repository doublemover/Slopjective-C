#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_field_modules.h"

#include <string>

#include "artifacts/objc3_frontend_artifact_module_lowering_plan.h"
#include "lower/contracts/cross_module_lowering_contracts.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffModuleManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan) {
  const Objc3NamespaceCollisionShadowingLoweringContract
      &namespace_collision_shadowing_lowering_contract =
          module_lowering_plan.namespace_collision_shadowing_lowering_contract;
  const std::string &namespace_collision_shadowing_lowering_replay_key =
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key;
  const Objc3PublicPrivateApiPartitionLoweringContract
      &public_private_api_partition_lowering_contract =
          module_lowering_plan.public_private_api_partition_lowering_contract;
  const std::string &public_private_api_partition_lowering_replay_key =
      module_lowering_plan.public_private_api_partition_lowering_replay_key;
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      &incremental_module_cache_invalidation_lowering_contract =
          module_lowering_plan
              .incremental_module_cache_invalidation_lowering_contract;
  const std::string &incremental_module_cache_invalidation_lowering_replay_key =
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key;
  const Objc3CrossModuleConformanceLoweringContract
      &cross_module_conformance_lowering_contract =
          module_lowering_plan.cross_module_conformance_lowering_contract;
  const std::string &cross_module_conformance_lowering_replay_key =
      module_lowering_plan.cross_module_conformance_lowering_replay_key;

  manifest << ",\"deterministic_namespace_collision_shadowing_lowering_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"namespace_collision_shadowing_lowering_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_collision_shadowing_lowering_namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_segment_sites
           << ",\"namespace_collision_shadowing_lowering_import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"namespace_collision_shadowing_lowering_object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"namespace_collision_shadowing_lowering_pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"namespace_collision_shadowing_lowering_normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"namespace_collision_shadowing_lowering_contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_namespace_collision_shadowing_replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\""
           << ",\"deterministic_public_private_api_partition_lowering_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"public_private_api_partition_lowering_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"public_private_api_partition_lowering_namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"public_private_api_partition_lowering_import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"public_private_api_partition_lowering_object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"public_private_api_partition_lowering_pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"public_private_api_partition_lowering_normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"public_private_api_partition_lowering_contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_public_private_api_partition_replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\""
           << ",\"deterministic_incremental_module_cache_invalidation_lowering_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << ",\"incremental_module_cache_invalidation_lowering_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"incremental_module_cache_invalidation_lowering_namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"incremental_module_cache_invalidation_lowering_import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"incremental_module_cache_invalidation_lowering_pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"incremental_module_cache_invalidation_lowering_normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_incremental_module_cache_invalidation_replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\""
           << ",\"deterministic_cross_module_conformance_lowering_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"cross_module_conformance_lowering_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"cross_module_conformance_lowering_namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"cross_module_conformance_lowering_import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .import_edge_candidate_sites
           << ",\"cross_module_conformance_lowering_object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"cross_module_conformance_lowering_pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"cross_module_conformance_lowering_normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cross_module_conformance_lowering_cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"cross_module_conformance_lowering_contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"lowering_cross_module_conformance_replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\"";
}

}  // namespace objc3::artifacts::frontend
