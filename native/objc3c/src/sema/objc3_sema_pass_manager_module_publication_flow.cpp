#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaModuleSemanticParityPublicationReadinessRecord
BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_generic_metadata_abi_handoff,
    bool deterministic_module_import_graph_handoff,
    bool deterministic_namespace_collision_shadowing_handoff,
    bool deterministic_public_private_api_partition_handoff,
    bool deterministic_incremental_module_cache_invalidation_handoff) {
  Objc3SemaModuleSemanticParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.generic_metadata_abi_ready =
      deterministic_generic_metadata_abi_handoff &&
      surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          surface.generic_metadata_abi_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites ==
          surface.generic_metadata_abi_generic_suffix_sites_total &&
      surface.generic_metadata_abi_summary.protocol_composition_sites ==
          surface.generic_metadata_abi_protocol_composition_sites_total &&
      surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          surface.generic_metadata_abi_object_pointer_type_sites_total &&
      surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          surface.generic_metadata_abi_pointer_declarator_sites_total &&
      surface.generic_metadata_abi_summary.normalized_sites ==
          surface.generic_metadata_abi_normalized_sites_total &&
      surface.generic_metadata_abi_summary.contract_violation_sites ==
          surface.generic_metadata_abi_contract_violation_sites_total &&
      surface.generic_metadata_abi_summary.generic_suffix_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.protocol_composition_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.normalized_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.contract_violation_sites <=
          surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      surface.generic_metadata_abi_summary.deterministic;
  record.module_import_graph_ready =
      deterministic_module_import_graph_handoff &&
      surface.module_import_graph_summary.module_import_graph_sites ==
          surface.module_import_graph_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites ==
          surface.module_import_graph_import_edge_candidate_sites_total &&
      surface.module_import_graph_summary.namespace_segment_sites ==
          surface.module_import_graph_namespace_segment_sites_total &&
      surface.module_import_graph_summary.object_pointer_type_sites ==
          surface.module_import_graph_object_pointer_type_sites_total &&
      surface.module_import_graph_summary.pointer_declarator_sites ==
          surface.module_import_graph_pointer_declarator_sites_total &&
      surface.module_import_graph_summary.normalized_sites ==
          surface.module_import_graph_normalized_sites_total &&
      surface.module_import_graph_summary.contract_violation_sites ==
          surface.module_import_graph_contract_violation_sites_total &&
      surface.module_import_graph_summary.import_edge_candidate_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.namespace_segment_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.normalized_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.contract_violation_sites <=
          surface.module_import_graph_summary.module_import_graph_sites &&
      surface.module_import_graph_summary.deterministic;
  record.namespace_collision_shadowing_ready =
      deterministic_namespace_collision_shadowing_handoff &&
      surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          surface.namespace_collision_shadowing_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites ==
          surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      surface.namespace_collision_shadowing_summary.object_pointer_type_sites ==
          surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      surface.namespace_collision_shadowing_summary.pointer_declarator_sites ==
          surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      surface.namespace_collision_shadowing_summary.normalized_sites ==
          surface.namespace_collision_shadowing_normalized_sites_total &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites ==
          surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      surface.namespace_collision_shadowing_summary.namespace_segment_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.normalized_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.contract_violation_sites <=
          surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      surface.namespace_collision_shadowing_summary.deterministic;
  record.public_private_api_partition_ready =
      deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary
              .import_edge_candidate_sites ==
          surface.public_private_api_partition_import_edge_candidate_sites_total &&
      surface.public_private_api_partition_summary.object_pointer_type_sites ==
          surface.public_private_api_partition_object_pointer_type_sites_total &&
      surface.public_private_api_partition_summary.pointer_declarator_sites ==
          surface.public_private_api_partition_pointer_declarator_sites_total &&
      surface.public_private_api_partition_summary.normalized_sites ==
          surface.public_private_api_partition_normalized_sites_total &&
      surface.public_private_api_partition_summary.contract_violation_sites ==
          surface.public_private_api_partition_contract_violation_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.normalized_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.contract_violation_sites <=
          surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      surface.public_private_api_partition_summary.deterministic;
  record.incremental_module_cache_invalidation_ready =
      deterministic_incremental_module_cache_invalidation_handoff &&
      surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          surface.incremental_module_cache_invalidation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites ==
          surface.incremental_module_cache_invalidation_normalized_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.normalized_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      surface.incremental_module_cache_invalidation_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.generic_metadata_abi_ready) +
      Objc3SemaEvidenceCount(record.module_import_graph_ready) +
      Objc3SemaEvidenceCount(record.namespace_collision_shadowing_ready) +
      Objc3SemaEvidenceCount(record.public_private_api_partition_ready) +
      Objc3SemaEvidenceCount(
          record.incremental_module_cache_invalidation_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_semantic_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 5u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}
