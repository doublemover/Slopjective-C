#include "artifacts/objc3_frontend_artifact_module_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendModuleMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &module_import_graph_lowering_replay_key,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const std::string &namespace_collision_shadowing_lowering_replay_key,
    const Objc3NamespaceCollisionShadowingLoweringContract
        &namespace_collision_shadowing_lowering_contract,
    const std::string &public_private_api_partition_lowering_replay_key,
    const Objc3PublicPrivateApiPartitionLoweringContract
        &public_private_api_partition_lowering_contract,
    const std::string
        &incremental_module_cache_invalidation_lowering_replay_key,
    const Objc3IncrementalModuleCacheInvalidationLoweringContract
        &incremental_module_cache_invalidation_lowering_contract,
    const std::string &cross_module_conformance_lowering_replay_key,
    const Objc3CrossModuleConformanceLoweringContract
        &cross_module_conformance_lowering_contract) {
  ir_frontend_metadata.lowering_module_import_graph_replay_key =
      module_import_graph_lowering_replay_key;
  ir_frontend_metadata.module_import_graph_lowering_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  ir_frontend_metadata.module_import_graph_lowering_import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.module_import_graph_lowering_namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.module_import_graph_lowering_object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.module_import_graph_lowering_pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.module_import_graph_lowering_normalized_sites =
      module_import_graph_lowering_contract.normalized_sites;
  ir_frontend_metadata.module_import_graph_lowering_contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_module_import_graph_lowering_handoff =
      module_import_graph_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_namespace_collision_shadowing_replay_key =
      namespace_collision_shadowing_lowering_replay_key;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_sites =
      namespace_collision_shadowing_lowering_contract
          .namespace_collision_shadowing_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_namespace_segment_sites =
      namespace_collision_shadowing_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_import_edge_candidate_sites =
      namespace_collision_shadowing_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_object_pointer_type_sites =
      namespace_collision_shadowing_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_pointer_declarator_sites =
      namespace_collision_shadowing_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_normalized_sites =
      namespace_collision_shadowing_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_contract_violation_sites =
      namespace_collision_shadowing_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_namespace_collision_shadowing_lowering_handoff =
      namespace_collision_shadowing_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_public_private_api_partition_replay_key =
      public_private_api_partition_lowering_replay_key;
  ir_frontend_metadata.public_private_api_partition_lowering_sites =
      public_private_api_partition_lowering_contract
          .public_private_api_partition_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_namespace_segment_sites =
      public_private_api_partition_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_import_edge_candidate_sites =
      public_private_api_partition_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_object_pointer_type_sites =
      public_private_api_partition_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_pointer_declarator_sites =
      public_private_api_partition_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.public_private_api_partition_lowering_normalized_sites =
      public_private_api_partition_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_contract_violation_sites =
      public_private_api_partition_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_public_private_api_partition_lowering_handoff =
      public_private_api_partition_lowering_contract.deterministic;

  ir_frontend_metadata
      .lowering_incremental_module_cache_invalidation_replay_key =
      incremental_module_cache_invalidation_lowering_replay_key;
  ir_frontend_metadata.incremental_module_cache_invalidation_lowering_sites =
      incremental_module_cache_invalidation_lowering_contract
          .incremental_module_cache_invalidation_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_namespace_segment_sites =
      incremental_module_cache_invalidation_lowering_contract
          .namespace_segment_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_object_pointer_type_sites =
      incremental_module_cache_invalidation_lowering_contract
          .object_pointer_type_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_pointer_declarator_sites =
      incremental_module_cache_invalidation_lowering_contract
          .pointer_declarator_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_normalized_sites =
      incremental_module_cache_invalidation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_contract_violation_sites =
      incremental_module_cache_invalidation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_incremental_module_cache_invalidation_lowering_handoff =
      incremental_module_cache_invalidation_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_cross_module_conformance_replay_key =
      cross_module_conformance_lowering_replay_key;
  ir_frontend_metadata.cross_module_conformance_lowering_sites =
      cross_module_conformance_lowering_contract.cross_module_conformance_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_namespace_segment_sites =
      cross_module_conformance_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_import_edge_candidate_sites =
      cross_module_conformance_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_object_pointer_type_sites =
      cross_module_conformance_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_pointer_declarator_sites =
      cross_module_conformance_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.cross_module_conformance_lowering_normalized_sites =
      cross_module_conformance_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_cache_invalidation_candidate_sites =
      cross_module_conformance_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_contract_violation_sites =
      cross_module_conformance_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_cross_module_conformance_lowering_handoff =
      cross_module_conformance_lowering_contract.deterministic;
}

}  // namespace objc3::artifacts::frontend
