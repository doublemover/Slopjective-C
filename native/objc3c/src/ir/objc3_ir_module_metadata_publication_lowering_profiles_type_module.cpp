#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_type_module.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataTypeModuleLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_lightweight_generic_constraint_lowering_profile = generic_constraint_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_generic_constraint_sites
      << ", generic_suffix_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_generic_suffix_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_object_pointer_type_sites
      << ", terminated_generic_suffix_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_pointer_declarator_sites
      << ", normalized_constraint_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_normalized_constraint_sites
      << ", contract_violation_sites="
      << frontend_metadata_.lightweight_generic_constraint_lowering_contract_violation_sites
      << ", deterministic_lightweight_generic_constraint_lowering_handoff="
      << (frontend_metadata_.deterministic_lightweight_generic_constraint_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_nullability_flow_warning_precision_lowering_profile = nullability_flow_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_object_pointer_type_sites
      << ", nullability_suffix_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_nullability_suffix_sites
      << ", nullable_suffix_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_nullable_suffix_sites
      << ", nonnull_suffix_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_nonnull_suffix_sites
      << ", normalized_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.nullability_flow_warning_precision_lowering_contract_violation_sites
      << ", deterministic_nullability_flow_warning_precision_lowering_handoff="
      << (frontend_metadata_.deterministic_nullability_flow_warning_precision_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_protocol_qualified_object_type_lowering_profile = protocol_qualified_object_type_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_sites
      << ", protocol_composition_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_protocol_composition_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_object_pointer_type_sites
      << ", terminated_protocol_composition_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_pointer_declarator_sites
      << ", normalized_protocol_composition_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites
      << ", contract_violation_sites="
      << frontend_metadata_.protocol_qualified_object_type_lowering_contract_violation_sites
      << ", deterministic_protocol_qualified_object_type_lowering_handoff="
      << (frontend_metadata_.deterministic_protocol_qualified_object_type_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_variance_bridge_cast_lowering_profile = variance_bridge_cast_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_sites
      << ", protocol_composition_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_protocol_composition_sites
      << ", ownership_qualifier_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_ownership_qualifier_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.variance_bridge_cast_lowering_contract_violation_sites
      << ", deterministic_variance_bridge_cast_lowering_handoff="
      << (frontend_metadata_.deterministic_variance_bridge_cast_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_generic_metadata_abi_lowering_profile = generic_metadata_abi_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_sites
      << ", generic_suffix_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_generic_suffix_sites
      << ", protocol_composition_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_protocol_composition_sites
      << ", ownership_qualifier_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_ownership_qualifier_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.generic_metadata_abi_lowering_contract_violation_sites
      << ", deterministic_generic_metadata_abi_lowering_handoff="
      << (frontend_metadata_.deterministic_generic_metadata_abi_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_module_import_graph_lowering_profile = module_import_graph_sites="
      << frontend_metadata_.module_import_graph_lowering_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_.module_import_graph_lowering_import_edge_candidate_sites
      << ", namespace_segment_sites="
      << frontend_metadata_.module_import_graph_lowering_namespace_segment_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.module_import_graph_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.module_import_graph_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.module_import_graph_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.module_import_graph_lowering_contract_violation_sites
      << ", deterministic_module_import_graph_lowering_handoff="
      << (frontend_metadata_.deterministic_module_import_graph_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_namespace_collision_shadowing_lowering_profile = namespace_collision_shadowing_sites="
      << frontend_metadata_.namespace_collision_shadowing_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .namespace_collision_shadowing_lowering_contract_violation_sites
      << ", deterministic_namespace_collision_shadowing_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_namespace_collision_shadowing_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_public_private_api_partition_lowering_profile = public_private_api_partition_sites="
      << frontend_metadata_.public_private_api_partition_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .public_private_api_partition_lowering_contract_violation_sites
      << ", deterministic_public_private_api_partition_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_public_private_api_partition_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_incremental_module_cache_invalidation_lowering_profile = incremental_module_cache_invalidation_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_normalized_sites
      << ", cache_invalidation_candidate_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .incremental_module_cache_invalidation_lowering_contract_violation_sites
      << ", deterministic_incremental_module_cache_invalidation_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_incremental_module_cache_invalidation_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_cross_module_conformance_lowering_profile = cross_module_conformance_sites="
      << frontend_metadata_.cross_module_conformance_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.cross_module_conformance_lowering_normalized_sites
      << ", cache_invalidation_candidate_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_cache_invalidation_candidate_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .cross_module_conformance_lowering_contract_violation_sites
      << ", deterministic_cross_module_conformance_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_cross_module_conformance_lowering_handoff
              ? "true"
              : "false")
      << "\n";
}
