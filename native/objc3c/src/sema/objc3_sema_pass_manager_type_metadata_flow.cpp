#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaTypedSemanticHandoffRecord BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypedSemanticHandoffRecord record;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.type_metadata_identity_handoffs_ready =
      surface.deterministic_interface_implementation_handoff &&
      surface.deterministic_protocol_category_composition_handoff &&
      surface.deterministic_class_protocol_category_linking_handoff &&
      surface.deterministic_selector_normalization_handoff;
  record.type_annotation_handoffs_ready =
      surface.deterministic_property_attribute_handoff &&
      surface.deterministic_type_annotation_surface_handoff &&
      surface.deterministic_lightweight_generic_constraint_handoff &&
      surface.deterministic_nullability_flow_warning_precision_handoff &&
      surface.deterministic_protocol_qualified_object_type_handoff &&
      surface.deterministic_variance_bridge_cast_handoff &&
      surface.deterministic_generic_metadata_abi_handoff;
  record.module_boundary_handoffs_ready =
      surface.deterministic_module_import_graph_handoff &&
      surface.deterministic_namespace_collision_shadowing_handoff &&
      surface.deterministic_public_private_api_partition_handoff &&
      surface.deterministic_incremental_module_cache_invalidation_handoff &&
      surface.deterministic_cross_module_conformance_handoff;
  record.concurrency_recovery_handoffs_ready =
      surface.deterministic_throws_propagation_handoff &&
      surface.deterministic_unwind_cleanup_handoff &&
      surface.deterministic_async_continuation_handoff &&
      surface.deterministic_actor_isolation_sendability_handoff &&
      surface.deterministic_task_runtime_cancellation_handoff &&
      surface.deterministic_concurrency_replay_race_guard_handoff &&
      surface.deterministic_unsafe_pointer_extension_handoff &&
      surface.deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.deterministic_ns_error_bridging_handoff &&
      surface.deterministic_error_diagnostics_recovery_handoff &&
      surface.deterministic_result_like_lowering_handoff &&
      surface.deterministic_await_lowering_suspension_state_lowering_handoff;
  record.symbol_dispatch_handoffs_ready =
      surface.deterministic_symbol_graph_scope_resolution_handoff &&
      surface.deterministic_method_lookup_override_conflict_handoff &&
      surface.deterministic_property_synthesis_ivar_binding_handoff &&
      surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.deterministic_message_send_selector_lowering_handoff &&
      surface.deterministic_dispatch_abi_marshalling_handoff &&
      surface.deterministic_nil_receiver_semantics_foldability_handoff &&
      surface.deterministic_super_dispatch_method_family_handoff;
  record.block_dispatch_handoffs_ready =
      surface.deterministic_block_literal_capture_semantics_handoff &&
      surface.deterministic_block_abi_invoke_trampoline_handoff &&
      surface.deterministic_block_storage_escape_handoff &&
      surface.deterministic_block_copy_dispose_handoff &&
      surface.deterministic_block_determinism_perf_baseline_handoff;
  record.ownership_runtime_handoffs_ready =
      surface.deterministic_runtime_link_host_link_handoff &&
      surface.deterministic_retain_release_operation_handoff &&
      surface.deterministic_weak_unowned_semantics_handoff &&
      surface.deterministic_arc_diagnostics_fixit_handoff &&
      surface.deterministic_autoreleasepool_scope_handoff;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_identity_handoffs_ready &&
      record.type_annotation_handoffs_ready &&
      record.module_boundary_handoffs_ready &&
      record.concurrency_recovery_handoffs_ready &&
      record.symbol_dispatch_handoffs_ready &&
      record.block_dispatch_handoffs_ready &&
      record.ownership_runtime_handoffs_ready;
  return record;
}

Objc3SemaAtomicVectorMappingPublicationRecord
BuildObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3AtomicMemoryOrderMappingSummary &atomic_memory_order_mapping,
    bool deterministic_atomic_memory_order_mapping,
    const Objc3VectorTypeLoweringSummary &vector_type_lowering,
    bool deterministic_vector_type_lowering) {
  Objc3SemaAtomicVectorMappingPublicationRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.atomic_memory_order_mapping = atomic_memory_order_mapping;
  record.deterministic_atomic_memory_order_mapping =
      deterministic_atomic_memory_order_mapping;
  record.vector_type_lowering = vector_type_lowering;
  record.deterministic_vector_type_lowering =
      deterministic_vector_type_lowering;
  record.atomic_memory_order_mapping_ready =
      record.deterministic_atomic_memory_order_mapping &&
      record.atomic_memory_order_mapping.deterministic;
  record.vector_type_lowering_ready =
      record.deterministic_vector_type_lowering &&
      record.vector_type_lowering.deterministic;
  record.mapping_summaries_ready =
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.atomic_vector_mapping_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready && record.mapping_summaries_ready;
  return record;
}

Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3SemaTypeMetadataMappingReadinessRecord record;
  record.integration_surface_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.globals_total = surface.globals_total;
  record.functions_total = surface.functions_total;
  record.interfaces_total = surface.interfaces_total;
  record.implementations_total = surface.implementations_total;
  record.type_metadata_global_entries = surface.type_metadata_global_entries;
  record.type_metadata_function_entries =
      surface.type_metadata_function_entries;
  record.type_metadata_interface_entries =
      surface.type_metadata_interface_entries;
  record.type_metadata_implementation_entries =
      surface.type_metadata_implementation_entries;
  record.type_metadata_publication_ready =
      surface.deterministic_type_metadata_publication_record &&
      IsReadyObjc3SemaTypeMetadataPublicationRecord(
          surface.type_metadata_publication_record);
  record.type_metadata_handoff_ready =
      surface.deterministic_type_metadata_handoff;
  record.cardinality_consistent =
      record.globals_total == record.type_metadata_global_entries &&
      record.functions_total == record.type_metadata_function_entries &&
      record.interfaces_total == record.type_metadata_interface_entries &&
      record.implementations_total ==
          record.type_metadata_implementation_entries;
  const Objc3SemaAtomicVectorMappingPublicationRecord &mapping_publication =
      surface.atomic_vector_mapping_publication_record;
  record.atomic_vector_mapping_publication_owner =
      mapping_publication.atomic_vector_mapping_publication_owner;
  record.atomic_memory_order_mapping_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.atomic_memory_order_mapping_ready;
  record.vector_type_lowering_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.vector_type_lowering_ready;
  record.mapping_summaries_ready =
      surface.deterministic_atomic_vector_mapping_publication_record &&
      mapping_publication.mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.integration_surface_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.atomic_vector_mapping_publication_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.type_metadata_publication_ready &&
      record.type_metadata_handoff_ready && record.cardinality_consistent &&
      record.atomic_memory_order_mapping_ready &&
      record.vector_type_lowering_ready && record.mapping_summaries_ready;
  return record;
}

bool Objc3ParserSemaSyncCountsReady(std::size_t expected_count,
                                    std::size_t required_count,
                                    std::size_t passed_count,
                                    std::size_t failed_count) {
  return required_count == expected_count && passed_count == required_count &&
         failed_count == 0u;
}

std::size_t Objc3SemaEvidenceCount(bool ready) {
  return ready ? 1u : 0u;
}
