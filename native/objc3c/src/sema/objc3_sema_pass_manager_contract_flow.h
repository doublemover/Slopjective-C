#pragma once

#include "sema/objc3_sema_parity_contract_surface.h"

Objc3SemaCoreSemanticSummaryReadinessRecord
BuildObjc3SemaCoreSemanticSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaSelectorPropertyTypeAnnotationReadinessRecord
BuildObjc3SemaSelectorPropertyTypeAnnotationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaTypeBoundarySummaryReadinessRecord
BuildObjc3SemaTypeBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaModuleTypeAbiSummaryReadinessRecord
BuildObjc3SemaModuleTypeAbiSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaModuleBoundarySummaryReadinessRecord
BuildObjc3SemaModuleBoundarySummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaTypedSemanticHandoffRecord
BuildObjc3SemaTypedSemanticHandoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaAtomicVectorMappingPublicationRecord
BuildObjc3SemaAtomicVectorMappingPublicationRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3AtomicMemoryOrderMappingSummary &atomic_memory_order_mapping,
    bool deterministic_atomic_memory_order_mapping,
    const Objc3VectorTypeLoweringSummary &vector_type_lowering,
    bool deterministic_vector_type_lowering);

Objc3SemaTypeMetadataMappingReadinessRecord
BuildObjc3SemaTypeMetadataMappingReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

bool Objc3ParserSemaSyncCountsReady(
    std::size_t expected_count,
    std::size_t required_count,
    std::size_t passed_count,
    std::size_t failed_count);

std::size_t Objc3SemaEvidenceCount(bool ready);

Objc3ParserSemaParityPublicationReadinessRecord
BuildObjc3ParserSemaParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffPublicationTransferRecord &transfer_record,
    bool deterministic_transfer_record);

Objc3SemaCoreSemanticParityPublicationReadinessRecord
BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff,
    bool deterministic_interface_implementation_handoff,
    bool deterministic_protocol_category_composition_handoff,
    bool deterministic_class_protocol_category_linking_handoff,
    bool deterministic_selector_normalization_handoff,
    bool deterministic_property_attribute_handoff,
    bool deterministic_type_annotation_surface_handoff,
    bool deterministic_lightweight_generic_constraint_handoff,
    bool deterministic_nullability_flow_warning_precision_handoff,
    bool deterministic_protocol_qualified_object_type_handoff,
    bool deterministic_variance_bridge_cast_handoff);

Objc3SemaModuleSemanticParityPublicationReadinessRecord
BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_generic_metadata_abi_handoff,
    bool deterministic_module_import_graph_handoff,
    bool deterministic_namespace_collision_shadowing_handoff,
    bool deterministic_public_private_api_partition_handoff,
    bool deterministic_incremental_module_cache_invalidation_handoff);

Objc3SemaIntermoduleFlowSummaryReadinessRecord
BuildObjc3SemaIntermoduleFlowSummaryReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

Objc3SemaIntermoduleFlowParityPublicationReadinessRecord
BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_cross_module_conformance_handoff,
    bool deterministic_throws_propagation_handoff);

Objc3SemaConcurrencyParityPublicationReadinessRecord
BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_actor_isolation_sendability_handoff,
    bool deterministic_task_runtime_cancellation_handoff,
    bool deterministic_concurrency_replay_race_guard_handoff);

Objc3SemaUnsafeErrorParityValidationReadinessRecord
BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unsafe_pointer_extension_handoff,
    bool deterministic_inline_asm_intrinsic_governance_handoff,
    bool deterministic_ns_error_bridging_handoff,
    bool deterministic_error_diagnostics_recovery_handoff,
    bool deterministic_result_like_lowering_handoff);

Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff);

Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_await_lowering_suspension_state_lowering_handoff,
    bool deterministic_block_literal_capture_semantics_handoff,
    bool deterministic_block_abi_invoke_trampoline_handoff,
    bool deterministic_block_storage_escape_handoff,
    bool deterministic_block_copy_dispose_handoff,
    bool deterministic_block_determinism_perf_baseline_handoff,
    bool deterministic_message_send_selector_lowering_handoff);

Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff);

Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

#include "sema/objc3_sema_pass_manager_closeout_readiness.inc"

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface);

#include "sema/objc3_sema_pass_manager_surface_readiness.inc"

bool IsReadyObjc3SemaParityContractSurface(
    const Objc3SemaParityContractSurface &surface);
