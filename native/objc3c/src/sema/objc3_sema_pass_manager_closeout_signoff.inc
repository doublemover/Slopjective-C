      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .advanced_contract_rejection_shard2_ready &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .shard_surface_sync &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .passed_sync_count ==
          result.parity_surface.parser_sema_integration_closeout_signoff
              .required_sync_count &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .gate_signoff_surface_sync &&
      result.parity_surface.parser_sema_conformance_matrix
          .top_level_declaration_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .global_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .protocol_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .interface_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .implementation_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .function_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .protocol_property_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .protocol_method_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .interface_property_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .interface_method_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .implementation_property_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .implementation_method_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .interface_category_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .implementation_category_decl_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .function_prototype_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .function_pure_count_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .ast_shape_fingerprint_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .ast_top_level_layout_fingerprint_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_contract_snapshot_fingerprint_matches &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_diagnostic_budget_consistent &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_token_top_level_budget_consistent &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_subset_count_consistent &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_contract_snapshot_deterministic &&
      result.parity_surface.parser_sema_conformance_matrix
          .parser_recovery_replay_ready &&
      result.parity_surface.parser_sema_conformance_corpus.required_case_count ==
          5u &&
      result.parity_surface.parser_sema_conformance_corpus.passed_case_count ==
          result.parity_surface.parser_sema_conformance_corpus.required_case_count &&
      result.parity_surface.parser_sema_conformance_corpus.failed_case_count ==
          0u &&
      result.parity_surface.parser_sema_conformance_corpus
          .has_top_level_declaration_count_case &&
      result.parity_surface.parser_sema_conformance_corpus
          .has_snapshot_fingerprint_case &&
      result.parity_surface.parser_sema_conformance_corpus
          .has_diagnostic_budget_case &&
      result.parity_surface.parser_sema_conformance_corpus
          .has_subset_count_case &&
      result.parity_surface.parser_sema_conformance_corpus
          .has_recovery_replay_case &&
      result.parity_surface.parser_sema_conformance_corpus
          .top_level_declaration_count_case_passed &&
      result.parity_surface.parser_sema_conformance_corpus
          .snapshot_fingerprint_case_passed &&
      result.parity_surface.parser_sema_conformance_corpus
          .diagnostic_budget_case_passed &&
      result.parity_surface.parser_sema_conformance_corpus
          .subset_count_case_passed &&
      result.parity_surface.parser_sema_conformance_corpus
          .recovery_replay_case_passed &&
      result.parity_surface.diagnostics_accounting_consistent &&
      result.parity_surface.diagnostics_bus_publish_consistent &&
      result.parity_surface.diagnostics_canonicalized &&
      result.parity_surface.diagnostics_hardening_satisfied &&
      result.parity_surface.pass_flow_recovery_replay_contract_satisfied &&
      !result.parity_surface.pass_flow_recovery_replay_key.empty() &&
      result.parity_surface.pass_flow_recovery_replay_key_deterministic &&
      result.parity_surface.pass_flow_recovery_determinism_hardening_satisfied &&
      result.parity_surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.parity_surface.pass_manager_publication_record) &&
      result.parity_surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.parity_surface.type_metadata_publication_record) &&
      result.parity_surface.diagnostics_after_pass_monotonic &&
      result.parity_surface.deterministic_semantic_diagnostics &&
      result.parity_surface.deterministic_type_metadata_handoff &&
      result.parity_surface.deterministic_atomic_memory_order_mapping &&
      result.parity_surface.deterministic_vector_type_lowering &&
      result.parity_surface.atomic_memory_order_mapping.deterministic &&
      result.parity_surface.vector_type_lowering.deterministic &&
      result.parity_surface.globals_total == result.parity_surface.type_metadata_global_entries &&
      result.parity_surface.functions_total == result.parity_surface.type_metadata_function_entries &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.deterministic &&
      result.parity_surface.deterministic_interface_implementation_handoff &&
      result.parity_surface.protocol_category_composition_summary.deterministic &&
      result.parity_surface.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic &&
      result.parity_surface.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.selector_normalization_summary.deterministic &&
      result.parity_surface.deterministic_selector_normalization_handoff &&
      result.parity_surface.property_attribute_summary.deterministic &&
      result.parity_surface.deterministic_property_attribute_handoff &&
      result.parity_surface.type_annotation_surface_summary.deterministic &&
      result.parity_surface.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.lightweight_generic_constraint_summary.deterministic &&
      result.parity_surface.deterministic_lightweight_generic_constraint_handoff &&
      result.parity_surface.nullability_flow_warning_precision_summary.deterministic &&
      result.parity_surface.deterministic_nullability_flow_warning_precision_handoff &&
      result.parity_surface.protocol_qualified_object_type_summary.deterministic &&
      result.parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      result.parity_surface.variance_bridge_cast_summary.deterministic &&
      result.parity_surface.deterministic_variance_bridge_cast_handoff &&
      result.parity_surface.generic_metadata_abi_summary.deterministic &&
      result.parity_surface.deterministic_generic_metadata_abi_handoff &&
      result.parity_surface.module_import_graph_summary.deterministic &&
      result.parity_surface.deterministic_module_import_graph_handoff &&
      result.parity_surface.namespace_collision_shadowing_summary.deterministic &&
      result.parity_surface.deterministic_namespace_collision_shadowing_handoff &&
      result.parity_surface.public_private_api_partition_summary.deterministic &&
      result.parity_surface.deterministic_public_private_api_partition_handoff &&
      result.parity_surface.incremental_module_cache_invalidation_summary
          .deterministic &&
      result.parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      result.parity_surface.cross_module_conformance_summary.deterministic &&
      result.parity_surface.deterministic_cross_module_conformance_handoff &&
      result.parity_surface.throws_propagation_summary.deterministic &&
      result.parity_surface.deterministic_throws_propagation_handoff &&
      result.parity_surface.unwind_cleanup_summary.deterministic &&
      result.parity_surface.deterministic_unwind_cleanup_handoff &&
      result.parity_surface.async_continuation_summary.deterministic &&
      result.parity_surface.deterministic_async_continuation_handoff &&
      result.parity_surface.actor_isolation_sendability_summary.deterministic &&
      result.parity_surface
          .deterministic_actor_isolation_sendability_handoff &&
      result.parity_surface.task_runtime_cancellation_summary.deterministic &&
      result.parity_surface
          .deterministic_task_runtime_cancellation_handoff &&
      result.parity_surface.concurrency_replay_race_guard_summary.deterministic &&
      result.parity_surface
          .deterministic_concurrency_replay_race_guard_handoff &&
      result.parity_surface.unsafe_pointer_extension_summary.deterministic &&
      result.parity_surface.deterministic_unsafe_pointer_extension_handoff &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .deterministic &&
      result.parity_surface
          .deterministic_inline_asm_intrinsic_governance_handoff &&
      result.parity_surface.ns_error_bridging_summary.deterministic &&
      result.parity_surface.deterministic_ns_error_bridging_handoff &&
      result.parity_surface.error_diagnostics_recovery_summary.deterministic &&
      result.parity_surface.deterministic_error_diagnostics_recovery_handoff &&
      result.parity_surface.result_like_lowering_summary.deterministic &&
      result.parity_surface.deterministic_result_like_lowering_handoff &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .deterministic &&
      result.parity_surface
          .deterministic_await_lowering_suspension_state_lowering_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic &&
      result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.method_lookup_override_conflict_summary.deterministic &&
      result.parity_surface.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.block_literal_capture_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.block_storage_escape_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_storage_escape_handoff &&
      result.parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic &&
      result.parity_surface.deterministic_block_determinism_perf_baseline_handoff &&
      result.parity_surface.message_send_selector_lowering_summary.deterministic &&
      result.parity_surface.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.dispatch_abi_marshalling_summary.deterministic &&
      result.parity_surface.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.super_dispatch_method_family_summary.deterministic &&
      result.parity_surface.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.runtime_link_host_link_summary.deterministic &&
      result.parity_surface.deterministic_runtime_link_host_link_handoff &&
      result.parity_surface.retain_release_operation_summary.deterministic &&
      result.parity_surface.deterministic_retain_release_operation_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic &&
      result.parity_surface.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      result.parity_surface.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic &&
      result.parity_surface.deterministic_autoreleasepool_scope_handoff;
  const bool closeout_parser_sema_ready =
      result.parity_surface.deterministic_parser_sema_integration_closeout_signoff &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .deterministic &&
      result.parity_surface.parser_sema_integration_closeout_signoff
              .required_sync_count == 3u &&
      result.parity_surface.parser_sema_integration_closeout_signoff
              .passed_sync_count ==
          result.parity_surface.parser_sema_integration_closeout_signoff
              .required_sync_count &&
      result.parity_surface.parser_sema_integration_closeout_signoff
              .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .gate_signoff_surface_sync;
  const bool closeout_pass_manager_publication_ready =
      result.parity_surface.deterministic_pass_manager_publication_record &&
      IsReadyObjc3SemaPassManagerPublicationRecord(
          result.parity_surface.pass_manager_publication_record);
  const bool closeout_type_metadata_publication_ready =
      result.parity_surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          result.parity_surface.type_metadata_publication_record);
  const bool closeout_diagnostics_publication_ready =
      result.parity_surface.diagnostics_accounting_consistent &&
      result.parity_surface.diagnostics_bus_publish_consistent &&
      result.parity_surface.diagnostics_canonicalized &&
      result.parity_surface.diagnostics_hardening_satisfied &&
      result.parity_surface.diagnostics_after_pass_monotonic &&
      result.parity_surface.deterministic_semantic_diagnostics;
  const bool closeout_pass_flow_recovery_ready =
      result.parity_surface.pass_flow_recovery_replay_contract_satisfied &&
      !result.parity_surface.pass_flow_recovery_replay_key.empty() &&
      result.parity_surface.pass_flow_recovery_replay_key_deterministic &&
      result.parity_surface.pass_flow_recovery_determinism_hardening_satisfied;
  const bool closeout_type_metadata_handoff_ready =
      result.parity_surface.deterministic_type_metadata_handoff &&
      result.parity_surface.globals_total ==
          result.parity_surface.type_metadata_global_entries &&
      result.parity_surface.functions_total ==
          result.parity_surface.type_metadata_function_entries &&
      result.parity_surface.interfaces_total ==
          result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total ==
          result.parity_surface.type_metadata_implementation_entries;
  const bool closeout_typed_semantic_handoffs_ready =
      result.parity_surface.deterministic_interface_implementation_handoff &&
      result.parity_surface.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.deterministic_selector_normalization_handoff &&
      result.parity_surface.deterministic_property_attribute_handoff &&
      result.parity_surface.deterministic_type_annotation_surface_handoff &&
      result.parity_surface
          .deterministic_lightweight_generic_constraint_handoff &&
      result.parity_surface
          .deterministic_nullability_flow_warning_precision_handoff &&
      result.parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      result.parity_surface.deterministic_variance_bridge_cast_handoff &&
      result.parity_surface.deterministic_generic_metadata_abi_handoff &&
      result.parity_surface.deterministic_module_import_graph_handoff &&
      result.parity_surface.deterministic_namespace_collision_shadowing_handoff &&
      result.parity_surface.deterministic_public_private_api_partition_handoff &&
      result.parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      result.parity_surface.deterministic_cross_module_conformance_handoff &&
      result.parity_surface.deterministic_throws_propagation_handoff &&
      result.parity_surface.deterministic_unwind_cleanup_handoff &&
      result.parity_surface.deterministic_async_continuation_handoff &&
      result.parity_surface.deterministic_actor_isolation_sendability_handoff &&
      result.parity_surface.deterministic_task_runtime_cancellation_handoff &&
      result.parity_surface
          .deterministic_concurrency_replay_race_guard_handoff &&
      result.parity_surface.deterministic_unsafe_pointer_extension_handoff &&
      result.parity_surface
          .deterministic_inline_asm_intrinsic_governance_handoff &&
      result.parity_surface.deterministic_ns_error_bridging_handoff &&
      result.parity_surface.deterministic_error_diagnostics_recovery_handoff &&
      result.parity_surface.deterministic_result_like_lowering_handoff &&
      result.parity_surface
          .deterministic_await_lowering_suspension_state_lowering_handoff &&
      result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface
          .deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.deterministic_block_storage_escape_handoff &&
      result.parity_surface.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.deterministic_block_determinism_perf_baseline_handoff &&
      result.parity_surface.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface
          .deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.deterministic_runtime_link_host_link_handoff &&
      result.parity_surface.deterministic_retain_release_operation_handoff &&
      result.parity_surface.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.deterministic_autoreleasepool_scope_handoff &&
      result.parity_surface.deterministic_atomic_memory_order_mapping &&
      result.parity_surface.deterministic_vector_type_lowering;
  result.closeout_signoff_record = BuildObjc3SemaCloseoutSignoffRecord(
      input,
      result.parity_surface.ready,
      closeout_parser_sema_ready,
      closeout_pass_manager_publication_ready,
      closeout_type_metadata_publication_ready,
      closeout_diagnostics_publication_ready,
      closeout_pass_flow_recovery_ready,
      closeout_type_metadata_handoff_ready,
      closeout_typed_semantic_handoffs_ready);
  result.deterministic_closeout_signoff_record =
      IsReadyObjc3SemaCloseoutSignoffRecord(result.closeout_signoff_record);
  result.parity_surface.closeout_signoff_record = result.closeout_signoff_record;
  result.parity_surface.deterministic_closeout_signoff_record =
      result.deterministic_closeout_signoff_record;
  result.parity_surface.ready =
      result.parity_surface.ready &&
      result.parity_surface.deterministic_closeout_signoff_record &&
      IsReadyObjc3SemaCloseoutSignoffRecord(
          result.parity_surface.closeout_signoff_record);
  return result;
}
