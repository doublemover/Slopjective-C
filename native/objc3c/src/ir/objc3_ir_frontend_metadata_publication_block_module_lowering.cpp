#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!19 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_parameter_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_capture_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_body_statement_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_empty_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_non_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_literal_capture_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!20 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!21 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_storage_escape_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_mutable_capture_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_byref_slot_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_requires_byref_cells_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_analysis_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_to_heap_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_profile_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_byref_layout_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_storage_escape_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!22 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_copy_dispose_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_mutable_capture_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_byref_slot_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_copy_helper_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_dispose_helper_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_profile_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_copy_dispose_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!23 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_baseline_weight_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_determinism_perf_baseline_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRTypeModuleLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!24 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_generic_constraint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_lightweight_generic_constraint_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!25 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_nullability_flow_warning_precision_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!26 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_protocol_qualified_object_type_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!27 = !{i64 "
      << static_cast<unsigned long long>(metadata.variance_bridge_cast_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.variance_bridge_cast_lowering_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.variance_bridge_cast_lowering_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.variance_bridge_cast_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.variance_bridge_cast_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.variance_bridge_cast_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.variance_bridge_cast_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_variance_bridge_cast_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!28 = !{i64 "
      << static_cast<unsigned long long>(metadata.generic_metadata_abi_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.generic_metadata_abi_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_generic_metadata_abi_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!29 = !{i64 "
      << static_cast<unsigned long long>(metadata.module_import_graph_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.module_import_graph_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_module_import_graph_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!30 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_namespace_collision_shadowing_lowering_handoff ? 1 : 0)
      << "}\n\n";
}

void EmitObjc3IRModuleGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!31 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_public_private_api_partition_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!32 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_incremental_module_cache_invalidation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!33 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_cross_module_conformance_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
