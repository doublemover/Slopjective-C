  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_final_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_final_container_record_count;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_imported_sealed_container_record_count =
      dispatch_dispatch_metadata_interface_preservation_summary
          .imported_sealed_container_record_count;
  ir_frontend_metadata.dispatch_dispatch_metadata_runtime_import_artifact_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .dispatch_dispatch_metadata_separate_compilation_preservation_ready =
      dispatch_dispatch_metadata_interface_preservation_summary
          .separate_compilation_preservation_ready;
  ir_frontend_metadata.deterministic_dispatch_dispatch_metadata_interface_handoff =
      dispatch_dispatch_metadata_interface_preservation_summary.deterministic;
  ir_frontend_metadata.lowering_ownership_system_extension_replay_key =
      ownership_system_extension_lowering_replay_key;
  ir_frontend_metadata.ownership_system_extension_lowering_cleanup_hook_sites =
      ownership_system_extension_lowering_contract.cleanup_hook_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_resource_local_sites =
      ownership_system_extension_lowering_contract.resource_local_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_cleanup_owned_local_sites =
      ownership_system_extension_lowering_contract.cleanup_owned_local_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_resource_move_capture_sites =
      ownership_system_extension_lowering_contract.resource_move_capture_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_parameter_sites =
      ownership_system_extension_lowering_contract.borrowed_parameter_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_return_callable_sites =
      ownership_system_extension_lowering_contract.borrowed_return_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_borrowed_escape_candidate_sites =
      ownership_system_extension_lowering_contract.borrowed_escape_candidate_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_explicit_capture_item_sites =
      ownership_system_extension_lowering_contract.explicit_capture_item_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_callable_sites =
      ownership_system_extension_lowering_contract.retainable_family_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_operation_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_operation_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_retainable_family_alias_callable_sites =
      ownership_system_extension_lowering_contract
          .retainable_family_alias_callable_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_guard_blocked_sites =
      ownership_system_extension_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.ownership_system_extension_lowering_contract_violation_sites =
      ownership_system_extension_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ownership_system_extension_lowering_handoff =
      ownership_system_extension_lowering_contract.deterministic;
  ir_frontend_metadata.ownership_borrowed_retainable_abi_completion_replay_key =
      ownership_borrowed_retainable_abi_completion_replay_key;
  ir_frontend_metadata.ownership_borrowed_retainable_returns_borrowed_attribute_sites =
      ownership_system_extension_source_closure_summary
          .returns_borrowed_attribute_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_retain_sites =
      ownership_retainable_c_family_source_completion_summary.family_retain_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_release_sites =
      ownership_retainable_c_family_source_completion_summary.family_release_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_family_autorelease_sites =
      ownership_retainable_c_family_source_completion_summary
          .family_autorelease_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_retained_sites;
  ir_frontend_metadata
      .ownership_borrowed_retainable_compatibility_returns_not_retained_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_returns_not_retained_sites;
  ir_frontend_metadata.ownership_borrowed_retainable_compatibility_consumed_sites =
      ownership_retainable_c_family_source_completion_summary
          .compatibility_consumed_sites;
  ir_frontend_metadata
      .deterministic_ownership_borrowed_retainable_abi_completion_handoff =
      true;
  ir_frontend_metadata.lowering_task_runtime_interop_cancellation_replay_key =
      concurrency_task_runtime_interop_cancellation_lowering_replay_key;
  ir_frontend_metadata.task_runtime_interop_cancellation_lowering_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_interop_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_interop_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_probe_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_probe_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_handler_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_handler_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_resume_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_resume_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_cancel_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_cancel_sites;
  ir_frontend_metadata.task_runtime_interop_cancellation_lowering_normalized_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .normalized_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_guard_blocked_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .guard_blocked_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_contract_violation_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_task_runtime_interop_cancellation_lowering_handoff =
      concurrency_task_runtime_interop_cancellation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_concurrency_replay_race_guard_replay_key =
      concurrency_concurrency_replay_race_guard_lowering_replay_key;
  // implementation anchor: the runnable actor pipeline now carries
  // the strict-concurrency replay/race-guard lowering packet beside the actor
  // lowering contract so helper-backed actor runtime rewrites can prove both
  // artifact families together.
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .concurrency_replay_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_replay_proof_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .replay_proof_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_race_guard_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.race_guard_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_task_handoff_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.task_handoff_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_actor_isolation_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .actor_isolation_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_deterministic_schedule_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .deterministic_schedule_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_guard_blocked_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_contract_violation_sites =
      concurrency_concurrency_replay_race_guard_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata.deterministic_concurrency_replay_race_guard_lowering_handoff =
      concurrency_concurrency_replay_race_guard_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_ownership_qualifier_replay_key =
      ownership_qualifier_lowering_replay_key;
  ir_frontend_metadata.ownership_qualifier_lowering_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites =
      ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites;
  ir_frontend_metadata.lowering_retain_release_operation_replay_key =
      retain_release_operation_lowering_replay_key;
  ir_frontend_metadata.retain_release_operation_lowering_ownership_qualified_sites =
      retain_release_operation_lowering_contract.ownership_qualified_sites;
  ir_frontend_metadata.retain_release_operation_lowering_retain_insertion_sites =
      retain_release_operation_lowering_contract.retain_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_release_insertion_sites =
      retain_release_operation_lowering_contract.release_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_autorelease_insertion_sites =
      retain_release_operation_lowering_contract.autorelease_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_contract_violation_sites =
      retain_release_operation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =
      autoreleasepool_scope_lowering_replay_key;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_sites =
      autoreleasepool_scope_lowering_contract.scope_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_symbolized_sites =
      autoreleasepool_scope_lowering_contract.scope_symbolized_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_max_scope_depth =
      autoreleasepool_scope_lowering_contract.max_scope_depth;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_entry_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_exit_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_contract_violation_sites =
      autoreleasepool_scope_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =
      weak_unowned_semantics_lowering_replay_key;
  ir_frontend_metadata.weak_unowned_semantics_lowering_ownership_candidate_sites =
      weak_unowned_semantics_lowering_contract.ownership_candidate_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_weak_reference_sites =
      weak_unowned_semantics_lowering_contract.weak_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_conflict_sites =
      weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_contract_violation_sites =
      weak_unowned_semantics_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_weak_unowned_semantics_lowering_handoff =
      weak_unowned_semantics_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =
      arc_diagnostics_fixit_lowering_replay_key;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_contract_violation_sites =
      arc_diagnostics_fixit_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_arc_diagnostics_fixit_lowering_handoff =
      arc_diagnostics_fixit_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_literal_capture_replay_key =
      block_literal_capture_lowering_replay_key;
  ir_frontend_metadata.block_literal_capture_lowering_block_literal_sites =
      block_literal_capture_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_parameter_entries =
      block_literal_capture_lowering_contract.block_parameter_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_capture_entries =
      block_literal_capture_lowering_contract.block_capture_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_body_statement_entries =
      block_literal_capture_lowering_contract.block_body_statement_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_empty_capture_sites =
      block_literal_capture_lowering_contract.block_empty_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites =
      block_literal_capture_lowering_contract.block_nondeterministic_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_non_normalized_sites =
      block_literal_capture_lowering_contract.block_non_normalized_sites;
  ir_frontend_metadata.block_literal_capture_lowering_contract_violation_sites =
      block_literal_capture_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_literal_capture_lowering_handoff =
      block_literal_capture_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_source_model_completion_replay_key =
      block_source_model_completion_replay_key;
  ir_frontend_metadata.block_source_model_completion_block_literal_sites =
      block_source_model_completion_contract.block_literal_sites;
  ir_frontend_metadata.block_source_model_completion_signature_entries_total =
      block_source_model_completion_contract.signature_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_explicit_typed_parameter_entries_total =
      block_source_model_completion_contract
          .explicit_typed_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_implicit_parameter_entries_total =
      block_source_model_completion_contract
          .implicit_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_capture_inventory_entries_total =
      block_source_model_completion_contract.capture_inventory_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_byvalue_readonly_capture_entries_total =
      block_source_model_completion_contract
          .byvalue_readonly_capture_entries_total;
  ir_frontend_metadata.block_source_model_completion_invoke_surface_entries_total =
      block_source_model_completion_contract.invoke_surface_entries_total;
  ir_frontend_metadata.block_source_model_completion_non_normalized_sites =
      block_source_model_completion_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_model_completion_contract_violation_sites =
      block_source_model_completion_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_model_completion_handoff =
      block_source_model_completion_contract.deterministic;
  ir_frontend_metadata.lowering_block_source_storage_annotation_replay_key =
      block_source_storage_annotation_replay_key;
  ir_frontend_metadata.block_source_storage_annotation_block_literal_sites =
      block_source_storage_annotation_contract.block_literal_sites;
  ir_frontend_metadata.block_source_storage_annotation_capture_entries_total =
      block_source_storage_annotation_contract.capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_mutated_capture_entries_total =
      block_source_storage_annotation_contract
          .mutated_capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_byref_capture_entries_total =
      block_source_storage_annotation_contract.byref_capture_entries_total;
  ir_frontend_metadata.block_source_storage_annotation_copy_helper_intent_sites =
      block_source_storage_annotation_contract.copy_helper_intent_sites;
  ir_frontend_metadata
      .block_source_storage_annotation_dispose_helper_intent_sites =
      block_source_storage_annotation_contract
          .dispose_helper_intent_sites;
  ir_frontend_metadata.block_source_storage_annotation_heap_candidate_sites =
      block_source_storage_annotation_contract.heap_candidate_sites;
  ir_frontend_metadata.block_source_storage_annotation_expression_sites =
      block_source_storage_annotation_contract.expression_sites;
  ir_frontend_metadata.block_source_storage_annotation_global_initializer_sites =
      block_source_storage_annotation_contract.global_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_binding_initializer_sites =
      block_source_storage_annotation_contract.binding_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_assignment_value_sites =
      block_source_storage_annotation_contract.assignment_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_return_value_sites =
      block_source_storage_annotation_contract.return_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_call_argument_sites =
      block_source_storage_annotation_contract.call_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_message_argument_sites =
      block_source_storage_annotation_contract.message_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_non_normalized_sites =
      block_source_storage_annotation_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_storage_annotation_contract_violation_sites =
      block_source_storage_annotation_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_storage_annotation_handoff =
      block_source_storage_annotation_contract.deterministic;
  ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =
      block_abi_invoke_trampoline_lowering_replay_key;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total =
      block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total =
      block_abi_invoke_trampoline_lowering_contract.capture_word_count_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total =
      block_abi_invoke_trampoline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_entries_total =
      block_abi_invoke_trampoline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total =
      block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites =
      block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites =
      block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites =
      block_abi_invoke_trampoline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff =
      block_abi_invoke_trampoline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_storage_escape_replay_key =
      block_storage_escape_lowering_replay_key;
  ir_frontend_metadata.block_storage_escape_lowering_block_literal_sites =
      block_storage_escape_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_storage_escape_lowering_mutable_capture_count_total =
      block_storage_escape_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_byref_slot_count_total =
      block_storage_escape_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_parameter_entries_total =
      block_storage_escape_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_capture_entries_total =
      block_storage_escape_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_body_statement_entries_total =
      block_storage_escape_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_requires_byref_cells_sites =
      block_storage_escape_lowering_contract.requires_byref_cells_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_analysis_enabled_sites =
      block_storage_escape_lowering_contract.escape_analysis_enabled_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_profile_normalized_sites =
      block_storage_escape_lowering_contract.escape_profile_normalized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_contract_violation_sites =
      block_storage_escape_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_storage_escape_lowering_handoff =
      block_storage_escape_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_copy_dispose_replay_key =
      block_copy_dispose_lowering_replay_key;
  ir_frontend_metadata.block_copy_dispose_lowering_block_literal_sites =
      block_copy_dispose_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_mutable_capture_count_total =
      block_copy_dispose_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_byref_slot_count_total =
      block_copy_dispose_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_parameter_entries_total =
      block_copy_dispose_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_capture_entries_total =
      block_copy_dispose_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_body_statement_entries_total =
      block_copy_dispose_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_profile_normalized_sites =
      block_copy_dispose_lowering_contract.profile_normalized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_contract_violation_sites =
      block_copy_dispose_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_copy_dispose_lowering_handoff =
      block_copy_dispose_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =
      block_determinism_perf_baseline_lowering_replay_key;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_block_literal_sites =
      block_determinism_perf_baseline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_baseline_weight_total =
      block_determinism_perf_baseline_lowering_contract.baseline_weight_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_parameter_entries_total =
      block_determinism_perf_baseline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_capture_entries_total =
      block_determinism_perf_baseline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total =
      block_determinism_perf_baseline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites =
      block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites =
      block_determinism_perf_baseline_lowering_contract.heavy_tier_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites =
      block_determinism_perf_baseline_lowering_contract.normalized_profile_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_contract_violation_sites =
      block_determinism_perf_baseline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_determinism_perf_baseline_lowering_handoff =
      block_determinism_perf_baseline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_lightweight_generic_constraint_replay_key =
      lightweight_generic_constraint_lowering_replay_key;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_constraint_sites =
      lightweight_generic_constraint_lowering_contract.generic_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites =
      lightweight_generic_constraint_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites =
      lightweight_generic_constraint_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites =
      lightweight_generic_constraint_lowering_contract.normalized_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_contract_violation_sites =
      lightweight_generic_constraint_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_lightweight_generic_constraint_lowering_handoff =
      lightweight_generic_constraint_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_nullability_flow_warning_precision_replay_key =
      nullability_flow_warning_precision_lowering_replay_key;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_flow_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites =
      nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_normalized_sites =
      nullability_flow_warning_precision_lowering_contract.normalized_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_contract_violation_sites =
      nullability_flow_warning_precision_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_nullability_flow_warning_precision_lowering_handoff =
      nullability_flow_warning_precision_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_protocol_qualified_object_type_replay_key =
      protocol_qualified_object_type_lowering_replay_key;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_sites =
      protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites =
      protocol_qualified_object_type_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites =
      protocol_qualified_object_type_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_contract_violation_sites =
      protocol_qualified_object_type_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_protocol_qualified_object_type_lowering_handoff =
      protocol_qualified_object_type_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_variance_bridge_cast_replay_key =
      variance_bridge_cast_lowering_replay_key;
  ir_frontend_metadata.variance_bridge_cast_lowering_sites =
      variance_bridge_cast_lowering_contract.variance_bridge_cast_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_protocol_composition_sites =
      variance_bridge_cast_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_ownership_qualifier_sites =
      variance_bridge_cast_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_object_pointer_type_sites =
      variance_bridge_cast_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_pointer_declarator_sites =
      variance_bridge_cast_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_normalized_sites =
      variance_bridge_cast_lowering_contract.normalized_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_contract_violation_sites =
      variance_bridge_cast_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_variance_bridge_cast_lowering_handoff =
      variance_bridge_cast_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_generic_metadata_abi_replay_key =
      generic_metadata_abi_lowering_replay_key;
  ir_frontend_metadata.generic_metadata_abi_lowering_sites =
      generic_metadata_abi_lowering_contract.generic_metadata_abi_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_generic_suffix_sites =
      generic_metadata_abi_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_protocol_composition_sites =
      generic_metadata_abi_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_ownership_qualifier_sites =
      generic_metadata_abi_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_object_pointer_type_sites =
      generic_metadata_abi_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_pointer_declarator_sites =
      generic_metadata_abi_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_normalized_sites =
      generic_metadata_abi_lowering_contract.normalized_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_contract_violation_sites =
      generic_metadata_abi_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_generic_metadata_abi_lowering_handoff =
      generic_metadata_abi_lowering_contract.deterministic;
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
  ir_frontend_metadata.lowering_throws_propagation_replay_key =
      throws_propagation_lowering_replay_key;
  ir_frontend_metadata.lowering_error_handling_throws_abi_propagation_replay_key =
      error_handling_throws_abi_propagation_lowering_replay_key;
  ir_frontend_metadata.lowering_result_like_replay_key =
      result_like_lowering_replay_key;
  ir_frontend_metadata.deterministic_result_like_lowering_handoff =
      result_like_lowering_contract.deterministic;
  ir_frontend_metadata.throws_propagation_lowering_sites =
      throws_propagation_lowering_contract.throws_propagation_sites;
  ir_frontend_metadata.throws_propagation_lowering_namespace_segment_sites =
      throws_propagation_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.throws_propagation_lowering_import_edge_candidate_sites =
      throws_propagation_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_object_pointer_type_sites =
      throws_propagation_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.throws_propagation_lowering_pointer_declarator_sites =
      throws_propagation_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.throws_propagation_lowering_normalized_sites =
      throws_propagation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .throws_propagation_lowering_cache_invalidation_candidate_sites =
      throws_propagation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_contract_violation_sites =
      throws_propagation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_throws_propagation_lowering_handoff =
      throws_propagation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_ns_error_bridging_replay_key =
      ns_error_bridging_lowering_replay_key;
  ir_frontend_metadata.ns_error_bridging_lowering_sites =
      ns_error_bridging_lowering_contract.ns_error_bridging_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites =
      ns_error_bridging_lowering_contract.ns_error_out_parameter_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites =
      ns_error_bridging_lowering_contract.ns_error_bridge_path_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_failable_call_sites =
      ns_error_bridging_lowering_contract.failable_call_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_normalized_sites =
      ns_error_bridging_lowering_contract.normalized_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_bridge_boundary_sites =
      ns_error_bridging_lowering_contract.bridge_boundary_sites;
  ir_frontend_metadata.ns_error_bridging_lowering_contract_violation_sites =
      ns_error_bridging_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_ns_error_bridging_lowering_handoff =
      ns_error_bridging_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_unwind_cleanup_replay_key =
      unwind_cleanup_lowering_replay_key;
  ir_frontend_metadata.unwind_cleanup_lowering_sites =
      unwind_cleanup_lowering_contract.unwind_cleanup_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_unwind_edge_sites =
      unwind_cleanup_lowering_contract.unwind_edge_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_scope_sites =
      unwind_cleanup_lowering_contract.cleanup_scope_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_emit_sites =
      unwind_cleanup_lowering_contract.cleanup_emit_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_landing_pad_sites =
      unwind_cleanup_lowering_contract.landing_pad_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_cleanup_resume_sites =
      unwind_cleanup_lowering_contract.cleanup_resume_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_normalized_sites =
      unwind_cleanup_lowering_contract.normalized_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_guard_blocked_sites =
      unwind_cleanup_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata.unwind_cleanup_lowering_contract_violation_sites =
      unwind_cleanup_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_unwind_cleanup_lowering_handoff =
      unwind_cleanup_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_error_handling_result_and_bridging_artifact_replay_key =
      error_handling_result_and_bridging_artifact_replay_summary.replay_key;
  ir_frontend_metadata.imported_error_handling_result_and_bridging_artifact_modules =
      error_handling_result_and_bridging_artifact_replay_summary
          .imported_module_names_lexicographic.size();
  ir_frontend_metadata.error_handling_result_and_bridging_binary_artifact_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .binary_artifact_replay_ready;
  ir_frontend_metadata.error_handling_result_and_bridging_runtime_import_artifact_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .runtime_import_artifact_ready;
  ir_frontend_metadata
      .error_handling_result_and_bridging_separate_compilation_replay_ready =
      error_handling_result_and_bridging_artifact_replay_summary
          .separate_compilation_replay_ready;
  ir_frontend_metadata
      .deterministic_error_handling_result_and_bridging_artifact_replay_handoff =
      error_handling_result_and_bridging_artifact_replay_summary.deterministic;
  ir_frontend_metadata.object_pointer_type_spellings =
      object_pointer_nullability_generics_summary.object_pointer_type_spellings;
  ir_frontend_metadata.pointer_declarator_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_entries;
  ir_frontend_metadata.pointer_declarator_depth_total =
      object_pointer_nullability_generics_summary.pointer_declarator_depth_total;
  ir_frontend_metadata.pointer_declarator_token_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_token_entries;
  ir_frontend_metadata.nullability_suffix_entries =
      object_pointer_nullability_generics_summary.nullability_suffix_entries;
  ir_frontend_metadata.generic_suffix_entries = object_pointer_nullability_generics_summary.generic_suffix_entries;
  ir_frontend_metadata.terminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.terminated_generic_suffix_entries;
  ir_frontend_metadata.unterminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries;
  ir_frontend_metadata.global_symbol_nodes = symbol_graph_scope_resolution_summary.global_symbol_nodes;
  ir_frontend_metadata.function_symbol_nodes = symbol_graph_scope_resolution_summary.function_symbol_nodes;
  ir_frontend_metadata.interface_symbol_nodes = symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  ir_frontend_metadata.implementation_symbol_nodes = symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  ir_frontend_metadata.interface_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  ir_frontend_metadata.implementation_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  ir_frontend_metadata.interface_method_symbol_nodes = symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  ir_frontend_metadata.implementation_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  ir_frontend_metadata.top_level_scope_symbols = symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  ir_frontend_metadata.nested_scope_symbols = symbol_graph_scope_resolution_summary.nested_scope_symbols;
  ir_frontend_metadata.scope_frames_total = symbol_graph_scope_resolution_summary.scope_frames_total;
  ir_frontend_metadata.implementation_interface_resolution_sites =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  ir_frontend_metadata.implementation_interface_resolution_hits =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  ir_frontend_metadata.implementation_interface_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  ir_frontend_metadata.method_resolution_sites = symbol_graph_scope_resolution_summary.method_resolution_sites;
  ir_frontend_metadata.method_resolution_hits = symbol_graph_scope_resolution_summary.method_resolution_hits;
  ir_frontend_metadata.method_resolution_misses = symbol_graph_scope_resolution_summary.method_resolution_misses;
  ir_frontend_metadata.deterministic_interface_implementation_handoff =
      pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff &&
      interface_implementation_summary.deterministic;
  ir_frontend_metadata.deterministic_protocol_category_handoff =
      protocol_category_summary.deterministic_protocol_category_handoff;
  ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =
      class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff;
  ir_frontend_metadata.deterministic_selector_normalization_handoff =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  ir_frontend_metadata.deterministic_property_attribute_handoff =
      property_attribute_summary.deterministic_property_attribute_handoff;
  ir_frontend_metadata.runtime_metadata_source_ownership_contract_id =
      runtime_metadata_source_ownership.contract_id;
  ir_frontend_metadata.runtime_metadata_source_schema =
      runtime_metadata_source_ownership.canonical_source_schema;
  ir_frontend_metadata.runtime_metadata_ivar_source_model =
      runtime_metadata_source_ownership.ivar_record_source_model;
  ir_frontend_metadata.runtime_metadata_class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  ir_frontend_metadata.runtime_metadata_protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  ir_frontend_metadata.runtime_metadata_category_interface_record_count =
      runtime_metadata_source_ownership.category_interface_record_count;
  ir_frontend_metadata.runtime_metadata_category_implementation_record_count =
      runtime_metadata_source_ownership.category_implementation_record_count;
  ir_frontend_metadata.runtime_metadata_property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  ir_frontend_metadata.runtime_metadata_method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  ir_frontend_metadata.runtime_metadata_ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  ir_frontend_metadata.frontend_owns_runtime_metadata_source_records =
      runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records;
  ir_frontend_metadata.runtime_metadata_source_records_ready_for_lowering =
      runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering;
  ir_frontend_metadata.native_runtime_library_present =
      runtime_metadata_source_ownership.native_runtime_library_present;
  ir_frontend_metadata.runtime_metadata_source_boundary_fail_closed =
      runtime_metadata_source_ownership.fail_closed;
  ir_frontend_metadata.runtime_link_test_only =
      runtime_metadata_source_ownership.runtime_link_test_only;
  ir_frontend_metadata.deterministic_runtime_metadata_source_schema =
      runtime_metadata_source_ownership.deterministic_source_schema;
  ir_frontend_metadata.runtime_export_legality_contract_id =
      runtime_export_legality.contract_id;
  ir_frontend_metadata.runtime_export_semantic_boundary_frozen =
      runtime_export_legality.semantic_boundary_frozen;
  ir_frontend_metadata.runtime_export_metadata_export_enforcement_ready =
      runtime_export_legality.metadata_export_enforcement_ready;
  ir_frontend_metadata.runtime_export_fail_closed =
      runtime_export_legality.fail_closed;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_enforcement_pending =
      runtime_export_legality.duplicate_runtime_identity_enforcement_pending;
  ir_frontend_metadata.runtime_export_incomplete_declaration_export_blocking_pending =
      runtime_export_legality.incomplete_declaration_export_blocking_pending;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_export_blocking_pending =
      runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending;
  ir_frontend_metadata.runtime_export_class_record_count =
      runtime_export_legality.class_record_count;
  ir_frontend_metadata.runtime_export_protocol_record_count =
      runtime_export_legality.protocol_record_count;
  ir_frontend_metadata.runtime_export_category_record_count =
      runtime_export_legality.category_record_count;
  ir_frontend_metadata.runtime_export_property_record_count =
      runtime_export_legality.property_record_count;
  ir_frontend_metadata.runtime_export_method_record_count =
      runtime_export_legality.method_record_count;
  ir_frontend_metadata.runtime_export_ivar_record_count =
      runtime_export_legality.ivar_record_count;
  ir_frontend_metadata.runtime_export_invalid_protocol_composition_sites =
      runtime_export_legality.invalid_protocol_composition_sites;
  ir_frontend_metadata.runtime_export_property_attribute_invalid_entries =
      runtime_export_legality.property_attribute_invalid_entries;
  ir_frontend_metadata.runtime_export_property_attribute_contract_violations =
      runtime_export_legality.property_attribute_contract_violations;
  ir_frontend_metadata.runtime_export_invalid_type_annotation_sites =
      runtime_export_legality.invalid_type_annotation_sites;
  ir_frontend_metadata.runtime_export_property_ivar_binding_missing =
      runtime_export_legality.property_ivar_binding_missing;
  ir_frontend_metadata.runtime_export_property_ivar_binding_conflicts =
      runtime_export_legality.property_ivar_binding_conflicts;
  ir_frontend_metadata.runtime_export_implementation_resolution_misses =
      runtime_export_legality.implementation_resolution_misses;
  ir_frontend_metadata.runtime_export_method_resolution_misses =
      runtime_export_legality.method_resolution_misses;
  ir_frontend_metadata.runtime_export_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  ir_frontend_metadata.runtime_export_enforcement_contract_id =
      runtime_export_enforcement.contract_id;
  ir_frontend_metadata.runtime_export_metadata_completeness_enforced =
      runtime_export_enforcement.metadata_completeness_enforced;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_suppression_enforced =
      runtime_export_enforcement
          .duplicate_runtime_identity_suppression_enforced;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_blocking_enforced =
      runtime_export_enforcement
          .illegal_redeclaration_mix_blocking_enforced;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_blocking_enforced =
      runtime_export_enforcement.metadata_shape_drift_blocking_enforced;
  ir_frontend_metadata.runtime_export_enforcement_fail_closed =
      runtime_export_enforcement.fail_closed;
  ir_frontend_metadata.runtime_export_ready_for_runtime_export =
      runtime_export_enforcement.ready_for_runtime_export;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_sites =
      runtime_export_enforcement.duplicate_runtime_identity_sites;
  ir_frontend_metadata.runtime_export_incomplete_declaration_sites =
      runtime_export_enforcement.incomplete_declaration_sites;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_sites =
      runtime_export_enforcement.illegal_redeclaration_mix_sites;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_sites =
      runtime_export_enforcement.metadata_shape_drift_sites;
  ir_frontend_metadata.runtime_metadata_section_abi_contract_id =
      runtime_metadata_section_abi.contract_id;
  ir_frontend_metadata.runtime_metadata_section_boundary_frozen =
      runtime_metadata_section_abi.boundary_frozen;
  ir_frontend_metadata.runtime_metadata_section_fail_closed =
      runtime_metadata_section_abi.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_object_file_inventory_frozen =
      runtime_metadata_section_abi.object_file_section_inventory_frozen;
  ir_frontend_metadata.runtime_metadata_section_symbol_policy_frozen =
      runtime_metadata_section_abi.symbol_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_visibility_model_frozen =
      runtime_metadata_section_abi.visibility_model_frozen;
  ir_frontend_metadata.runtime_metadata_section_retention_policy_frozen =
      runtime_metadata_section_abi.retention_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_ready_for_scaffold =
      runtime_metadata_section_abi.ready_for_section_scaffold;
  ir_frontend_metadata.runtime_metadata_section_logical_image_info_section =
      runtime_metadata_section_abi.logical_image_info_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_class_descriptor_section =
      runtime_metadata_section_abi.logical_class_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_protocol_descriptor_section =
      runtime_metadata_section_abi.logical_protocol_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_category_descriptor_section =
      runtime_metadata_section_abi.logical_category_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_property_descriptor_section =
      runtime_metadata_section_abi.logical_property_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section =
      runtime_metadata_section_abi.logical_ivar_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix =
      runtime_metadata_section_abi.descriptor_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_aggregate_symbol_prefix =
      runtime_metadata_section_abi.aggregate_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_image_info_symbol =
      runtime_metadata_section_abi.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_descriptor_linkage =
      runtime_metadata_section_abi.descriptor_linkage;
  ir_frontend_metadata.runtime_metadata_section_aggregate_linkage =
      runtime_metadata_section_abi.aggregate_linkage;
  ir_frontend_metadata.runtime_metadata_section_visibility =
      runtime_metadata_section_abi.metadata_visibility;
  ir_frontend_metadata.runtime_metadata_section_retention_root =
      runtime_metadata_section_abi.retention_root;
  ir_frontend_metadata.runtime_metadata_section_publication_contract_id =
      runtime_metadata_section_publication.contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_abi_contract_id =
      runtime_metadata_section_publication.abi_contract_id;
  ir_frontend_metadata.runtime_metadata_section_publication_emitted =
      runtime_metadata_section_publication.publication_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_fail_closed =
      runtime_metadata_section_publication.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_publication_uses_llvm_used =
      runtime_metadata_section_publication.uses_llvm_used;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_emitted =
      runtime_metadata_section_publication.image_info_emitted;
  ir_frontend_metadata.runtime_metadata_section_publication_class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_publication_total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_publication_total_retained_global_count =
      runtime_metadata_section_publication.total_retained_global_count;
  ir_frontend_metadata.runtime_metadata_section_publication_image_info_symbol =
      runtime_metadata_section_publication.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_class_aggregate_symbol =
      runtime_metadata_section_publication.class_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_protocol_aggregate_symbol =
      runtime_metadata_section_publication.protocol_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_category_aggregate_symbol =
      runtime_metadata_section_publication.category_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_publication_property_aggregate_symbol =
      runtime_metadata_section_publication.property_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_section_publication_ivar_aggregate_symbol =
      runtime_metadata_section_publication.ivar_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_contract_id =
      kObjc3RuntimeClassMetaclassEmissionContractId;
  ir_frontend_metadata.runtime_metadata_class_metaclass_payload_model =
      kObjc3RuntimeClassMetaclassEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_name_model =
      kObjc3RuntimeClassMetaclassEmissionNameModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_super_link_model =
      kObjc3RuntimeClassMetaclassEmissionSuperLinkModel;
  ir_frontend_metadata
      .runtime_metadata_class_metaclass_method_list_reference_model =
      kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel;
  ir_frontend_metadata.executable_class_metaclass_source_closure_contract_id =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_source_closure_contract_id;
  ir_frontend_metadata.executable_class_metaclass_parent_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_parent_identity_model;
  ir_frontend_metadata.executable_class_metaclass_method_owner_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_method_owner_identity_model;
  ir_frontend_metadata.executable_class_metaclass_object_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_object_identity_model;
  ir_frontend_metadata.executable_class_metaclass_source_closure_ready =
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_declaration_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_parent_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_method_owner_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .class_metaclass_object_identity_closure_complete;
  ir_frontend_metadata.executable_class_metaclass_declaration_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .interface_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .implementation_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .class_nodes_lexicographic.size() +
      executable_metadata_typed_lowering_handoff.source_graph
          .metaclass_nodes_lexicographic.size();
  ir_frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.executable_protocol_category_source_closure_contract_id =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_source_closure_contract_id;
  ir_frontend_metadata.executable_protocol_inheritance_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_inheritance_identity_model;
  ir_frontend_metadata.executable_category_attachment_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .category_attachment_identity_model;
  ir_frontend_metadata.executable_protocol_category_conformance_identity_model =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_conformance_identity_model;
  ir_frontend_metadata.executable_protocol_category_source_closure_ready =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_declaration_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_inheritance_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .category_attachment_identity_closure_complete &&
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_category_conformance_identity_closure_complete;
  ir_frontend_metadata.executable_protocol_category_protocol_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .protocol_nodes_lexicographic.size();
  ir_frontend_metadata.executable_protocol_category_category_node_count =
      executable_metadata_typed_lowering_handoff.source_graph
          .category_nodes_lexicographic.size();
  for (const auto &edge :
       executable_metadata_typed_lowering_handoff.source_graph.owner_edges_lexicographic) {
    if (edge.edge_kind == "interface-to-superclass" ||
        edge.edge_kind == "interface-to-super-metaclass" ||
        edge.edge_kind == "implementation-to-superclass" ||
        edge.edge_kind == "implementation-to-super-metaclass" ||
        edge.edge_kind == "class-to-superclass" ||
        edge.edge_kind == "metaclass-to-super-metaclass") {
      ++ir_frontend_metadata
            .executable_class_metaclass_parent_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-instance-method-owner" ||
        edge.edge_kind == "interface-to-class-method-owner" ||
        edge.edge_kind == "implementation-to-instance-method-owner" ||
        edge.edge_kind == "implementation-to-class-method-owner") {
      ++ir_frontend_metadata
            .executable_class_metaclass_method_owner_identity_edge_count;
    }
    if (edge.edge_kind == "interface-to-class" ||
        edge.edge_kind == "interface-to-metaclass" ||
        edge.edge_kind == "implementation-to-class" ||
        edge.edge_kind == "implementation-to-metaclass" ||
        edge.edge_kind == "class-to-metaclass") {
      ++ir_frontend_metadata
            .executable_class_metaclass_object_identity_edge_count;
    }
    if (edge.edge_kind == "protocol-to-inherited-protocol") {
      ++ir_frontend_metadata
            .executable_protocol_inheritance_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-class" ||
        edge.edge_kind == "category-to-interface" ||
        edge.edge_kind == "category-to-implementation") {
      ++ir_frontend_metadata
            .executable_category_attachment_identity_edge_count;
    }
    if (edge.edge_kind == "category-to-protocol") {
      ++ir_frontend_metadata
            .executable_protocol_category_conformance_identity_edge_count;
    }
  }
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_contract_id =
      kObjc3RuntimeProtocolCategoryEmissionContractId;
  ir_frontend_metadata.runtime_metadata_protocol_emission_payload_model =
      kObjc3RuntimeProtocolEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_category_emission_payload_model =
      kObjc3RuntimeCategoryEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_protocol_reference_model =
      kObjc3RuntimeProtocolReferenceModel;
  ir_frontend_metadata.runtime_metadata_category_attachment_model =
      kObjc3RuntimeCategoryAttachmentModel;
  ir_frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_member_table_emission_contract_id =
      kObjc3RuntimeMemberTableEmissionContractId;
  ir_frontend_metadata.runtime_metadata_method_list_emission_payload_model =
      kObjc3RuntimeMethodListEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_method_list_grouping_model =
      kObjc3RuntimeMethodListEmissionGroupingModel;
  ir_frontend_metadata.runtime_metadata_property_descriptor_emission_payload_model =
      kObjc3RuntimePropertyDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_ivar_descriptor_emission_payload_model =
      kObjc3RuntimeIvarDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  ir_frontend_metadata.runtime_metadata_archive_static_link_anchor_seed_model =
      kObjc3RuntimeArchiveStaticLinkAnchorSeedModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_model =
      kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_merge_model =
      kObjc3RuntimeArchiveStaticLinkMergeModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_response_artifact_suffix =
      kObjc3RuntimeMergedLinkerResponseArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix =
      kObjc3RuntimeMergedDiscoveryArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_ready =
      true;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_key =
      BuildObjc3TranslationUnitIdentityKey(
          input_path, bundle.parse_lowering_readiness_surface);
  // bootstrap materialization anchor: the native IR emitter consumes
  // this lowering packet directly when it materializes the ctor root, derived
  // init stub, registration table, and image descriptor. Driver/process code
  // may publish the same packet, but they may not re-derive those symbol
  // shapes independently from truncated sidecar state.
  // registration-table/image-local-init anchor: the same lowering
  // packet now also carries the self-describing registration-table layout,
  // ABI/version counts, and image-local init-state model that the emitter,
  // manifest writers, and later runtime image-walk code must preserve exactly.
  ir_frontend_metadata.runtime_bootstrap_lowering_contract_id =
      bundle.runtime_bootstrap_lowering_summary.contract_id;
  ir_frontend_metadata.runtime_bootstrap_lowering_boundary_model =
      bundle.runtime_bootstrap_lowering_summary.lowering_boundary_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .constructor_init_stub_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_init_state_symbol_prefix;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_entrypoint_symbol =
      bundle.runtime_bootstrap_lowering_summary.registration_entrypoint_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model =
      bundle.runtime_bootstrap_lowering_summary.global_ctor_list_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_layout_model =
      bundle.runtime_bootstrap_lowering_summary.registration_table_layout_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_image_local_initialization_model =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_abi_version =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_abi_version;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_pointer_field_count =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_pointer_field_count;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_emission_state =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_emission_state;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_emission_state =
      bundle.runtime_bootstrap_lowering_summary.init_stub_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_emission_state =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .bootstrap_ir_materialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_initialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id =
      kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId;
  ir_frontend_metadata.runtime_bootstrap_registration_descriptor_identifier =
      bundle.runtime_registration_descriptor_frontend_closure_summary
          .registration_descriptor_identifier;
  ir_frontend_metadata.runtime_bootstrap_image_root_identifier =
      bundle.runtime_registration_descriptor_frontend_closure_summary
          .image_root_identifier;
  ir_frontend_metadata.runtime_bootstrap_registration_order_ordinal =
      bundle.runtime_translation_unit_registration_manifest_summary
          .translation_unit_registration_order_ordinal;
  ir_frontend_metadata.runtime_bootstrap_lowering_ready =
      bundle.runtime_bootstrap_lowering_summary
          .ready_for_bootstrap_materialization;
  ir_frontend_metadata.runtime_bootstrap_lowering_fail_closed =
      bundle.runtime_bootstrap_lowering_summary.fail_closed;
  {
    const bool typed_handoff_ready =
        IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
            executable_metadata_typed_lowering_handoff);
    if (typed_handoff_ready) {
      const auto &source_graph =
          executable_metadata_typed_lowering_handoff.source_graph;
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataClassGraphNode *>
          class_nodes_by_name;
      class_nodes_by_name.reserve(source_graph.class_nodes_lexicographic.size());
      for (const auto &class_node : source_graph.class_nodes_lexicographic) {
        class_nodes_by_name.emplace(class_node.class_name, &class_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataMetaclassGraphNode *>
          metaclass_nodes_by_name;
      metaclass_nodes_by_name.reserve(
          source_graph.metaclass_nodes_lexicographic.size());
      for (const auto &metaclass_node :
           source_graph.metaclass_nodes_lexicographic) {
        metaclass_nodes_by_name.emplace(metaclass_node.class_name,
                                        &metaclass_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataImplementationGraphNode *>
          implementation_nodes_by_name;
      implementation_nodes_by_name.reserve(
          source_graph.implementation_nodes_lexicographic.size());
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        implementation_nodes_by_name.emplace(implementation_node.class_name,
                                             &implementation_node);
      }

      bool bundle_payload_complete = true;
      std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle> bundles;
      bundles.reserve(source_graph.interface_nodes_lexicographic.size() +
                      source_graph.implementation_nodes_lexicographic.size());
      for (const auto &interface_node : source_graph.interface_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(interface_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(interface_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = interface_node.class_name;
        bundle.owner_identity = interface_node.owner_identity;
        bundle.class_owner_identity = interface_node.class_owner_identity;
        bundle.metaclass_owner_identity = interface_node.metaclass_owner_identity;
        bundle.has_super = class_it->second->has_super;
        bundle.super_class_owner_identity =
            class_it->second->super_class_owner_identity;
        bundle.super_metaclass_owner_identity =
            class_it->second->super_metaclass_owner_identity;
        bundle.super_bundle_owner_identity =
            class_it->second->has_super
                ? ("interface:" + class_it->second->super_class_owner_identity.substr(6))
                : std::string{};
        bundle.adopted_protocol_owner_identities_lexicographic =
            class_it->second->adopted_protocol_owner_identities_lexicographic;
        bundle.instance_method_owner_identity =
            interface_node.instance_method_owner_identity;
        bundle.class_method_owner_identity =
            interface_node.class_method_owner_identity;
        bundle.objc_final_declared = class_it->second->objc_final_declared;
        bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
        bundle.instance_method_count = interface_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->interface_class_method_count;
        bundles.push_back(std::move(bundle));
      }
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(implementation_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(implementation_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = implementation_node.class_name;
        bundle.owner_identity = implementation_node.owner_identity;
        bundle.class_owner_identity = implementation_node.class_owner_identity;
        bundle.metaclass_owner_identity =
            implementation_node.metaclass_owner_identity;
        bundle.has_super = class_it->second->has_super;
        bundle.super_class_owner_identity =
            implementation_node.super_class_owner_identity;
        bundle.super_metaclass_owner_identity =
            implementation_node.super_metaclass_owner_identity;
        if (class_it->second->has_super) {
          const std::string super_class_name =
              class_it->second->super_class_owner_identity.substr(6);
          const auto super_impl_it =
              implementation_nodes_by_name.find(super_class_name);
          if (super_impl_it == implementation_nodes_by_name.end()) {
            bundle_payload_complete = false;
            break;
          }
          bundle.super_bundle_owner_identity =
              super_impl_it->second->owner_identity;
        }
        bundle.adopted_protocol_owner_identities_lexicographic =
            class_it->second->adopted_protocol_owner_identities_lexicographic;
        bundle.instance_method_owner_identity =
            implementation_node.instance_method_owner_identity;
        bundle.class_method_owner_identity =
            implementation_node.class_method_owner_identity;
        bundle.objc_final_declared = class_it->second->objc_final_declared;
        bundle.objc_sealed_declared = class_it->second->objc_sealed_declared;
        bundle.instance_method_count = implementation_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->implementation_class_method_count;
        bundles.push_back(std::move(bundle));
      }

      bundle_payload_complete =
          bundle_payload_complete &&
          bundles.size() ==
              runtime_metadata_section_publication.class_descriptor_count;
      if (bundle_payload_complete) {
        ir_frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic =
            std::move(bundles);
      }
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_ready =
          bundle_payload_complete;
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed =
          bundle_payload_complete;

      // protocol/category data emission anchor: the typed lowering
      // handoff now expands one combined category graph node into explicit
      // interface/implementation record bundles so the emitted descriptor count
      // matches the runtime-export/scaffold record inventory exactly.
      bool protocol_category_payload_complete = true;
      std::unordered_set<std::string> protocol_owner_identities;
      protocol_owner_identities.reserve(
          source_graph.protocol_nodes_lexicographic.size());
      std::vector<Objc3IRRuntimeMetadataProtocolBundle> protocol_bundles;
      protocol_bundles.reserve(source_graph.protocol_nodes_lexicographic.size());
      for (const auto &protocol_node : source_graph.protocol_nodes_lexicographic) {
        if (protocol_node.protocol_name.empty() ||
            protocol_node.owner_identity.empty() ||
            !protocol_owner_identities.insert(protocol_node.owner_identity).second) {
          protocol_category_payload_complete = false;
          break;
        }
        for (const auto &inherited_owner_identity :
             protocol_node.inherited_protocol_owner_identities_lexicographic) {
          if (inherited_owner_identity.empty()) {
            protocol_category_payload_complete = false;
            break;
          }
        }
        if (!protocol_category_payload_complete) {
          break;
        }

        Objc3IRRuntimeMetadataProtocolBundle bundle;
        bundle.protocol_name = protocol_node.protocol_name;
        bundle.owner_identity = protocol_node.owner_identity;
        bundle.inherited_protocol_owner_identities_lexicographic =
            protocol_node.inherited_protocol_owner_identities_lexicographic;
        bundle.property_count = protocol_node.property_count;
        bundle.method_count = protocol_node.method_count;
        bundle.is_forward_declaration = protocol_node.is_forward_declaration;
        protocol_bundles.push_back(std::move(bundle));
      }

      std::vector<Objc3IRRuntimeMetadataCategoryBundle> category_bundles;
      category_bundles.reserve(source_graph.category_nodes_lexicographic.size());
      std::unordered_set<std::string> category_owner_identities;
      category_owner_identities.reserve(
          runtime_metadata_section_publication.category_descriptor_count);
      if (protocol_category_payload_complete) {
        for (const auto &category_node :
             source_graph.category_nodes_lexicographic) {
          if (category_node.class_name.empty() ||
              category_node.category_name.empty() ||
              category_node.owner_identity.empty() ||
              category_node.class_owner_identity.empty() ||
              (!category_node.has_interface &&
               !category_node.has_implementation)) {
            protocol_category_payload_complete = false;
            break;
          }

          for (const auto &protocol_owner_identity :
               category_node.adopted_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identity.empty() ||
                protocol_owner_identities.find(protocol_owner_identity) ==
                    protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }
