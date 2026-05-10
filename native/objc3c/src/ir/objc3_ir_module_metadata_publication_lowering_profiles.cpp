#include "ir/objc3_ir_module_metadata_publication_lowering_profiles.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites="
      << frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites
      << ", invalid_ownership_qualifier_sites="
      << frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites
      << ", object_pointer_type_annotation_sites="
      << frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites
      << ", deterministic_ownership_qualifier_lowering_handoff="
      << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites="
      << frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites
      << ", retain_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites
      << ", release_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_release_insertion_sites
      << ", autorelease_insertion_sites="
      << frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites
      << ", contract_violation_sites="
      << frontend_metadata_.retain_release_operation_lowering_contract_violation_sites
      << ", deterministic_retain_release_operation_lowering_handoff="
      << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
      << ", scope_symbolized_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites
      << ", max_scope_depth="
      << frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth
      << ", scope_entry_transition_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites
      << ", scope_exit_transition_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites
      << ", contract_violation_sites="
      << frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites
      << ", deterministic_autoreleasepool_scope_lowering_handoff="
      << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites
      << ", weak_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites
      << ", unowned_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites
      << ", unowned_safe_reference_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites
      << ", weak_unowned_conflict_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites
      << ", contract_violation_sites="
      << frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites
      << ", deterministic_weak_unowned_semantics_lowering_handoff="
      << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites
      << ", ownership_arc_fixit_available_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites
      << ", ownership_arc_profiled_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites
      << ", ownership_arc_weak_unowned_conflict_diagnostic_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ", ownership_arc_empty_fixit_hint_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites
      << ", contract_violation_sites="
      << frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites
      << ", deterministic_arc_diagnostics_fixit_lowering_handoff="
      << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_literal_sites
      << ", block_parameter_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_parameter_entries
      << ", block_capture_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_capture_entries
      << ", block_body_statement_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries
      << ", block_empty_capture_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites
      << ", block_nondeterministic_capture_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites
      << ", block_non_normalized_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_literal_capture_lowering_contract_violation_sites
      << ", deterministic_block_literal_capture_lowering_handoff="
      << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites
      << ", invoke_argument_slots_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total
      << ", capture_word_count_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total
      << ", descriptor_symbolized_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites
      << ", invoke_trampoline_symbolized_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites
      << ", missing_invoke_trampoline_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites
      << ", non_normalized_layout_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites
      << ", deterministic_block_abi_invoke_trampoline_lowering_handoff="
      << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_storage_escape_lowering_block_literal_sites
      << ", mutable_capture_count_total="
      << frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total
      << ", byref_slot_count_total="
      << frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total
      << ", requires_byref_cells_sites="
      << frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites
      << ", escape_analysis_enabled_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites
      << ", escape_to_heap_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites
      << ", escape_profile_normalized_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites
      << ", byref_layout_symbolized_sites="
      << frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_storage_escape_lowering_contract_violation_sites
      << ", deterministic_block_storage_escape_lowering_handoff="
      << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_copy_dispose_lowering_block_literal_sites
      << ", mutable_capture_count_total="
      << frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total
      << ", byref_slot_count_total="
      << frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total
      << ", copy_helper_required_sites="
      << frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites
      << ", dispose_helper_required_sites="
      << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites
      << ", profile_normalized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites
      << ", copy_helper_symbolized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites
      << ", dispose_helper_symbolized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites
      << ", deterministic_block_copy_dispose_lowering_handoff="
      << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites
      << ", baseline_weight_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total
      << ", deterministic_capture_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites
      << ", heavy_tier_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites
      << ", normalized_profile_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites
      << ", deterministic_block_determinism_perf_baseline_lowering_handoff="
      << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? "true" : "false")
      << "\n";
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
  out << "; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites="
      << frontend_metadata_.throws_propagation_lowering_sites
      << ", namespace_segment_sites="
      << frontend_metadata_.throws_propagation_lowering_namespace_segment_sites
      << ", import_edge_candidate_sites="
      << frontend_metadata_
             .throws_propagation_lowering_import_edge_candidate_sites
      << ", object_pointer_type_sites="
      << frontend_metadata_.throws_propagation_lowering_object_pointer_type_sites
      << ", pointer_declarator_sites="
      << frontend_metadata_.throws_propagation_lowering_pointer_declarator_sites
      << ", normalized_sites="
      << frontend_metadata_.throws_propagation_lowering_normalized_sites
      << ", cache_invalidation_candidate_sites="
      << frontend_metadata_
             .throws_propagation_lowering_cache_invalidation_candidate_sites
      << ", contract_violation_sites="
      << frontend_metadata_.throws_propagation_lowering_contract_violation_sites
      << ", deterministic_throws_propagation_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_throws_propagation_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites="
      << frontend_metadata_.ns_error_bridging_lowering_sites
      << ", ns_error_parameter_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_parameter_sites
      << ", ns_error_out_parameter_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_out_parameter_sites
      << ", ns_error_bridge_path_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_ns_error_bridge_path_sites
      << ", failable_call_sites="
      << frontend_metadata_.ns_error_bridging_lowering_failable_call_sites
      << ", normalized_sites="
      << frontend_metadata_.ns_error_bridging_lowering_normalized_sites
      << ", bridge_boundary_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_bridge_boundary_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .ns_error_bridging_lowering_contract_violation_sites
      << ", deterministic_ns_error_bridging_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_ns_error_bridging_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites="
      << frontend_metadata_.unwind_cleanup_lowering_sites
      << ", unwind_edge_sites="
      << frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites
      << ", cleanup_scope_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites
      << ", cleanup_emit_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites
      << ", landing_pad_sites="
      << frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites
      << ", cleanup_resume_sites="
      << frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites
      << ", normalized_sites="
      << frontend_metadata_.unwind_cleanup_lowering_normalized_sites
      << ", guard_blocked_sites="
      << frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites
      << ", deterministic_unwind_cleanup_lowering_handoff="
      << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_error_diagnostics_recovery_lowering_profile = error_diagnostic_sites="
      << frontend_metadata_.error_diagnostics_recovery_lowering_sites
      << ", parser_diagnostic_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_parser_diagnostic_sites
      << ", semantic_diagnostic_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_semantic_diagnostic_sites
      << ", fixit_hint_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_fixit_hint_sites
      << ", recovery_candidate_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_recovery_candidate_sites
      << ", recovery_applied_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_recovery_applied_sites
      << ", normalized_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_normalized_sites
      << ", guard_blocked_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_guard_blocked_sites
      << ", contract_violation_sites="
      << frontend_metadata_
             .error_diagnostics_recovery_lowering_contract_violation_sites
      << ", deterministic_error_diagnostics_recovery_lowering_handoff="
      << (frontend_metadata_
                  .deterministic_error_diagnostics_recovery_lowering_handoff
              ? "true"
              : "false")
      << "\n";
}
