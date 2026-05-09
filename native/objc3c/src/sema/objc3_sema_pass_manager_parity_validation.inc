              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .actor_isolation_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .guard_blocked_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .contract_violation_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              result.parity_surface.concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary.deterministic;
  result.unsafe_error_parity_validation_readiness_record =
      BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_unsafe_pointer_extension_handoff,
          result.deterministic_inline_asm_intrinsic_governance_handoff,
          result.deterministic_ns_error_bridging_handoff,
          result.deterministic_error_diagnostics_recovery_handoff,
          result.deterministic_result_like_lowering_handoff);
  result.deterministic_unsafe_error_parity_validation_readiness_record =
      IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
          result.unsafe_error_parity_validation_readiness_record);
  result.parity_surface.unsafe_error_parity_validation_readiness_record =
      result.unsafe_error_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_unsafe_error_parity_validation_readiness_record =
      result.deterministic_unsafe_error_parity_validation_readiness_record;
  result.parity_surface.deterministic_unsafe_pointer_extension_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .unsafe_pointer_extension_ready;
  result.parity_surface.deterministic_inline_asm_intrinsic_governance_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .inline_asm_intrinsic_governance_ready;
  result.parity_surface.deterministic_ns_error_bridging_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .ns_error_bridging_ready;
  result.parity_surface.deterministic_error_diagnostics_recovery_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .error_diagnostics_recovery_ready;
  result.parity_surface.deterministic_result_like_lowering_handoff =
      result.parity_surface.unsafe_error_parity_validation_readiness_record
          .result_like_lowering_ready;
  result.control_binding_parity_validation_readiness_record =
      BuildObjc3SemaControlBindingParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_unwind_cleanup_handoff,
          result.deterministic_async_continuation_handoff,
          result.deterministic_symbol_graph_scope_resolution_handoff,
          result.deterministic_method_lookup_override_conflict_handoff,
          result.deterministic_property_synthesis_ivar_binding_handoff,
          result.deterministic_id_class_sel_object_pointer_type_checking_handoff);
  result.deterministic_control_binding_parity_validation_readiness_record =
      IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
          result.control_binding_parity_validation_readiness_record);
  result.parity_surface.control_binding_parity_validation_readiness_record =
      result.control_binding_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_control_binding_parity_validation_readiness_record =
      result.deterministic_control_binding_parity_validation_readiness_record;
  result.parity_surface.deterministic_unwind_cleanup_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .unwind_cleanup_ready;
  result.parity_surface.deterministic_async_continuation_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .async_continuation_ready;
  result.async_block_message_parity_validation_readiness_record =
      BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_await_lowering_suspension_state_lowering_handoff,
          result.deterministic_block_literal_capture_semantics_handoff,
          result.deterministic_block_abi_invoke_trampoline_handoff,
          result.deterministic_block_storage_escape_handoff,
          result.deterministic_block_copy_dispose_handoff,
          result.deterministic_block_determinism_perf_baseline_handoff,
          result.deterministic_message_send_selector_lowering_handoff);
  result.deterministic_async_block_message_parity_validation_readiness_record =
      IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
          result.async_block_message_parity_validation_readiness_record);
  result.parity_surface.async_block_message_parity_validation_readiness_record =
      result.async_block_message_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_async_block_message_parity_validation_readiness_record =
      result.deterministic_async_block_message_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .await_lowering_suspension_state_lowering_ready;
  result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .symbol_graph_scope_resolution_ready;
  result.parity_surface.deterministic_method_lookup_override_conflict_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .method_lookup_override_conflict_ready;
  result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .property_synthesis_ivar_binding_ready;
  result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.parity_surface.control_binding_parity_validation_readiness_record
          .id_class_sel_object_pointer_type_checking_ready;
  result.parity_surface.deterministic_block_literal_capture_semantics_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .block_literal_capture_semantics_ready;
  result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .block_abi_invoke_trampoline_ready;
  result.parity_surface.deterministic_block_storage_escape_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .block_storage_escape_ready;
  result.parity_surface.deterministic_block_copy_dispose_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .block_copy_dispose_ready;
  result.parity_surface.deterministic_block_determinism_perf_baseline_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .block_determinism_perf_baseline_ready;
  result.parity_surface.deterministic_message_send_selector_lowering_handoff =
      result.parity_surface
          .async_block_message_parity_validation_readiness_record
          .message_send_selector_lowering_ready;
  result.dispatch_runtime_arc_parity_validation_readiness_record =
      BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_dispatch_abi_marshalling_handoff,
          result.deterministic_nil_receiver_semantics_foldability_handoff,
          result.deterministic_super_dispatch_method_family_handoff,
          result.deterministic_runtime_link_host_link_handoff,
          result.deterministic_retain_release_operation_handoff,
          result.deterministic_weak_unowned_semantics_handoff,
          result.deterministic_arc_diagnostics_fixit_handoff,
          result.deterministic_autoreleasepool_scope_handoff);
  result.deterministic_dispatch_runtime_arc_parity_validation_readiness_record =
      IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
          result.dispatch_runtime_arc_parity_validation_readiness_record);
  result.parity_surface.dispatch_runtime_arc_parity_validation_readiness_record =
      result.dispatch_runtime_arc_parity_validation_readiness_record;
  result.parity_surface
      .deterministic_dispatch_runtime_arc_parity_validation_readiness_record =
      result.deterministic_dispatch_runtime_arc_parity_validation_readiness_record;
  result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .dispatch_abi_marshalling_ready;
  result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .nil_receiver_semantics_foldability_ready;
  result.parity_surface.deterministic_super_dispatch_method_family_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .super_dispatch_method_family_ready;
  result.parity_surface.deterministic_runtime_link_host_link_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .runtime_link_host_link_ready;
  result.parity_surface.deterministic_retain_release_operation_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .retain_release_operation_ready;
  result.parity_surface.deterministic_weak_unowned_semantics_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .weak_unowned_semantics_ready;
  result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .arc_diagnostics_fixit_ready;
  result.parity_surface.deterministic_autoreleasepool_scope_handoff =
      result.parity_surface
          .dispatch_runtime_arc_parity_validation_readiness_record
          .autoreleasepool_scope_ready;
  result.atomic_vector_mapping_publication_record =
      BuildObjc3SemaAtomicVectorMappingPublicationRecord(
          input,
          result.atomic_memory_order_mapping,
          result.deterministic_atomic_memory_order_mapping,
          result.vector_type_lowering,
          result.deterministic_vector_type_lowering);
  result.deterministic_atomic_vector_mapping_publication_record =
      IsReadyObjc3SemaAtomicVectorMappingPublicationRecord(
          result.atomic_vector_mapping_publication_record);
  result.parity_surface.atomic_vector_mapping_publication_record =
      result.atomic_vector_mapping_publication_record;
  result.parity_surface.deterministic_atomic_vector_mapping_publication_record =
      result.deterministic_atomic_vector_mapping_publication_record;
  result.parity_surface.atomic_memory_order_mapping =
      result.parity_surface.atomic_vector_mapping_publication_record
          .atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping =
      result.parity_surface.atomic_vector_mapping_publication_record
          .deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering =
      result.parity_surface.atomic_vector_mapping_publication_record
          .vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering =
      result.parity_surface.atomic_vector_mapping_publication_record
          .deterministic_vector_type_lowering;
  result.type_metadata_mapping_readiness_record =
      BuildObjc3SemaTypeMetadataMappingReadinessRecord(
          input, result.parity_surface);
  result.deterministic_type_metadata_mapping_readiness_record =
      IsReadyObjc3SemaTypeMetadataMappingReadinessRecord(
          result.type_metadata_mapping_readiness_record);
  result.parity_surface.type_metadata_mapping_readiness_record =
      result.type_metadata_mapping_readiness_record;
  result.parity_surface.deterministic_type_metadata_mapping_readiness_record =
      result.deterministic_type_metadata_mapping_readiness_record;
  result.typed_semantic_handoff_record =
      BuildObjc3SemaTypedSemanticHandoffRecord(input, result.parity_surface);
  result.deterministic_typed_semantic_handoff_record =
      IsReadyObjc3SemaTypedSemanticHandoffRecord(
          result.typed_semantic_handoff_record);
  result.parity_surface.typed_semantic_handoff_record =
      result.typed_semantic_handoff_record;
  result.parity_surface.deterministic_typed_semantic_handoff_record =
      result.deterministic_typed_semantic_handoff_record;
  result.parser_sema_conformance_evidence_record =
      BuildObjc3ParserSemaConformanceEvidenceRecord(input, result.parity_surface);
  result.deterministic_parser_sema_conformance_evidence_record =
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          result.parser_sema_conformance_evidence_record);
  result.parity_surface.parser_sema_conformance_evidence_record =
      result.parser_sema_conformance_evidence_record;
  result.parity_surface.deterministic_parser_sema_conformance_evidence_record =
      result.deterministic_parser_sema_conformance_evidence_record;
  result.parser_sema_contract_readiness_record =
      BuildObjc3ParserSemaContractReadinessRecord(input, result.parity_surface);
  result.deterministic_parser_sema_contract_readiness_record =
      IsReadyObjc3ParserSemaContractReadinessRecord(
          result.parser_sema_contract_readiness_record);
  result.parity_surface.parser_sema_contract_readiness_record =
      result.parser_sema_contract_readiness_record;
  result.parity_surface.deterministic_parser_sema_contract_readiness_record =
      result.deterministic_parser_sema_contract_readiness_record;
  result.parity_closeout_publication_readiness_record =
      BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
          input, result.executed, result.parity_surface);
  result.deterministic_parity_closeout_publication_readiness_record =
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          result.parity_closeout_publication_readiness_record);
  result.parity_surface.parity_closeout_publication_readiness_record =
      result.parity_closeout_publication_readiness_record;
  result.parity_surface
      .deterministic_parity_closeout_publication_readiness_record =
      result.deterministic_parity_closeout_publication_readiness_record;
  result.parity_validation_record = BuildObjc3SemaParityValidationRecord(
      input,
      result.parity_closeout_publication_readiness_record);
  result.deterministic_parity_validation_record =
      IsReadyObjc3SemaParityValidationRecord(result.parity_validation_record);
  result.parity_surface.parity_validation_record =
      result.parity_validation_record;
  result.parity_surface.deterministic_parity_validation_record =
      result.deterministic_parity_validation_record;
  result.closeout_surface_readiness_record =
      BuildObjc3SemaCloseoutSurfaceReadinessRecord(
          input, result.parity_surface);
  result.deterministic_closeout_surface_readiness_record =
      IsReadyObjc3SemaCloseoutSurfaceReadinessRecord(
          result.closeout_surface_readiness_record);
  result.parity_surface.closeout_surface_readiness_record =
      result.closeout_surface_readiness_record;
  result.parity_surface.deterministic_closeout_surface_readiness_record =
      result.deterministic_closeout_surface_readiness_record;
  result.core_semantic_summary_readiness_record =
      BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_core_semantic_summary_readiness_record =
      IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
          result.core_semantic_summary_readiness_record);
  result.parity_surface.core_semantic_summary_readiness_record =
      result.core_semantic_summary_readiness_record;
  result.parity_surface.deterministic_core_semantic_summary_readiness_record =
      result.deterministic_core_semantic_summary_readiness_record;
  result.selector_property_type_annotation_readiness_record =
      BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          input, result.parity_surface);
  result.deterministic_selector_property_type_annotation_readiness_record =
      IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          result.selector_property_type_annotation_readiness_record);
  result.parity_surface.selector_property_type_annotation_readiness_record =
      result.selector_property_type_annotation_readiness_record;
  result.parity_surface
      .deterministic_selector_property_type_annotation_readiness_record =
      result.deterministic_selector_property_type_annotation_readiness_record;
  result.type_boundary_summary_readiness_record =
      BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_type_boundary_summary_readiness_record =
      IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
          result.type_boundary_summary_readiness_record);
  result.parity_surface.type_boundary_summary_readiness_record =
      result.type_boundary_summary_readiness_record;
  result.parity_surface.deterministic_type_boundary_summary_readiness_record =
      result.deterministic_type_boundary_summary_readiness_record;
  result.module_type_abi_summary_readiness_record =
      BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_module_type_abi_summary_readiness_record =
      IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          result.module_type_abi_summary_readiness_record);
  result.parity_surface.module_type_abi_summary_readiness_record =
      result.module_type_abi_summary_readiness_record;
  result.parity_surface.deterministic_module_type_abi_summary_readiness_record =
      result.deterministic_module_type_abi_summary_readiness_record;
  result.module_boundary_summary_readiness_record =
      BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
          input, result.parity_surface);
  result.deterministic_module_boundary_summary_readiness_record =
      IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
          result.module_boundary_summary_readiness_record);
  result.parity_surface.module_boundary_summary_readiness_record =
      result.module_boundary_summary_readiness_record;
  result.parity_surface
      .deterministic_module_boundary_summary_readiness_record =
      result.deterministic_module_boundary_summary_readiness_record;
  result.intermodule_flow_summary_readiness_record =
      BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_cross_module_conformance_handoff,
          result.deterministic_throws_propagation_handoff);
  result.deterministic_intermodule_flow_summary_readiness_record =
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          result.intermodule_flow_summary_readiness_record);
  result.parity_surface.intermodule_flow_summary_readiness_record =
      result.intermodule_flow_summary_readiness_record;
  result.parity_surface
      .deterministic_intermodule_flow_summary_readiness_record =
      result.deterministic_intermodule_flow_summary_readiness_record;
  result.parity_surface.deterministic_cross_module_conformance_handoff =
      result.parity_surface.intermodule_flow_summary_readiness_record
          .cross_module_conformance_ready;
  result.parity_surface.deterministic_throws_propagation_handoff =
      result.parity_surface.intermodule_flow_summary_readiness_record
          .throws_propagation_ready;
  result.parity_surface.ready =
      result.parity_surface
          .deterministic_parity_closeout_publication_readiness_record &&
      IsReadyObjc3SemaParityCloseoutPublicationReadinessRecord(
          result.parity_surface
              .parity_closeout_publication_readiness_record) &&
      result.parity_surface.deterministic_parity_validation_record &&
      IsReadyObjc3SemaParityValidationRecord(
          result.parity_surface.parity_validation_record) &&
      result.parity_surface
          .deterministic_core_semantic_summary_readiness_record &&
      IsReadyObjc3SemaCoreSemanticSummaryReadinessRecord(
          result.parity_surface.core_semantic_summary_readiness_record) &&
      result.parity_surface
          .deterministic_selector_property_type_annotation_readiness_record &&
      IsReadyObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
          result.parity_surface
              .selector_property_type_annotation_readiness_record) &&
      result.parity_surface
          .deterministic_type_boundary_summary_readiness_record &&
      IsReadyObjc3SemaTypeBoundarySummaryReadinessRecord(
          result.parity_surface.type_boundary_summary_readiness_record) &&
      result.parity_surface
          .deterministic_module_type_abi_summary_readiness_record &&
      IsReadyObjc3SemaModuleTypeAbiSummaryReadinessRecord(
          result.parity_surface.module_type_abi_summary_readiness_record) &&
      result.parity_surface
          .deterministic_module_boundary_summary_readiness_record &&
      IsReadyObjc3SemaModuleBoundarySummaryReadinessRecord(
          result.parity_surface.module_boundary_summary_readiness_record) &&
      result.parity_surface
          .deterministic_intermodule_flow_summary_readiness_record &&
      IsReadyObjc3SemaIntermoduleFlowSummaryReadinessRecord(
          result.parity_surface.intermodule_flow_summary_readiness_record);
