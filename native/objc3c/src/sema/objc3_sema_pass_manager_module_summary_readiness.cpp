#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaModuleTypeAbiSummaryReadinessRecord
BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleTypeAbiSummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.variance_bridge_cast_ready =
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          surface.variance_bridge_cast_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites ==
          surface.variance_bridge_cast_protocol_composition_sites_total &&
      surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          surface.variance_bridge_cast_object_pointer_type_sites_total &&
      surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          surface.variance_bridge_cast_pointer_declarator_sites_total &&
      surface.variance_bridge_cast_summary.normalized_sites ==
          surface.variance_bridge_cast_normalized_sites_total &&
      surface.variance_bridge_cast_summary.contract_violation_sites ==
          surface.variance_bridge_cast_contract_violation_sites_total &&
      surface.variance_bridge_cast_summary.protocol_composition_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.normalized_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.contract_violation_sites <=
          surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      surface.variance_bridge_cast_summary.deterministic;
  record.generic_metadata_abi_ready =
      surface.deterministic_generic_metadata_abi_handoff &&
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
      surface.deterministic_module_import_graph_handoff &&
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
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_type_abi_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.variance_bridge_cast_ready &&
      record.generic_metadata_abi_ready && record.module_import_graph_ready;
  return record;
}

Objc3SemaModuleBoundarySummaryReadinessRecord
BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaModuleBoundarySummaryReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.namespace_collision_shadowing_ready =
      surface.deterministic_namespace_collision_shadowing_handoff &&
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
      surface.deterministic_public_private_api_partition_handoff &&
      surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          surface.public_private_api_partition_sites_total &&
      surface.public_private_api_partition_summary.namespace_segment_sites ==
          surface.public_private_api_partition_namespace_segment_sites_total &&
      surface.public_private_api_partition_summary.import_edge_candidate_sites ==
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
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
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
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.module_boundary_summary_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.namespace_collision_shadowing_ready &&
      record.public_private_api_partition_ready &&
      record.incremental_module_cache_invalidation_ready;
  return record;
}
