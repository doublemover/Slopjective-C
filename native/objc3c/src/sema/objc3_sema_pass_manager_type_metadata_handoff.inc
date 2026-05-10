#include "sema/objc3_sema_pass_manager_type_metadata_handoff_runtime_extension_gates.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_error_flow.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_concurrency.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_source_graph.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_blocks.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_message_dispatch.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_arc.inc"
#include "sema/objc3_sema_pass_manager_type_metadata_handoff_type_system.inc"
  result.type_metadata_publication_record =
      BuildObjc3SemaTypeMetadataPublicationRecord(
          input,
          result.integration_surface,
          result.type_metadata_handoff,
          result.deterministic_type_metadata_handoff,
          result.deterministic_interface_implementation_handoff,
          result.deterministic_protocol_category_composition_handoff,
          result.deterministic_class_protocol_category_linking_handoff,
          result.deterministic_selector_normalization_handoff);
  result.deterministic_type_metadata_publication_record =
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.type_metadata_publication_record);
  PublishObjc3SemaPassFlowSummary(
      handoff_publication,
      diagnostics_run,
      result);

  result.parity_surface.parser_sema_conformance_matrix =
      result.parser_sema_conformance_matrix;
  result.parity_surface.parser_sema_conformance_corpus =
      result.parser_sema_conformance_corpus;
  result.parity_surface.parser_sema_performance_quality_guardrails =
      result.parser_sema_performance_quality_guardrails;
  result.parity_surface.parser_sema_cross_lane_integration_sync =
      result.parser_sema_cross_lane_integration_sync;
  result.parity_surface.parser_sema_docs_runbook_sync =
      result.parser_sema_docs_runbook_sync;
  result.parity_surface.parser_sema_release_candidate_replay_dry_run =
      result.parser_sema_release_candidate_replay_dry_run;
  result.parity_surface.parser_sema_advanced_core_shard1 =
      result.parser_sema_advanced_core_shard1;
  result.parity_surface.parser_sema_advanced_contract_rejection_shard1 =
      result.parser_sema_advanced_contract_rejection_shard1;
  result.parity_surface.parser_sema_advanced_diagnostics_shard1 =
      result.parser_sema_advanced_diagnostics_shard1;
  result.parity_surface.parser_sema_advanced_conformance_shard1 =
      result.parser_sema_advanced_conformance_shard1;
  result.parity_surface.parser_sema_advanced_integration_shard1 =
      result.parser_sema_advanced_integration_shard1;
  result.parity_surface.parser_sema_advanced_performance_shard1 =
      result.parser_sema_advanced_performance_shard1;
  result.parity_surface.parser_sema_advanced_core_shard2 =
      result.parser_sema_advanced_core_shard2;
  result.parity_surface.parser_sema_advanced_contract_rejection_shard2 =
      result.parser_sema_advanced_contract_rejection_shard2;
  result.parity_surface.parser_sema_advanced_diagnostics_shard2 =
      result.parser_sema_advanced_diagnostics_shard2;
  result.parity_surface.parser_sema_integration_closeout_signoff =
      result.parser_sema_integration_closeout_signoff;
  result.parity_surface.sema_pass_flow_summary = result.sema_pass_flow_summary;
  result.parity_surface.diagnostics_publication_record =
      result.diagnostics_publication_record;
  result.parity_surface.pass_flow_recovery_record =
      result.pass_flow_recovery_record;
  result.parity_surface.pass_manager_publication_record =
      result.pass_manager_publication_record;
  result.parity_surface.type_metadata_publication_record =
      result.type_metadata_publication_record;
  result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;
  result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;
  result.parity_surface.diagnostics_total = result.diagnostics.size();
  result.parity_surface.globals_total = result.integration_surface.globals.size();
  result.parity_surface.functions_total = result.integration_surface.functions.size();
  result.parity_surface.interfaces_total = result.integration_surface.interfaces.size();
  result.parity_surface.implementations_total = result.integration_surface.implementations.size();
  result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();
  result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();
  result.parity_surface.type_metadata_interface_entries = result.type_metadata_handoff.interfaces_lexicographic.size();
  result.parity_surface.type_metadata_implementation_entries =
      result.type_metadata_handoff.implementations_lexicographic.size();
  result.parity_surface.interface_implementation_summary = result.type_metadata_handoff.interface_implementation_summary;
  result.parity_surface.bootstrap_legality_failure_contract_summary =
      result.integration_surface.bootstrap_legality_failure_contract_summary;
  result.parity_surface.bootstrap_legality_semantics_summary =
      result.integration_surface.bootstrap_legality_semantics_summary;
  result.parity_surface.bootstrap_failure_restart_semantics_summary =
      result.integration_surface.bootstrap_failure_restart_semantics_summary;
  result.parity_surface.compatibility_strictness_claim_semantics_summary =
      result.integration_surface.compatibility_strictness_claim_semantics_summary;
  result.parity_surface.interface_method_symbols_total =
      result.parity_surface.interface_implementation_summary.interface_method_symbols;
  result.parity_surface.implementation_method_symbols_total =
      result.parity_surface.interface_implementation_summary.implementation_method_symbols;
  result.parity_surface.linked_implementation_symbols_total =
      result.parity_surface.interface_implementation_summary.linked_implementation_symbols;
  result.parity_surface.protocol_category_composition_summary =
      result.type_metadata_handoff.protocol_category_composition_summary;
  result.parity_surface.protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites;
  result.parity_surface.protocol_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols;
  result.parity_surface.category_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_sites;
  result.parity_surface.category_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols;
  result.parity_surface.invalid_protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.parity_surface.class_protocol_category_linking_summary =
      result.type_metadata_handoff.class_protocol_category_linking_summary;
  result.parity_surface.selector_normalization_summary = result.type_metadata_handoff.selector_normalization_summary;
  result.parity_surface.selector_normalization_methods_total =
      result.parity_surface.selector_normalization_summary.methods_total;
  result.parity_surface.selector_normalization_normalized_methods_total =
      result.parity_surface.selector_normalization_summary.normalized_methods;
  result.parity_surface.selector_normalization_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_piece_entries;
  result.parity_surface.selector_normalization_parameter_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries;
  result.parity_surface.selector_normalization_pieceless_methods_total =
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods;
  result.parity_surface.selector_normalization_spelling_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches;
  result.parity_surface.selector_normalization_arity_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches;
  result.parity_surface.selector_normalization_parameter_linkage_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches;
  result.parity_surface.selector_normalization_flag_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches;
  result.parity_surface.selector_normalization_missing_keyword_pieces_total =
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces;
  result.parity_surface.property_attribute_summary = result.type_metadata_handoff.property_attribute_summary;
  result.parity_surface.property_attribute_properties_total =
      result.parity_surface.property_attribute_summary.properties_total;
  result.parity_surface.property_attribute_entries_total =
      result.parity_surface.property_attribute_summary.attribute_entries;
  result.parity_surface.property_attribute_readonly_modifiers_total =
      result.parity_surface.property_attribute_summary.readonly_modifiers;
  result.parity_surface.property_attribute_readwrite_modifiers_total =
      result.parity_surface.property_attribute_summary.readwrite_modifiers;
  result.parity_surface.property_attribute_atomic_modifiers_total =
      result.parity_surface.property_attribute_summary.atomic_modifiers;
  result.parity_surface.property_attribute_nonatomic_modifiers_total =
      result.parity_surface.property_attribute_summary.nonatomic_modifiers;
  result.parity_surface.property_attribute_copy_modifiers_total =
      result.parity_surface.property_attribute_summary.copy_modifiers;
  result.parity_surface.property_attribute_strong_modifiers_total =
      result.parity_surface.property_attribute_summary.strong_modifiers;
  result.parity_surface.property_attribute_weak_modifiers_total =
      result.parity_surface.property_attribute_summary.weak_modifiers;
  result.parity_surface.property_attribute_assign_modifiers_total =
      result.parity_surface.property_attribute_summary.assign_modifiers;
  result.parity_surface.property_attribute_getter_modifiers_total =
      result.parity_surface.property_attribute_summary.getter_modifiers;
  result.parity_surface.property_attribute_setter_modifiers_total =
      result.parity_surface.property_attribute_summary.setter_modifiers;
  result.parity_surface.property_attribute_invalid_attribute_entries_total =
      result.parity_surface.property_attribute_summary.invalid_attribute_entries;
  result.parity_surface.property_attribute_contract_violations_total =
      result.parity_surface.property_attribute_summary.property_contract_violations;
  result.parity_surface.type_annotation_surface_summary = result.type_metadata_handoff.type_annotation_surface_summary;
  result.parity_surface.type_annotation_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites;
  result.parity_surface.type_annotation_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites;
  result.parity_surface.type_annotation_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites;
  result.parity_surface.type_annotation_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites;
  result.parity_surface.type_annotation_object_pointer_type_sites_total =
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites;
  result.parity_surface.type_annotation_invalid_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites;
  result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
  result.parity_surface.lightweight_generic_constraint_summary =
      result.type_metadata_handoff.lightweight_generic_constraint_summary;
  result.parity_surface.lightweight_generic_constraint_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.generic_constraint_sites;
  result.parity_surface.lightweight_generic_constraint_generic_suffix_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.generic_suffix_sites;
  result.parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.object_pointer_type_sites;
  result.parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites;
  result.parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.pointer_declarator_sites;
  result.parity_surface.lightweight_generic_constraint_normalized_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.normalized_constraint_sites;
  result.parity_surface.lightweight_generic_constraint_contract_violation_sites_total =
      result.parity_surface.lightweight_generic_constraint_summary.contract_violation_sites;
  result.parity_surface.nullability_flow_warning_precision_summary =
      result.type_metadata_handoff.nullability_flow_warning_precision_summary;
  result.parity_surface.nullability_flow_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_flow_sites;
  result.parity_surface.nullability_flow_object_pointer_type_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.object_pointer_type_sites;
  result.parity_surface.nullability_flow_nullability_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_suffix_sites;
  result.parity_surface.nullability_flow_nullable_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nullable_suffix_sites;
  result.parity_surface.nullability_flow_nonnull_suffix_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites;
  result.parity_surface.nullability_flow_normalized_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.normalized_sites;
  result.parity_surface.nullability_flow_contract_violation_sites_total =
      result.parity_surface.nullability_flow_warning_precision_summary.contract_violation_sites;
  result.parity_surface.protocol_qualified_object_type_summary =
      result.type_metadata_handoff.protocol_qualified_object_type_summary;
  result.parity_surface.protocol_qualified_object_type_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites;
  result.parity_surface.protocol_qualified_object_type_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.object_pointer_type_sites;
  result.parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.pointer_declarator_sites;
  result.parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites;
  result.parity_surface.protocol_qualified_object_type_contract_violation_sites_total =
      result.parity_surface.protocol_qualified_object_type_summary.contract_violation_sites;
  result.parity_surface.variance_bridge_cast_summary = result.type_metadata_handoff.variance_bridge_cast_summary;
  result.parity_surface.variance_bridge_cast_sites_total =
      result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites;
  result.parity_surface.variance_bridge_cast_protocol_composition_sites_total =
      result.parity_surface.variance_bridge_cast_summary.protocol_composition_sites;
  result.parity_surface.variance_bridge_cast_ownership_qualifier_sites_total =
      result.parity_surface.variance_bridge_cast_summary.ownership_qualifier_sites;
  result.parity_surface.variance_bridge_cast_object_pointer_type_sites_total =
      result.parity_surface.variance_bridge_cast_summary.object_pointer_type_sites;
  result.parity_surface.variance_bridge_cast_pointer_declarator_sites_total =
      result.parity_surface.variance_bridge_cast_summary.pointer_declarator_sites;
  result.parity_surface.variance_bridge_cast_normalized_sites_total =
      result.parity_surface.variance_bridge_cast_summary.normalized_sites;
  result.parity_surface.variance_bridge_cast_contract_violation_sites_total =
      result.parity_surface.variance_bridge_cast_summary.contract_violation_sites;
  result.parity_surface.generic_metadata_abi_summary = result.type_metadata_handoff.generic_metadata_abi_summary;
  result.parity_surface.generic_metadata_abi_sites_total =
      result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites;
  result.parity_surface.generic_metadata_abi_generic_suffix_sites_total =
      result.parity_surface.generic_metadata_abi_summary.generic_suffix_sites;
  result.parity_surface.generic_metadata_abi_protocol_composition_sites_total =
      result.parity_surface.generic_metadata_abi_summary.protocol_composition_sites;
  result.parity_surface.generic_metadata_abi_ownership_qualifier_sites_total =
      result.parity_surface.generic_metadata_abi_summary.ownership_qualifier_sites;
  result.parity_surface.generic_metadata_abi_object_pointer_type_sites_total =
      result.parity_surface.generic_metadata_abi_summary.object_pointer_type_sites;
  result.parity_surface.generic_metadata_abi_pointer_declarator_sites_total =
      result.parity_surface.generic_metadata_abi_summary.pointer_declarator_sites;
  result.parity_surface.generic_metadata_abi_normalized_sites_total =
      result.parity_surface.generic_metadata_abi_summary.normalized_sites;
  result.parity_surface.generic_metadata_abi_contract_violation_sites_total =
      result.parity_surface.generic_metadata_abi_summary.contract_violation_sites;
  result.parity_surface.module_import_graph_summary =
      result.type_metadata_handoff.module_import_graph_summary;
  result.parity_surface.module_import_graph_sites_total =
      result.parity_surface.module_import_graph_summary.module_import_graph_sites;
  result.parity_surface.module_import_graph_import_edge_candidate_sites_total =
      result.parity_surface.module_import_graph_summary.import_edge_candidate_sites;
  result.parity_surface.module_import_graph_namespace_segment_sites_total =
      result.parity_surface.module_import_graph_summary.namespace_segment_sites;
  result.parity_surface.module_import_graph_object_pointer_type_sites_total =
      result.parity_surface.module_import_graph_summary.object_pointer_type_sites;
  result.parity_surface.module_import_graph_pointer_declarator_sites_total =
      result.parity_surface.module_import_graph_summary.pointer_declarator_sites;
  result.parity_surface.module_import_graph_normalized_sites_total =
      result.parity_surface.module_import_graph_summary.normalized_sites;
  result.parity_surface.module_import_graph_contract_violation_sites_total =
      result.parity_surface.module_import_graph_summary.contract_violation_sites;
  result.parity_surface.namespace_collision_shadowing_summary =
      result.type_metadata_handoff.namespace_collision_shadowing_summary;
  result.parity_surface.namespace_collision_shadowing_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .namespace_collision_shadowing_sites;
  result.parity_surface.namespace_collision_shadowing_namespace_segment_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary.namespace_segment_sites;
  result.parity_surface
      .namespace_collision_shadowing_import_edge_candidate_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .import_edge_candidate_sites;
  result.parity_surface.namespace_collision_shadowing_object_pointer_type_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .object_pointer_type_sites;
  result.parity_surface.namespace_collision_shadowing_pointer_declarator_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .pointer_declarator_sites;
  result.parity_surface.namespace_collision_shadowing_normalized_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary.normalized_sites;
  result.parity_surface
      .namespace_collision_shadowing_contract_violation_sites_total =
      result.parity_surface.namespace_collision_shadowing_summary
          .contract_violation_sites;
  result.parity_surface.public_private_api_partition_summary =
      result.type_metadata_handoff.public_private_api_partition_summary;
  result.parity_surface.public_private_api_partition_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .public_private_api_partition_sites;
  result.parity_surface.public_private_api_partition_namespace_segment_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .namespace_segment_sites;
  result.parity_surface
      .public_private_api_partition_import_edge_candidate_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .import_edge_candidate_sites;
  result.parity_surface.public_private_api_partition_object_pointer_type_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .object_pointer_type_sites;
  result.parity_surface.public_private_api_partition_pointer_declarator_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .pointer_declarator_sites;
  result.parity_surface.public_private_api_partition_normalized_sites_total =
      result.parity_surface.public_private_api_partition_summary.normalized_sites;
  result.parity_surface.public_private_api_partition_contract_violation_sites_total =
      result.parity_surface.public_private_api_partition_summary
          .contract_violation_sites;
  result.parity_surface.incremental_module_cache_invalidation_summary =
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary;
  result.parity_surface.incremental_module_cache_invalidation_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .incremental_module_cache_invalidation_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_namespace_segment_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .namespace_segment_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_import_edge_candidate_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .import_edge_candidate_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_object_pointer_type_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .object_pointer_type_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_pointer_declarator_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .pointer_declarator_sites;
  result.parity_surface.incremental_module_cache_invalidation_normalized_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .normalized_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface
      .incremental_module_cache_invalidation_contract_violation_sites_total =
      result.parity_surface.incremental_module_cache_invalidation_summary
          .contract_violation_sites;
  result.parity_surface.cross_module_conformance_summary =
      result.type_metadata_handoff.cross_module_conformance_summary;
  result.parity_surface.cross_module_conformance_sites_total =
      result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites;
  result.parity_surface.cross_module_conformance_namespace_segment_sites_total =
      result.parity_surface.cross_module_conformance_summary.namespace_segment_sites;
  result.parity_surface.cross_module_conformance_import_edge_candidate_sites_total =
      result.parity_surface.cross_module_conformance_summary.import_edge_candidate_sites;
  result.parity_surface.cross_module_conformance_object_pointer_type_sites_total =
      result.parity_surface.cross_module_conformance_summary.object_pointer_type_sites;
  result.parity_surface.cross_module_conformance_pointer_declarator_sites_total =
      result.parity_surface.cross_module_conformance_summary.pointer_declarator_sites;
  result.parity_surface.cross_module_conformance_normalized_sites_total =
      result.parity_surface.cross_module_conformance_summary.normalized_sites;
  result.parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total =
      result.parity_surface.cross_module_conformance_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface.cross_module_conformance_contract_violation_sites_total =
      result.parity_surface.cross_module_conformance_summary.contract_violation_sites;
  result.parity_surface.throws_propagation_summary =
      result.type_metadata_handoff.throws_propagation_summary;
  result.parity_surface.throws_propagation_sites_total =
      result.parity_surface.throws_propagation_summary.throws_propagation_sites;
  result.parity_surface.throws_propagation_namespace_segment_sites_total =
      result.parity_surface.throws_propagation_summary.namespace_segment_sites;
  result.parity_surface.throws_propagation_import_edge_candidate_sites_total =
      result.parity_surface.throws_propagation_summary.import_edge_candidate_sites;
  result.parity_surface.throws_propagation_object_pointer_type_sites_total =
      result.parity_surface.throws_propagation_summary.object_pointer_type_sites;
  result.parity_surface.throws_propagation_pointer_declarator_sites_total =
      result.parity_surface.throws_propagation_summary.pointer_declarator_sites;
  result.parity_surface.throws_propagation_normalized_sites_total =
      result.parity_surface.throws_propagation_summary.normalized_sites;
  result.parity_surface.throws_propagation_cache_invalidation_candidate_sites_total =
      result.parity_surface.throws_propagation_summary
          .cache_invalidation_candidate_sites;
  result.parity_surface.throws_propagation_contract_violation_sites_total =
      result.parity_surface.throws_propagation_summary.contract_violation_sites;
  result.parity_surface.async_continuation_summary =
      result.type_metadata_handoff.async_continuation_summary;
  result.parity_surface.async_continuation_sites_total =
      result.parity_surface.async_continuation_summary.async_continuation_sites;
  result.parity_surface.async_continuation_async_keyword_sites_total =
      result.parity_surface.async_continuation_summary.async_keyword_sites;
  result.parity_surface.async_continuation_async_function_sites_total =
      result.parity_surface.async_continuation_summary.async_function_sites;
  result.parity_surface.async_continuation_allocation_sites_total =
      result.parity_surface.async_continuation_summary
          .continuation_allocation_sites;
  result.parity_surface.async_continuation_resume_sites_total =
      result.parity_surface.async_continuation_summary.continuation_resume_sites;
  result.parity_surface.async_continuation_suspend_sites_total =
      result.parity_surface.async_continuation_summary
          .continuation_suspend_sites;
  result.parity_surface.async_continuation_state_machine_sites_total =
      result.parity_surface.async_continuation_summary.async_state_machine_sites;
  result.parity_surface.async_continuation_normalized_sites_total =
      result.parity_surface.async_continuation_summary.normalized_sites;
  result.parity_surface.async_continuation_gate_blocked_sites_total =
      result.parity_surface.async_continuation_summary.gate_blocked_sites;
  result.parity_surface.async_continuation_contract_violation_sites_total =
      result.parity_surface.async_continuation_summary.contract_violation_sites;
  result.parity_surface.actor_isolation_sendability_summary =
      result.type_metadata_handoff.actor_isolation_sendability_summary;
  result.parity_surface.actor_isolation_sendability_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .actor_isolation_sendability_sites;
  result.parity_surface.actor_isolation_decl_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .actor_isolation_decl_sites;
  result.parity_surface.actor_hop_sites_total =
      result.parity_surface.actor_isolation_sendability_summary.actor_hop_sites;
  result.parity_surface.sendable_annotation_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .sendable_annotation_sites;
  result.parity_surface.non_sendable_crossing_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .non_sendable_crossing_sites;
  result.parity_surface.actor_isolation_sendability_isolation_boundary_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .isolation_boundary_sites;
  result.parity_surface.actor_isolation_sendability_normalized_sites_total =
      result.parity_surface.actor_isolation_sendability_summary.normalized_sites;
  result.parity_surface.actor_isolation_sendability_gate_blocked_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .gate_blocked_sites;
  result.parity_surface.actor_isolation_sendability_contract_violation_sites_total =
      result.parity_surface.actor_isolation_sendability_summary
          .contract_violation_sites;
  result.parity_surface.task_runtime_cancellation_summary =
      result.type_metadata_handoff.task_runtime_cancellation_summary;
  result.parity_surface.task_runtime_cancellation_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .task_runtime_interop_sites;
  result.parity_surface.task_runtime_cancellation_runtime_hook_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
