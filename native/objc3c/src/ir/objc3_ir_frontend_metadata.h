#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_bundles.h"
#include "ir/objc3_ir_frontend_metadata_pipeline_readiness.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"
#include "ir/objc3_ir_frontend_metadata_runtime_metadata.h"
#include "ir/objc3_ir_frontend_metadata_runtime_support.h"
// Historical extraction contract marker:
// #include "parse/objc3_parser_contract.h"

struct Objc3Program;

struct Objc3IRFrontendMetadata : Objc3IRFrontendRuntimeSupportMetadata,
                                 Objc3IRFrontendPipelineReadinessMetadata,
                                 Objc3IRFrontendRuntimeMetadata {
  std::uint8_t language_version = 3u;
  std::string language_profile = "canonical";
  std::string arc_mode = "disabled";
  bool arc_mode_enabled = false;
  bool versioned_conformance_report_lowering_ready = false;
  std::string versioned_conformance_report_lowering_replay_key;
  std::size_t canonical_literal_yes_rejection_sites = 0;
  std::size_t canonical_literal_no_rejection_sites = 0;
  std::size_t canonical_literal_null_rejection_sites = 0;
  std::size_t declared_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_interface_symbols = 0;
  std::size_t resolved_implementation_symbols = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic_interface_implementation_handoff = false;
  std::size_t declared_protocols = 0;
  std::size_t declared_categories = 0;
  std::size_t resolved_protocol_symbols = 0;
  std::size_t resolved_category_symbols = 0;
  std::size_t protocol_method_symbols = 0;
  std::size_t category_method_symbols = 0;
  std::size_t linked_category_symbols = 0;
  bool deterministic_protocol_category_handoff = false;
  std::size_t declared_class_interfaces = 0;
  std::size_t declared_class_implementations = 0;
  std::size_t resolved_class_interfaces = 0;
  std::size_t resolved_class_implementations = 0;
  std::size_t linked_class_method_symbols = 0;
  std::size_t linked_category_method_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic_class_protocol_category_linking_handoff = false;
  std::size_t selector_method_declaration_entries = 0;
  std::size_t selector_normalized_method_declarations = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_piece_parameter_links = 0;
  bool deterministic_selector_normalization_handoff = false;
  std::size_t property_declaration_entries = 0;
  std::size_t property_attribute_entries = 0;
  std::size_t property_attribute_value_entries = 0;
  std::size_t property_accessor_modifier_entries = 0;
  std::size_t property_getter_selector_entries = 0;
  std::size_t property_setter_selector_entries = 0;
  bool deterministic_property_attribute_handoff = false;
  std::string lowering_property_synthesis_ivar_binding_replay_key;
  std::size_t lowering_property_synthesis_sites = 0;
  std::size_t lowering_property_synthesis_explicit_ivar_bindings = 0;
  std::size_t lowering_property_synthesis_default_ivar_bindings = 0;
  std::size_t lowering_interface_owned_property_synthesis_sites = 0;
  std::size_t lowering_implementation_property_redeclaration_sites = 0;
  std::size_t lowering_property_synthesis_ivar_binding_resolved = 0;
  bool lowering_property_synthesis_deterministic_handoff = false;
  std::string lowering_id_class_sel_object_pointer_typecheck_replay_key;
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t id_class_sel_object_pointer_typecheck_sites_total = 0;
  bool deterministic_id_class_sel_object_pointer_typecheck_handoff = false;
  std::string lowering_dispatch_surface_classification_replay_key;
  std::size_t dispatch_surface_classification_instance_sites = 0;
  std::size_t dispatch_surface_classification_class_sites = 0;
  std::size_t dispatch_surface_classification_super_sites = 0;
  std::size_t dispatch_surface_classification_direct_sites = 0;
  std::size_t dispatch_surface_classification_dynamic_sites = 0;
  std::string dispatch_surface_classification_instance_entrypoint_family;
  std::string dispatch_surface_classification_class_entrypoint_family;
  std::string dispatch_surface_classification_super_entrypoint_family;
  std::string dispatch_surface_classification_direct_entrypoint_family;
  std::string dispatch_surface_classification_dynamic_entrypoint_family;
  bool deterministic_dispatch_surface_classification_handoff = false;
  std::string lowering_message_send_selector_lowering_replay_key;
  std::size_t message_send_selector_lowering_sites = 0;
  std::size_t message_send_selector_lowering_unary_sites = 0;
  std::size_t message_send_selector_lowering_keyword_sites = 0;
  std::size_t message_send_selector_lowering_selector_piece_sites = 0;
  std::size_t message_send_selector_lowering_argument_expression_sites = 0;
  std::size_t message_send_selector_lowering_receiver_sites = 0;
  std::size_t message_send_selector_lowering_selector_literal_entries = 0;
  std::size_t message_send_selector_lowering_selector_literal_characters = 0;
  bool deterministic_message_send_selector_lowering_handoff = false;
  std::string lowering_dispatch_abi_marshalling_replay_key;
  std::size_t dispatch_abi_marshalling_message_send_sites = 0;
  std::size_t dispatch_abi_marshalling_receiver_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_selector_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_value_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_padding_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_argument_total_slots_marshaled = 0;
  std::size_t dispatch_abi_marshalling_total_marshaled_slots = 0;
  std::size_t dispatch_abi_marshalling_runtime_dispatch_arg_slots = 0;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  std::string lowering_nil_receiver_semantics_foldability_replay_key;
  std::size_t nil_receiver_semantics_foldability_message_send_sites = 0;
  std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_foldability_enabled_sites = 0;
  std::size_t nil_receiver_semantics_foldability_foldable_sites = 0;
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites = 0;
  std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites = 0;
  std::size_t nil_receiver_semantics_foldability_contract_violation_sites = 0;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  std::string lowering_super_dispatch_method_family_replay_key;
  std::size_t super_dispatch_method_family_message_send_sites = 0;
  std::size_t super_dispatch_method_family_receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_method_family_enabled_sites = 0;
  std::size_t super_dispatch_method_family_requires_class_context_sites = 0;
  std::size_t super_dispatch_method_family_init_sites = 0;
  std::size_t super_dispatch_method_family_copy_sites = 0;
  std::size_t super_dispatch_method_family_mutable_copy_sites = 0;
  std::size_t super_dispatch_method_family_new_sites = 0;
  std::size_t super_dispatch_method_family_none_sites = 0;
  std::size_t super_dispatch_method_family_returns_retained_result_sites = 0;
  std::size_t super_dispatch_method_family_returns_related_result_sites = 0;
  std::size_t super_dispatch_method_family_contract_violation_sites = 0;
  bool deterministic_super_dispatch_method_family_handoff = false;
  std::string lowering_runtime_link_host_link_replay_key;
  std::size_t runtime_link_host_link_message_send_sites = 0;
  std::size_t runtime_link_host_link_required_sites = 0;
  std::size_t runtime_link_host_link_elided_sites = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_link_host_link_runtime_dispatch_declaration_parameter_count = 0;
  std::size_t runtime_link_host_link_contract_violation_sites = 0;
  std::string runtime_link_host_link_runtime_dispatch_symbol;
  bool runtime_link_host_link_default_runtime_dispatch_symbol_binding = true;
  bool deterministic_runtime_link_host_link_handoff = false;
  std::string lowering_ownership_qualifier_replay_key;
  std::size_t ownership_qualifier_lowering_ownership_qualifier_sites = 0;
  std::size_t ownership_qualifier_lowering_invalid_ownership_qualifier_sites = 0;
  std::size_t ownership_qualifier_lowering_object_pointer_type_annotation_sites = 0;
  bool deterministic_ownership_qualifier_lowering_handoff = false;
  std::string lowering_retain_release_operation_replay_key;
  std::size_t retain_release_operation_lowering_ownership_qualified_sites = 0;
  std::size_t retain_release_operation_lowering_retain_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_release_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_autorelease_insertion_sites = 0;
  std::size_t retain_release_operation_lowering_contract_violation_sites = 0;
  bool deterministic_retain_release_operation_lowering_handoff = false;
  std::string lowering_autoreleasepool_scope_replay_key;
  std::size_t autoreleasepool_scope_lowering_scope_sites = 0;
  std::size_t autoreleasepool_scope_lowering_scope_symbolized_sites = 0;
  unsigned autoreleasepool_scope_lowering_max_scope_depth = 0;
  std::size_t autoreleasepool_scope_lowering_scope_entry_transition_sites = 0;
  std::size_t autoreleasepool_scope_lowering_scope_exit_transition_sites = 0;
  std::size_t autoreleasepool_scope_lowering_contract_violation_sites = 0;
  bool deterministic_autoreleasepool_scope_lowering_handoff = false;
  std::string lowering_weak_unowned_semantics_replay_key;
  std::size_t weak_unowned_semantics_lowering_ownership_candidate_sites = 0;
  std::size_t weak_unowned_semantics_lowering_weak_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_unowned_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_semantics_lowering_conflict_sites = 0;
  std::size_t weak_unowned_semantics_lowering_contract_violation_sites = 0;
  bool deterministic_weak_unowned_semantics_lowering_handoff = false;
  std::string lowering_arc_diagnostics_fixit_replay_key;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t arc_diagnostics_fixit_lowering_contract_violation_sites = 0;
  bool deterministic_arc_diagnostics_fixit_lowering_handoff = false;
  std::string lowering_block_source_model_completion_replay_key;
  std::size_t block_source_model_completion_block_literal_sites = 0;
  std::size_t block_source_model_completion_signature_entries_total = 0;
  std::size_t block_source_model_completion_explicit_typed_parameter_entries_total = 0;
  std::size_t block_source_model_completion_implicit_parameter_entries_total = 0;
  std::size_t block_source_model_completion_capture_inventory_entries_total = 0;
  std::size_t block_source_model_completion_byvalue_readonly_capture_entries_total = 0;
  std::size_t block_source_model_completion_invoke_surface_entries_total = 0;
  std::size_t block_source_model_completion_non_normalized_sites = 0;
  std::size_t block_source_model_completion_contract_violation_sites = 0;
  bool deterministic_block_source_model_completion_handoff = false;
  std::string lowering_block_source_storage_annotation_replay_key;
  std::size_t block_source_storage_annotation_block_literal_sites = 0;
  std::size_t block_source_storage_annotation_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_mutated_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_byref_capture_entries_total = 0;
  std::size_t block_source_storage_annotation_copy_helper_intent_sites = 0;
  std::size_t block_source_storage_annotation_dispose_helper_intent_sites = 0;
  std::size_t block_source_storage_annotation_heap_candidate_sites = 0;
  std::size_t block_source_storage_annotation_expression_sites = 0;
  std::size_t block_source_storage_annotation_global_initializer_sites = 0;
  std::size_t block_source_storage_annotation_binding_initializer_sites = 0;
  std::size_t block_source_storage_annotation_assignment_value_sites = 0;
  std::size_t block_source_storage_annotation_return_value_sites = 0;
  std::size_t block_source_storage_annotation_call_argument_sites = 0;
  std::size_t block_source_storage_annotation_message_argument_sites = 0;
  std::size_t block_source_storage_annotation_non_normalized_sites = 0;
  std::size_t block_source_storage_annotation_contract_violation_sites = 0;
  bool deterministic_block_source_storage_annotation_handoff = false;
  std::string lowering_block_literal_capture_replay_key;
  std::size_t block_literal_capture_lowering_block_literal_sites = 0;
  std::size_t block_literal_capture_lowering_block_parameter_entries = 0;
  std::size_t block_literal_capture_lowering_block_capture_entries = 0;
  std::size_t block_literal_capture_lowering_block_body_statement_entries = 0;
  std::size_t block_literal_capture_lowering_block_empty_capture_sites = 0;
  std::size_t block_literal_capture_lowering_block_nondeterministic_capture_sites = 0;
  std::size_t block_literal_capture_lowering_block_non_normalized_sites = 0;
  std::size_t block_literal_capture_lowering_contract_violation_sites = 0;
  bool deterministic_block_literal_capture_lowering_handoff = false;
  std::string lowering_block_abi_invoke_trampoline_replay_key;
  std::size_t block_abi_invoke_trampoline_lowering_block_literal_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_invoke_argument_slots_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_capture_word_count_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_parameter_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_capture_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_body_statement_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_invoke_symbolized_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_missing_invoke_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_non_normalized_layout_sites = 0;
  std::size_t block_abi_invoke_trampoline_lowering_contract_violation_sites = 0;
  bool deterministic_block_abi_invoke_trampoline_lowering_handoff = false;
  std::string lowering_block_storage_escape_replay_key;
  std::size_t block_storage_escape_lowering_block_literal_sites = 0;
  std::size_t block_storage_escape_lowering_mutable_capture_count_total = 0;
  std::size_t block_storage_escape_lowering_byref_slot_count_total = 0;
  std::size_t block_storage_escape_lowering_parameter_entries_total = 0;
  std::size_t block_storage_escape_lowering_capture_entries_total = 0;
  std::size_t block_storage_escape_lowering_body_statement_entries_total = 0;
  std::size_t block_storage_escape_lowering_requires_byref_cells_sites = 0;
  std::size_t block_storage_escape_lowering_escape_analysis_enabled_sites = 0;
  std::size_t block_storage_escape_lowering_escape_to_heap_sites = 0;
  std::size_t block_storage_escape_lowering_escape_profile_normalized_sites = 0;
  std::size_t block_storage_escape_lowering_byref_layout_symbolized_sites = 0;
  std::size_t block_storage_escape_lowering_contract_violation_sites = 0;
  bool deterministic_block_storage_escape_lowering_handoff = false;
  std::string lowering_block_copy_dispose_replay_key;
  std::size_t block_copy_dispose_lowering_block_literal_sites = 0;
  std::size_t block_copy_dispose_lowering_mutable_capture_count_total = 0;
  std::size_t block_copy_dispose_lowering_byref_slot_count_total = 0;
  std::size_t block_copy_dispose_lowering_parameter_entries_total = 0;
  std::size_t block_copy_dispose_lowering_capture_entries_total = 0;
  std::size_t block_copy_dispose_lowering_body_statement_entries_total = 0;
  std::size_t block_copy_dispose_lowering_copy_helper_required_sites = 0;
  std::size_t block_copy_dispose_lowering_dispose_helper_required_sites = 0;
  std::size_t block_copy_dispose_lowering_profile_normalized_sites = 0;
  std::size_t block_copy_dispose_lowering_copy_helper_symbolized_sites = 0;
  std::size_t block_copy_dispose_lowering_dispose_helper_symbolized_sites = 0;
  std::size_t block_copy_dispose_lowering_contract_violation_sites = 0;
  bool deterministic_block_copy_dispose_lowering_handoff = false;
  std::string lowering_block_determinism_perf_baseline_replay_key;
  std::size_t block_determinism_perf_baseline_lowering_block_literal_sites = 0;
  std::size_t block_determinism_perf_baseline_lowering_baseline_weight_total = 0;
  std::size_t block_determinism_perf_baseline_lowering_parameter_entries_total = 0;
  std::size_t block_determinism_perf_baseline_lowering_capture_entries_total = 0;
  std::size_t block_determinism_perf_baseline_lowering_body_statement_entries_total = 0;
  std::size_t block_determinism_perf_baseline_lowering_deterministic_capture_sites = 0;
  std::size_t block_determinism_perf_baseline_lowering_heavy_tier_sites = 0;
  std::size_t block_determinism_perf_baseline_lowering_normalized_profile_sites = 0;
  std::size_t block_determinism_perf_baseline_lowering_contract_violation_sites = 0;
  bool deterministic_block_determinism_perf_baseline_lowering_handoff = false;
  std::string lowering_lightweight_generic_constraint_replay_key;
  std::size_t lightweight_generic_constraint_lowering_generic_constraint_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_generic_suffix_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_object_pointer_type_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_terminated_generic_suffix_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_pointer_declarator_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_normalized_constraint_sites = 0;
  std::size_t lightweight_generic_constraint_lowering_contract_violation_sites = 0;
  bool deterministic_lightweight_generic_constraint_lowering_handoff = false;
  std::string lowering_nullability_flow_warning_precision_replay_key;
  std::size_t nullability_flow_warning_precision_lowering_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_object_pointer_type_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_nullability_suffix_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_nullable_suffix_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_nonnull_suffix_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_normalized_sites = 0;
  std::size_t nullability_flow_warning_precision_lowering_contract_violation_sites = 0;
  bool deterministic_nullability_flow_warning_precision_lowering_handoff = false;
  std::string lowering_protocol_qualified_object_type_replay_key;
  std::size_t protocol_qualified_object_type_lowering_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_object_pointer_type_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_terminated_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_pointer_declarator_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_normalized_protocol_composition_sites = 0;
  std::size_t protocol_qualified_object_type_lowering_contract_violation_sites = 0;
  bool deterministic_protocol_qualified_object_type_lowering_handoff = false;
  std::string lowering_variance_bridge_cast_replay_key;
  std::size_t variance_bridge_cast_lowering_sites = 0;
  std::size_t variance_bridge_cast_lowering_protocol_composition_sites = 0;
  std::size_t variance_bridge_cast_lowering_ownership_qualifier_sites = 0;
  std::size_t variance_bridge_cast_lowering_object_pointer_type_sites = 0;
  std::size_t variance_bridge_cast_lowering_pointer_declarator_sites = 0;
  std::size_t variance_bridge_cast_lowering_normalized_sites = 0;
  std::size_t variance_bridge_cast_lowering_contract_violation_sites = 0;
  bool deterministic_variance_bridge_cast_lowering_handoff = false;
  std::string lowering_generic_metadata_abi_replay_key;
  std::size_t generic_metadata_abi_lowering_sites = 0;
  std::size_t generic_metadata_abi_lowering_generic_suffix_sites = 0;
  std::size_t generic_metadata_abi_lowering_protocol_composition_sites = 0;
  std::size_t generic_metadata_abi_lowering_ownership_qualifier_sites = 0;
  std::size_t generic_metadata_abi_lowering_object_pointer_type_sites = 0;
  std::size_t generic_metadata_abi_lowering_pointer_declarator_sites = 0;
  std::size_t generic_metadata_abi_lowering_normalized_sites = 0;
  std::size_t generic_metadata_abi_lowering_contract_violation_sites = 0;
  bool deterministic_generic_metadata_abi_lowering_handoff = false;
  std::string lowering_module_import_graph_replay_key;
  std::size_t module_import_graph_lowering_sites = 0;
  std::size_t module_import_graph_lowering_import_edge_candidate_sites = 0;
  std::size_t module_import_graph_lowering_namespace_segment_sites = 0;
  std::size_t module_import_graph_lowering_object_pointer_type_sites = 0;
  std::size_t module_import_graph_lowering_pointer_declarator_sites = 0;
  std::size_t module_import_graph_lowering_normalized_sites = 0;
  std::size_t module_import_graph_lowering_contract_violation_sites = 0;
  bool deterministic_module_import_graph_lowering_handoff = false;
  std::string lowering_namespace_collision_shadowing_replay_key;
  std::size_t namespace_collision_shadowing_lowering_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_namespace_segment_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_import_edge_candidate_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_object_pointer_type_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_pointer_declarator_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_normalized_sites = 0;
  std::size_t namespace_collision_shadowing_lowering_contract_violation_sites = 0;
  bool deterministic_namespace_collision_shadowing_lowering_handoff = false;
  std::string lowering_public_private_api_partition_replay_key;
  std::size_t public_private_api_partition_lowering_sites = 0;
  std::size_t public_private_api_partition_lowering_namespace_segment_sites = 0;
  std::size_t public_private_api_partition_lowering_import_edge_candidate_sites = 0;
  std::size_t public_private_api_partition_lowering_object_pointer_type_sites = 0;
  std::size_t public_private_api_partition_lowering_pointer_declarator_sites = 0;
  std::size_t public_private_api_partition_lowering_normalized_sites = 0;
  std::size_t public_private_api_partition_lowering_contract_violation_sites = 0;
  bool deterministic_public_private_api_partition_lowering_handoff = false;
  std::string lowering_incremental_module_cache_invalidation_replay_key;
  std::size_t incremental_module_cache_invalidation_lowering_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_namespace_segment_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_import_edge_candidate_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_object_pointer_type_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_pointer_declarator_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_normalized_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t incremental_module_cache_invalidation_lowering_contract_violation_sites = 0;
  bool deterministic_incremental_module_cache_invalidation_lowering_handoff = false;
  std::string lowering_cross_module_conformance_replay_key;
  std::size_t cross_module_conformance_lowering_sites = 0;
  std::size_t cross_module_conformance_lowering_namespace_segment_sites = 0;
  std::size_t cross_module_conformance_lowering_import_edge_candidate_sites = 0;
  std::size_t cross_module_conformance_lowering_object_pointer_type_sites = 0;
  std::size_t cross_module_conformance_lowering_pointer_declarator_sites = 0;
  std::size_t cross_module_conformance_lowering_normalized_sites = 0;
  std::size_t cross_module_conformance_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t cross_module_conformance_lowering_contract_violation_sites = 0;
  bool deterministic_cross_module_conformance_lowering_handoff = false;
  std::string lowering_error_handling_throws_abi_propagation_replay_key;
  std::string lowering_throws_propagation_replay_key;
  std::string lowering_result_like_replay_key;
  bool deterministic_result_like_lowering_handoff = false;
  std::size_t throws_propagation_lowering_sites = 0;
  std::size_t throws_propagation_lowering_namespace_segment_sites = 0;
  std::size_t throws_propagation_lowering_import_edge_candidate_sites = 0;
  std::size_t throws_propagation_lowering_object_pointer_type_sites = 0;
  std::size_t throws_propagation_lowering_pointer_declarator_sites = 0;
  std::size_t throws_propagation_lowering_normalized_sites = 0;
  std::size_t throws_propagation_lowering_cache_invalidation_candidate_sites = 0;
  std::size_t throws_propagation_lowering_contract_violation_sites = 0;
  bool deterministic_throws_propagation_lowering_handoff = false;
  std::string lowering_ns_error_bridging_replay_key;
  std::size_t ns_error_bridging_lowering_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_parameter_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridging_lowering_ns_error_bridge_path_sites = 0;
  std::size_t ns_error_bridging_lowering_failable_call_sites = 0;
  std::size_t ns_error_bridging_lowering_normalized_sites = 0;
  std::size_t ns_error_bridging_lowering_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_lowering_contract_violation_sites = 0;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::string lowering_unwind_cleanup_replay_key;
  std::size_t unwind_cleanup_lowering_sites = 0;
  std::size_t unwind_cleanup_lowering_unwind_edge_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_scope_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_emit_sites = 0;
  std::size_t unwind_cleanup_lowering_landing_pad_sites = 0;
  std::size_t unwind_cleanup_lowering_cleanup_resume_sites = 0;
  std::size_t unwind_cleanup_lowering_normalized_sites = 0;
  std::size_t unwind_cleanup_lowering_guard_blocked_sites = 0;
  std::size_t unwind_cleanup_lowering_contract_violation_sites = 0;
  bool deterministic_unwind_cleanup_lowering_handoff = false;
  std::string lowering_error_handling_result_and_bridging_artifact_replay_key;
  std::size_t imported_error_handling_result_and_bridging_artifact_modules = 0;
  bool error_handling_result_and_bridging_binary_artifact_replay_ready = false;
  bool error_handling_result_and_bridging_runtime_import_artifact_ready = false;
  bool error_handling_result_and_bridging_separate_compilation_replay_ready = false;
  bool deterministic_error_handling_result_and_bridging_artifact_replay_handoff = false;
  std::string lowering_error_diagnostics_recovery_replay_key;
  std::size_t error_diagnostics_recovery_lowering_sites = 0;
  std::size_t
      error_diagnostics_recovery_lowering_parser_diagnostic_sites = 0;
  std::size_t
      error_diagnostics_recovery_lowering_semantic_diagnostic_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_fixit_hint_sites = 0;
  std::size_t
      error_diagnostics_recovery_lowering_recovery_candidate_sites = 0;
  std::size_t
      error_diagnostics_recovery_lowering_recovery_applied_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_normalized_sites = 0;
  std::size_t error_diagnostics_recovery_lowering_guard_blocked_sites = 0;
  std::size_t
      error_diagnostics_recovery_lowering_contract_violation_sites = 0;
  bool deterministic_error_diagnostics_recovery_lowering_handoff = false;
  std::string lowering_async_continuation_replay_key;
  std::size_t async_continuation_lowering_sites = 0;
  std::size_t async_continuation_lowering_async_keyword_sites = 0;
  std::size_t async_continuation_lowering_async_function_sites = 0;
  std::size_t async_continuation_lowering_continuation_allocation_sites = 0;
  std::size_t async_continuation_lowering_continuation_resume_sites = 0;
  std::size_t async_continuation_lowering_continuation_suspend_sites = 0;
  std::size_t async_continuation_lowering_async_state_machine_sites = 0;
  std::size_t async_continuation_lowering_normalized_sites = 0;
  std::size_t async_continuation_lowering_gate_blocked_sites = 0;
  std::size_t async_continuation_lowering_contract_violation_sites = 0;
  bool deterministic_async_continuation_lowering_handoff = false;
  std::string lowering_await_lowering_suspension_state_replay_key;
  std::size_t await_lowering_suspension_state_lowering_sites = 0;
  std::size_t
      await_lowering_suspension_state_lowering_await_keyword_sites = 0;
  std::size_t
      await_lowering_suspension_state_lowering_await_suspension_point_sites =
          0;
  std::size_t await_lowering_suspension_state_lowering_await_resume_sites = 0;
  std::size_t
      await_lowering_suspension_state_lowering_await_state_machine_sites = 0;
  std::size_t
      await_lowering_suspension_state_lowering_await_continuation_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_normalized_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_gate_blocked_sites = 0;
  std::size_t
      await_lowering_suspension_state_lowering_contract_violation_sites = 0;
  bool deterministic_await_lowering_suspension_state_lowering_handoff = false;
  std::string lowering_actor_isolation_sendability_replay_key;
  std::size_t actor_isolation_sendability_lowering_sites = 0;
  std::size_t actor_isolation_sendability_lowering_sendability_check_sites = 0;
  std::size_t actor_isolation_sendability_lowering_cross_actor_hop_sites = 0;
  std::size_t actor_isolation_sendability_lowering_non_sendable_capture_sites =
      0;
  std::size_t actor_isolation_sendability_lowering_sendable_transfer_sites = 0;
  std::size_t actor_isolation_sendability_lowering_isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_lowering_guard_blocked_sites = 0;
  std::size_t actor_isolation_sendability_lowering_contract_violation_sites = 0;
  bool deterministic_actor_isolation_sendability_lowering_handoff = false;
  std::string lowering_actor_lowering_metadata_replay_key;
  std::size_t actor_lowering_metadata_actor_interface_sites = 0;
  std::size_t actor_lowering_metadata_actor_method_sites = 0;
  std::size_t actor_lowering_metadata_actor_metadata_record_sites = 0;
  std::size_t actor_lowering_metadata_nonisolated_entry_sites = 0;
  std::size_t actor_lowering_metadata_executor_affinity_sites = 0;
  std::size_t actor_lowering_metadata_actor_hop_artifact_sites = 0;
  std::size_t actor_lowering_metadata_actor_isolation_thunk_sites = 0;
  std::size_t actor_lowering_metadata_replay_proof_dependency_sites = 0;
  std::size_t actor_lowering_metadata_race_guard_dependency_sites = 0;
  std::size_t actor_lowering_metadata_task_handoff_sites = 0;
  std::size_t actor_lowering_metadata_guard_blocked_sites = 0;
  std::size_t actor_lowering_metadata_contract_violation_sites = 0;
  bool deterministic_actor_lowering_metadata_handoff = false;
  std::string lowering_dispatch_dispatch_control_replay_key;
  std::size_t dispatch_dispatch_control_lowering_direct_call_candidate_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      0;
  std::size_t dispatch_dispatch_control_lowering_dynamic_opt_out_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_final_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_sealed_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_override_legality_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      0;
  std::size_t
      dispatch_dispatch_control_lowering_metadata_preserved_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_guard_blocked_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_contract_violation_sites = 0;
  bool deterministic_dispatch_dispatch_control_lowering_handoff = false;
  std::string lowering_interop_interop_replay_key;
  std::size_t interop_interop_lowering_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_c_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_objc_runtime_parity_callable_sites = 0;
  std::size_t interop_interop_lowering_ownership_bridge_callable_sites = 0;
  std::size_t interop_interop_lowering_error_surface_sites = 0;
  std::size_t interop_interop_lowering_async_boundary_sites = 0;
  std::size_t interop_interop_lowering_swift_concurrency_metadata_sites = 0;
  std::size_t interop_interop_lowering_interface_preserved_foreign_callable_sites =
      0;
  std::size_t
      interop_interop_lowering_interface_preserved_metadata_annotation_sites = 0;
  std::size_t interop_interop_lowering_guard_blocked_sites = 0;
  std::size_t interop_interop_lowering_contract_violation_sites = 0;
  bool deterministic_interop_interop_lowering_handoff = false;
  std::string lowering_interop_foreign_call_lifetime_replay_key;
  std::size_t interop_foreign_call_lifetime_lowering_foreign_callable_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_c_foreign_callable_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites =
          0;
  std::size_t interop_foreign_call_lifetime_lowering_ownership_bridge_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_lifetime_bridge_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_metadata_preservation_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_guard_blocked_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_contract_violation_sites = 0;
  bool deterministic_interop_foreign_call_lifetime_lowering_handoff = false;
  std::string lowering_interop_ffi_metadata_interface_preservation_key;
  std::size_t interop_ffi_metadata_interface_preservation_local_foreign_callable_count =
      0;
  std::size_t
      interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_local_interface_annotation_sites =
          0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_module_count =
      0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_foreign_callable_count =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites =
          0;
  bool interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready =
      false;
  bool interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready =
      false;
  bool deterministic_interop_ffi_metadata_interface_preservation_handoff =
      false;
  std::string lowering_interop_header_module_bridge_generation_key;
  std::string lowering_metaprogramming_expansion_replay_key;
  std::size_t metaprogramming_expansion_lowering_derive_inventory_sites = 0;
  std::size_t metaprogramming_expansion_lowering_derived_selector_artifact_sites = 0;
  std::size_t metaprogramming_expansion_lowering_macro_replay_visible_sites = 0;
  std::size_t metaprogramming_expansion_lowering_property_behavior_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_binding_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_getter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_setter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_replay_visible_metadata_sites = 0;
  std::size_t metaprogramming_expansion_lowering_guard_blocked_sites = 0;
  std::size_t metaprogramming_expansion_lowering_contract_violation_sites = 0;
  bool deterministic_metaprogramming_expansion_lowering_handoff = false;
  std::string lowering_metaprogramming_synthesized_emission_replay_key;
  std::size_t metaprogramming_synthesized_emitted_derive_method_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_macro_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_property_behavior_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_global_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_runtime_method_list_sites = 0;
  std::size_t metaprogramming_synthesized_guard_blocked_sites = 0;
  std::size_t metaprogramming_synthesized_contract_violation_sites = 0;
  bool deterministic_metaprogramming_synthesized_emission_handoff = false;
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles_lexicographic;
  std::string lowering_metaprogramming_module_interface_replay_preservation_key;
  std::size_t metaprogramming_module_replay_local_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_local_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_interface_property_behavior_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_local_runtime_method_list_count = 0;
  std::size_t metaprogramming_module_replay_imported_module_count = 0;
  std::size_t metaprogramming_module_replay_imported_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_imported_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
          0;
  std::size_t
      metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_imported_runtime_method_list_count = 0;
  bool metaprogramming_module_replay_runtime_import_artifact_ready = false;
  bool metaprogramming_module_replay_separate_compilation_preservation_ready = false;
  bool deterministic_metaprogramming_module_interface_replay_handoff = false;
  std::string lowering_dispatch_dispatch_metadata_interface_preservation_key;
  std::size_t dispatch_dispatch_metadata_local_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_sealed_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_module_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_sealed_container_record_count = 0;
  bool dispatch_dispatch_metadata_runtime_import_artifact_ready = false;
  bool dispatch_dispatch_metadata_separate_compilation_preservation_ready = false;
  bool deterministic_dispatch_dispatch_metadata_interface_handoff = false;
  std::string lowering_ownership_system_extension_replay_key;
  std::size_t ownership_system_extension_lowering_cleanup_hook_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_local_sites = 0;
  std::size_t ownership_system_extension_lowering_cleanup_owned_local_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_move_capture_sites = 0;
  std::size_t ownership_system_extension_lowering_borrowed_parameter_sites = 0;
  std::size_t
      ownership_system_extension_lowering_borrowed_return_callable_sites = 0;
  std::size_t
      ownership_system_extension_lowering_borrowed_escape_candidate_sites = 0;
  std::size_t ownership_system_extension_lowering_explicit_capture_item_sites = 0;
  std::size_t ownership_system_extension_lowering_retainable_family_callable_sites =
      0;
  std::size_t
      ownership_system_extension_lowering_retainable_family_operation_callable_sites =
          0;
  std::size_t
      ownership_system_extension_lowering_retainable_family_alias_callable_sites =
          0;
  std::size_t ownership_system_extension_lowering_guard_blocked_sites = 0;
  std::size_t ownership_system_extension_lowering_contract_violation_sites = 0;
  bool deterministic_ownership_system_extension_lowering_handoff = false;
  std::string ownership_borrowed_retainable_abi_completion_replay_key;
  std::size_t ownership_borrowed_retainable_returns_borrowed_attribute_sites = 0;
  std::size_t ownership_borrowed_retainable_family_retain_sites = 0;
  std::size_t ownership_borrowed_retainable_family_release_sites = 0;
  std::size_t ownership_borrowed_retainable_family_autorelease_sites = 0;
  std::size_t
      ownership_borrowed_retainable_compatibility_returns_retained_sites = 0;
  std::size_t
      ownership_borrowed_retainable_compatibility_returns_not_retained_sites = 0;
  std::size_t ownership_borrowed_retainable_compatibility_consumed_sites = 0;
  bool deterministic_ownership_borrowed_retainable_abi_completion_handoff = false;
  std::string lowering_task_runtime_interop_cancellation_replay_key;
  std::size_t task_runtime_interop_cancellation_lowering_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_interop_sites =
      0;
  std::size_t
      task_runtime_interop_cancellation_lowering_cancellation_probe_sites = 0;
  std::size_t
      task_runtime_interop_cancellation_lowering_cancellation_handler_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_resume_sites =
      0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_cancel_sites =
      0;
  std::size_t task_runtime_interop_cancellation_lowering_normalized_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_guard_blocked_sites =
      0;
  std::size_t
      task_runtime_interop_cancellation_lowering_contract_violation_sites = 0;
  bool deterministic_task_runtime_interop_cancellation_lowering_handoff =
      false;
  std::string lowering_concurrency_replay_race_guard_replay_key;
  std::size_t concurrency_replay_race_guard_lowering_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_replay_proof_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_race_guard_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_task_handoff_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_actor_isolation_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_guard_blocked_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_contract_violation_sites = 0;
  bool deterministic_concurrency_replay_race_guard_lowering_handoff = false;
  std::string lowering_unsafe_pointer_extension_replay_key;
  std::size_t unsafe_pointer_extension_lowering_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_keyword_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_pointer_arithmetic_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_raw_pointer_type_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_operation_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_normalized_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_gate_blocked_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_lowering_handoff = false;
  std::string lowering_inline_asm_intrinsic_governance_replay_key;
  std::size_t inline_asm_intrinsic_governance_lowering_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_inline_asm_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites =
      0;
  std::size_t
      inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_normalized_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_gate_blocked_sites = 0;
  std::size_t
      inline_asm_intrinsic_governance_lowering_contract_violation_sites = 0;
  bool deterministic_inline_asm_intrinsic_governance_lowering_handoff = false;
  std::size_t object_pointer_type_spellings = 0;
  std::size_t pointer_declarator_entries = 0;
  std::size_t pointer_declarator_depth_total = 0;
  std::size_t pointer_declarator_token_entries = 0;
  std::size_t nullability_suffix_entries = 0;
  std::size_t generic_suffix_entries = 0;
  std::size_t terminated_generic_suffix_entries = 0;
  std::size_t unterminated_generic_suffix_entries = 0;
  bool deterministic_object_pointer_nullability_generics_handoff = false;
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic_symbol_graph_handoff = false;
  bool deterministic_scope_resolution_handoff = false;
  std::string deterministic_symbol_graph_scope_resolution_handoff_key;
  std::size_t canonical_literal_rejection_total() const {
    return canonical_literal_yes_rejection_sites +
           canonical_literal_no_rejection_sites +
           canonical_literal_null_rejection_sites;
  }
};

