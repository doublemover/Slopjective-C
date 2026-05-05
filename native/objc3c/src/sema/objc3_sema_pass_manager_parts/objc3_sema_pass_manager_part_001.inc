#include "sema/objc3_sema_pass_manager.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <vector>

#include "diag/objc3_diag_utils.h"
#include "sema/objc3_parser_sema_handoff_scaffold.h"
#include "sema/objc3_sema_pass_flow_scaffold.h"
#include "sema/objc3_semantic_passes.h"

namespace {

bool IsDiagnosticLess(const std::string &lhs, const std::string &rhs) {
  const DiagSortKey lhs_key = ParseDiagSortKey(lhs);
  const DiagSortKey rhs_key = ParseDiagSortKey(rhs);
  if (lhs_key.line != rhs_key.line) {
    return lhs_key.line < rhs_key.line;
  }
  if (lhs_key.column != rhs_key.column) {
    return lhs_key.column < rhs_key.column;
  }
  if (lhs_key.severity_rank != rhs_key.severity_rank) {
    return lhs_key.severity_rank < rhs_key.severity_rank;
  }
  if (lhs_key.code != rhs_key.code) {
    return lhs_key.code < rhs_key.code;
  }
  if (lhs_key.message != rhs_key.message) {
    return lhs_key.message < rhs_key.message;
  }
  return lhs_key.raw < rhs_key.raw;
}

void CanonicalizePassDiagnostics(std::vector<std::string> &diagnostics) {
  std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsCanonicalPassDiagnostics(const std::vector<std::string> &diagnostics) {
  return std::is_sorted(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsEquivalentSelectorNormalizationSummary(const Objc3SelectorNormalizationSummary &lhs,
                                             const Objc3SelectorNormalizationSummary &rhs) {
  return lhs.methods_total == rhs.methods_total &&
         lhs.normalized_methods == rhs.normalized_methods &&
         lhs.selector_piece_entries == rhs.selector_piece_entries &&
         lhs.selector_parameter_piece_entries == rhs.selector_parameter_piece_entries &&
         lhs.selector_pieceless_methods == rhs.selector_pieceless_methods &&
         lhs.selector_spelling_mismatches == rhs.selector_spelling_mismatches &&
         lhs.selector_arity_mismatches == rhs.selector_arity_mismatches &&
         lhs.selector_parameter_linkage_mismatches == rhs.selector_parameter_linkage_mismatches &&
         lhs.selector_normalization_flag_mismatches == rhs.selector_normalization_flag_mismatches &&
         lhs.selector_missing_keyword_pieces == rhs.selector_missing_keyword_pieces;
}

bool IsEquivalentPropertyAttributeSummary(const Objc3PropertyAttributeSummary &lhs,
                                          const Objc3PropertyAttributeSummary &rhs) {
  return lhs.properties_total == rhs.properties_total &&
         lhs.attribute_entries == rhs.attribute_entries &&
         lhs.readonly_modifiers == rhs.readonly_modifiers &&
         lhs.readwrite_modifiers == rhs.readwrite_modifiers &&
         lhs.atomic_modifiers == rhs.atomic_modifiers &&
         lhs.nonatomic_modifiers == rhs.nonatomic_modifiers &&
         lhs.copy_modifiers == rhs.copy_modifiers &&
         lhs.strong_modifiers == rhs.strong_modifiers &&
         lhs.weak_modifiers == rhs.weak_modifiers &&
         lhs.assign_modifiers == rhs.assign_modifiers &&
         lhs.getter_modifiers == rhs.getter_modifiers &&
         lhs.setter_modifiers == rhs.setter_modifiers &&
         lhs.invalid_attribute_entries == rhs.invalid_attribute_entries &&
         lhs.property_contract_violations == rhs.property_contract_violations;
}

bool IsEquivalentTypeAnnotationSurfaceSummary(const Objc3TypeAnnotationSurfaceSummary &lhs,
                                              const Objc3TypeAnnotationSurfaceSummary &rhs) {
  return lhs.generic_suffix_sites == rhs.generic_suffix_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.nullability_suffix_sites == rhs.nullability_suffix_sites &&
         lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.invalid_generic_suffix_sites == rhs.invalid_generic_suffix_sites &&
         lhs.invalid_pointer_declarator_sites == rhs.invalid_pointer_declarator_sites &&
         lhs.invalid_nullability_suffix_sites == rhs.invalid_nullability_suffix_sites &&
         lhs.invalid_ownership_qualifier_sites == rhs.invalid_ownership_qualifier_sites;
}

bool IsEquivalentLightweightGenericConstraintSummary(
    const Objc3LightweightGenericConstraintSummary &lhs,
    const Objc3LightweightGenericConstraintSummary &rhs) {
  return lhs.generic_constraint_sites == rhs.generic_constraint_sites &&
         lhs.generic_suffix_sites == rhs.generic_suffix_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.terminated_generic_suffix_sites == rhs.terminated_generic_suffix_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_constraint_sites == rhs.normalized_constraint_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNullabilityFlowWarningPrecisionSummary(
    const Objc3NullabilityFlowWarningPrecisionSummary &lhs,
    const Objc3NullabilityFlowWarningPrecisionSummary &rhs) {
  return lhs.nullability_flow_sites == rhs.nullability_flow_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.nullability_suffix_sites == rhs.nullability_suffix_sites &&
         lhs.nullable_suffix_sites == rhs.nullable_suffix_sites &&
         lhs.nonnull_suffix_sites == rhs.nonnull_suffix_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentProtocolQualifiedObjectTypeSummary(
    const Objc3ProtocolQualifiedObjectTypeSummary &lhs,
    const Objc3ProtocolQualifiedObjectTypeSummary &rhs) {
  return lhs.protocol_qualified_object_type_sites == rhs.protocol_qualified_object_type_sites &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.terminated_protocol_composition_sites == rhs.terminated_protocol_composition_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_protocol_composition_sites == rhs.normalized_protocol_composition_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentVarianceBridgeCastSummary(
    const Objc3VarianceBridgeCastSummary &lhs,
    const Objc3VarianceBridgeCastSummary &rhs) {
  return lhs.variance_bridge_cast_sites == rhs.variance_bridge_cast_sites &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentGenericMetadataAbiSummary(
    const Objc3GenericMetadataAbiSummary &lhs,
    const Objc3GenericMetadataAbiSummary &rhs) {
  return lhs.generic_metadata_abi_sites == rhs.generic_metadata_abi_sites &&
         lhs.generic_suffix_sites == rhs.generic_suffix_sites &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentModuleImportGraphSummary(
    const Objc3ModuleImportGraphSummary &lhs,
    const Objc3ModuleImportGraphSummary &rhs) {
  return lhs.module_import_graph_sites == rhs.module_import_graph_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNamespaceCollisionShadowingSummary(
    const Objc3NamespaceCollisionShadowingSummary &lhs,
    const Objc3NamespaceCollisionShadowingSummary &rhs) {
  return lhs.namespace_collision_shadowing_sites ==
             rhs.namespace_collision_shadowing_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentPublicPrivateApiPartitionSummary(
    const Objc3PublicPrivateApiPartitionSummary &lhs,
    const Objc3PublicPrivateApiPartitionSummary &rhs) {
  return lhs.public_private_api_partition_sites ==
             rhs.public_private_api_partition_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentIncrementalModuleCacheInvalidationSummary(
    const Objc3IncrementalModuleCacheInvalidationSummary &lhs,
    const Objc3IncrementalModuleCacheInvalidationSummary &rhs) {
  return lhs.incremental_module_cache_invalidation_sites ==
             rhs.incremental_module_cache_invalidation_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites ==
             rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentCrossModuleConformanceSummary(
    const Objc3CrossModuleConformanceSummary &lhs,
    const Objc3CrossModuleConformanceSummary &rhs) {
  return lhs.cross_module_conformance_sites ==
             rhs.cross_module_conformance_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites ==
             rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentThrowsPropagationSummary(
    const Objc3ThrowsPropagationSummary &lhs,
    const Objc3ThrowsPropagationSummary &rhs) {
  return lhs.throws_propagation_sites == rhs.throws_propagation_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites ==
             rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAsyncContinuationSummary(
    const Objc3AsyncContinuationSummary &lhs,
    const Objc3AsyncContinuationSummary &rhs) {
  return lhs.async_continuation_sites == rhs.async_continuation_sites &&
         lhs.async_keyword_sites == rhs.async_keyword_sites &&
         lhs.async_function_sites == rhs.async_function_sites &&
         lhs.continuation_allocation_sites ==
             rhs.continuation_allocation_sites &&
         lhs.continuation_resume_sites == rhs.continuation_resume_sites &&
         lhs.continuation_suspend_sites == rhs.continuation_suspend_sites &&
         lhs.async_state_machine_sites == rhs.async_state_machine_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentActorIsolationSendabilitySummary(
    const Objc3ActorIsolationSendabilitySummary &lhs,
    const Objc3ActorIsolationSendabilitySummary &rhs) {
  return lhs.actor_isolation_sendability_sites ==
             rhs.actor_isolation_sendability_sites &&
         lhs.actor_isolation_decl_sites == rhs.actor_isolation_decl_sites &&
         lhs.actor_hop_sites == rhs.actor_hop_sites &&
         lhs.sendable_annotation_sites == rhs.sendable_annotation_sites &&
         lhs.non_sendable_crossing_sites == rhs.non_sendable_crossing_sites &&
         lhs.isolation_boundary_sites == rhs.isolation_boundary_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentTaskRuntimeCancellationSummary(
    const Objc3TaskRuntimeCancellationSummary &lhs,
    const Objc3TaskRuntimeCancellationSummary &rhs) {
  return lhs.task_runtime_interop_sites == rhs.task_runtime_interop_sites &&
         lhs.runtime_hook_sites == rhs.runtime_hook_sites &&
         lhs.cancellation_check_sites == rhs.cancellation_check_sites &&
         lhs.cancellation_handler_sites == rhs.cancellation_handler_sites &&
         lhs.suspension_point_sites == rhs.suspension_point_sites &&
         lhs.cancellation_propagation_sites ==
             rhs.cancellation_propagation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentConcurrencyReplayRaceGuardSummary(
    const Objc3ConcurrencyReplayRaceGuardSummary &lhs,
    const Objc3ConcurrencyReplayRaceGuardSummary &rhs) {
  return lhs.concurrency_replay_race_guard_sites ==
             rhs.concurrency_replay_race_guard_sites &&
         lhs.concurrency_replay_sites == rhs.concurrency_replay_sites &&
         lhs.replay_proof_sites == rhs.replay_proof_sites &&
         lhs.race_guard_sites == rhs.race_guard_sites &&
         lhs.task_handoff_sites == rhs.task_handoff_sites &&
         lhs.actor_isolation_sites == rhs.actor_isolation_sites &&
         lhs.deterministic_schedule_sites == rhs.deterministic_schedule_sites &&
         lhs.guard_blocked_sites == rhs.guard_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentUnsafePointerExtensionSummary(
    const Objc3UnsafePointerExtensionSummary &lhs,
    const Objc3UnsafePointerExtensionSummary &rhs) {
  return lhs.unsafe_pointer_extension_sites == rhs.unsafe_pointer_extension_sites &&
         lhs.unsafe_keyword_sites == rhs.unsafe_keyword_sites &&
         lhs.pointer_arithmetic_sites == rhs.pointer_arithmetic_sites &&
         lhs.raw_pointer_type_sites == rhs.raw_pointer_type_sites &&
         lhs.unsafe_operation_sites == rhs.unsafe_operation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentInlineAsmIntrinsicGovernanceSummary(
    const Objc3InlineAsmIntrinsicGovernanceSummary &lhs,
    const Objc3InlineAsmIntrinsicGovernanceSummary &rhs) {
  return lhs.inline_asm_intrinsic_sites == rhs.inline_asm_intrinsic_sites &&
         lhs.inline_asm_sites == rhs.inline_asm_sites &&
         lhs.intrinsic_sites == rhs.intrinsic_sites &&
         lhs.governed_intrinsic_sites == rhs.governed_intrinsic_sites &&
         lhs.privileged_intrinsic_sites == rhs.privileged_intrinsic_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNSErrorBridgingSummary(
    const Objc3NSErrorBridgingSummary &lhs,
    const Objc3NSErrorBridgingSummary &rhs) {
  return lhs.ns_error_bridging_sites == rhs.ns_error_bridging_sites &&
         lhs.ns_error_parameter_sites == rhs.ns_error_parameter_sites &&
         lhs.ns_error_out_parameter_sites == rhs.ns_error_out_parameter_sites &&
         lhs.ns_error_bridge_path_sites == rhs.ns_error_bridge_path_sites &&
         lhs.failable_call_sites == rhs.failable_call_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.bridge_boundary_sites == rhs.bridge_boundary_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentErrorDiagnosticsRecoverySummary(
    const Objc3ErrorDiagnosticsRecoverySummary &lhs,
    const Objc3ErrorDiagnosticsRecoverySummary &rhs) {
  return lhs.error_diagnostics_recovery_sites ==
             rhs.error_diagnostics_recovery_sites &&
         lhs.diagnostic_emit_sites == rhs.diagnostic_emit_sites &&
         lhs.recovery_anchor_sites == rhs.recovery_anchor_sites &&
         lhs.recovery_boundary_sites == rhs.recovery_boundary_sites &&
         lhs.fail_closed_diagnostic_sites == rhs.fail_closed_diagnostic_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentResultLikeLoweringSummary(
    const Objc3ResultLikeLoweringSummary &lhs,
    const Objc3ResultLikeLoweringSummary &rhs) {
  return lhs.result_like_sites == rhs.result_like_sites &&
         lhs.result_success_sites == rhs.result_success_sites &&
         lhs.result_failure_sites == rhs.result_failure_sites &&
         lhs.result_branch_sites == rhs.result_branch_sites &&
         lhs.result_payload_sites == rhs.result_payload_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.branch_merge_sites == rhs.branch_merge_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentUnwindCleanupSummary(
    const Objc3UnwindCleanupSummary &lhs,
    const Objc3UnwindCleanupSummary &rhs) {
  return lhs.unwind_cleanup_sites == rhs.unwind_cleanup_sites &&
         lhs.exceptional_exit_sites == rhs.exceptional_exit_sites &&
         lhs.cleanup_action_sites == rhs.cleanup_action_sites &&
         lhs.cleanup_scope_sites == rhs.cleanup_scope_sites &&
         lhs.cleanup_resume_sites == rhs.cleanup_resume_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.fail_closed_sites == rhs.fail_closed_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAwaitLoweringSuspensionStateSummary(
    const Objc3AwaitLoweringSuspensionStateSummary &lhs,
    const Objc3AwaitLoweringSuspensionStateSummary &rhs) {
  return lhs.await_suspension_sites == rhs.await_suspension_sites &&
         lhs.await_keyword_sites == rhs.await_keyword_sites &&
         lhs.await_suspension_point_sites == rhs.await_suspension_point_sites &&
         lhs.await_resume_sites == rhs.await_resume_sites &&
         lhs.await_state_machine_sites == rhs.await_state_machine_sites &&
         lhs.await_continuation_sites == rhs.await_continuation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentSymbolGraphScopeResolutionSummary(const Objc3SymbolGraphScopeResolutionSummary &lhs,
                                                   const Objc3SymbolGraphScopeResolutionSummary &rhs) {
  return lhs.global_symbol_nodes == rhs.global_symbol_nodes &&
         lhs.function_symbol_nodes == rhs.function_symbol_nodes &&
         lhs.interface_symbol_nodes == rhs.interface_symbol_nodes &&
         lhs.implementation_symbol_nodes == rhs.implementation_symbol_nodes &&
         lhs.interface_property_symbol_nodes == rhs.interface_property_symbol_nodes &&
         lhs.implementation_property_symbol_nodes == rhs.implementation_property_symbol_nodes &&
         lhs.interface_method_symbol_nodes == rhs.interface_method_symbol_nodes &&
         lhs.implementation_method_symbol_nodes == rhs.implementation_method_symbol_nodes &&
         lhs.top_level_scope_symbols == rhs.top_level_scope_symbols &&
         lhs.nested_scope_symbols == rhs.nested_scope_symbols &&
         lhs.scope_frames_total == rhs.scope_frames_total &&
         lhs.implementation_interface_resolution_sites == rhs.implementation_interface_resolution_sites &&
         lhs.implementation_interface_resolution_hits == rhs.implementation_interface_resolution_hits &&
         lhs.implementation_interface_resolution_misses == rhs.implementation_interface_resolution_misses &&
         lhs.method_resolution_sites == rhs.method_resolution_sites &&
         lhs.method_resolution_hits == rhs.method_resolution_hits &&
         lhs.method_resolution_misses == rhs.method_resolution_misses;
}

bool IsEquivalentClassProtocolCategoryLinkingSummary(const Objc3ClassProtocolCategoryLinkingSummary &lhs,
                                                     const Objc3ClassProtocolCategoryLinkingSummary &rhs) {
  return lhs.declared_interfaces == rhs.declared_interfaces &&
         lhs.resolved_interfaces == rhs.resolved_interfaces &&
         lhs.declared_implementations == rhs.declared_implementations &&
         lhs.resolved_implementations == rhs.resolved_implementations &&
         lhs.interface_method_symbols == rhs.interface_method_symbols &&
         lhs.implementation_method_symbols == rhs.implementation_method_symbols &&
         lhs.linked_implementation_symbols == rhs.linked_implementation_symbols &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.protocol_composition_symbols == rhs.protocol_composition_symbols &&
         lhs.category_composition_sites == rhs.category_composition_sites &&
         lhs.category_composition_symbols == rhs.category_composition_symbols &&
         lhs.invalid_protocol_composition_sites == rhs.invalid_protocol_composition_sites;
}

bool IsEquivalentMethodLookupOverrideConflictSummary(const Objc3MethodLookupOverrideConflictSummary &lhs,
                                                     const Objc3MethodLookupOverrideConflictSummary &rhs) {
  return lhs.method_lookup_sites == rhs.method_lookup_sites &&
         lhs.method_lookup_hits == rhs.method_lookup_hits &&
         lhs.method_lookup_misses == rhs.method_lookup_misses &&
         lhs.override_lookup_sites == rhs.override_lookup_sites &&
         lhs.override_lookup_hits == rhs.override_lookup_hits &&
         lhs.override_lookup_misses == rhs.override_lookup_misses &&
         lhs.override_conflicts == rhs.override_conflicts &&
         lhs.unresolved_base_interfaces == rhs.unresolved_base_interfaces;
}

bool IsEquivalentPropertySynthesisIvarBindingSummary(const Objc3PropertySynthesisIvarBindingSummary &lhs,
                                                     const Objc3PropertySynthesisIvarBindingSummary &rhs) {
  return lhs.property_synthesis_sites == rhs.property_synthesis_sites &&
         lhs.property_synthesis_explicit_ivar_bindings == rhs.property_synthesis_explicit_ivar_bindings &&
         lhs.property_synthesis_default_ivar_bindings == rhs.property_synthesis_default_ivar_bindings &&
         lhs.ivar_binding_sites == rhs.ivar_binding_sites &&
         lhs.ivar_binding_resolved == rhs.ivar_binding_resolved &&
         lhs.ivar_binding_missing == rhs.ivar_binding_missing &&
         lhs.ivar_binding_conflicts == rhs.ivar_binding_conflicts;
}

bool IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &lhs,
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &rhs) {
  return lhs.param_type_sites == rhs.param_type_sites &&
         lhs.param_id_spelling_sites == rhs.param_id_spelling_sites &&
         lhs.param_class_spelling_sites == rhs.param_class_spelling_sites &&
         lhs.param_sel_spelling_sites == rhs.param_sel_spelling_sites &&
         lhs.param_instancetype_spelling_sites == rhs.param_instancetype_spelling_sites &&
         lhs.param_object_pointer_type_sites == rhs.param_object_pointer_type_sites &&
         lhs.return_type_sites == rhs.return_type_sites &&
         lhs.return_id_spelling_sites == rhs.return_id_spelling_sites &&
         lhs.return_class_spelling_sites == rhs.return_class_spelling_sites &&
         lhs.return_sel_spelling_sites == rhs.return_sel_spelling_sites &&
         lhs.return_instancetype_spelling_sites == rhs.return_instancetype_spelling_sites &&
         lhs.return_object_pointer_type_sites == rhs.return_object_pointer_type_sites &&
         lhs.property_type_sites == rhs.property_type_sites &&
         lhs.property_id_spelling_sites == rhs.property_id_spelling_sites &&
         lhs.property_class_spelling_sites == rhs.property_class_spelling_sites &&
         lhs.property_sel_spelling_sites == rhs.property_sel_spelling_sites &&
         lhs.property_instancetype_spelling_sites == rhs.property_instancetype_spelling_sites &&
         lhs.property_object_pointer_type_sites == rhs.property_object_pointer_type_sites;
}

// executable-block-source-closure anchor: sema parity for block
// source closure remains summary-based at this freeze point because runnable
// block realization has not started yet.
bool IsEquivalentBlockLiteralCaptureSemanticsSummary(const Objc3BlockLiteralCaptureSemanticsSummary &lhs,
                                                     const Objc3BlockLiteralCaptureSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.block_parameter_entries == rhs.block_parameter_entries &&
         lhs.block_capture_entries == rhs.block_capture_entries &&
         lhs.block_body_statement_entries == rhs.block_body_statement_entries &&
         lhs.block_empty_capture_sites == rhs.block_empty_capture_sites &&
         lhs.block_nondeterministic_capture_sites == rhs.block_nondeterministic_capture_sites &&
         lhs.block_non_normalized_sites == rhs.block_non_normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &lhs,
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.invoke_argument_slots_total == rhs.invoke_argument_slots_total &&
         lhs.capture_word_count_total == rhs.capture_word_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.descriptor_symbolized_sites == rhs.descriptor_symbolized_sites &&
         lhs.invoke_trampoline_symbolized_sites == rhs.invoke_trampoline_symbolized_sites &&
         lhs.missing_invoke_trampoline_sites == rhs.missing_invoke_trampoline_sites &&
         lhs.non_normalized_layout_sites == rhs.non_normalized_layout_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockStorageEscapeSemanticsSummary(
    const Objc3BlockStorageEscapeSemanticsSummary &lhs,
    const Objc3BlockStorageEscapeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.requires_byref_cells_sites == rhs.requires_byref_cells_sites &&
         lhs.escape_analysis_enabled_sites == rhs.escape_analysis_enabled_sites &&
         lhs.escape_to_heap_sites == rhs.escape_to_heap_sites &&
         lhs.escape_profile_normalized_sites == rhs.escape_profile_normalized_sites &&
         lhs.byref_layout_symbolized_sites == rhs.byref_layout_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockCopyDisposeSemanticsSummary(
    const Objc3BlockCopyDisposeSemanticsSummary &lhs,
    const Objc3BlockCopyDisposeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.copy_helper_required_sites == rhs.copy_helper_required_sites &&
         lhs.dispose_helper_required_sites == rhs.dispose_helper_required_sites &&
         lhs.profile_normalized_sites == rhs.profile_normalized_sites &&
         lhs.copy_helper_symbolized_sites == rhs.copy_helper_symbolized_sites &&
         lhs.dispose_helper_symbolized_sites == rhs.dispose_helper_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockDeterminismPerfBaselineSummary(
    const Objc3BlockDeterminismPerfBaselineSummary &lhs,
    const Objc3BlockDeterminismPerfBaselineSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.baseline_weight_total == rhs.baseline_weight_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.deterministic_capture_sites == rhs.deterministic_capture_sites &&
         lhs.heavy_tier_sites == rhs.heavy_tier_sites &&
         lhs.normalized_profile_sites == rhs.normalized_profile_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockDeterminismPerfBaselineSiteMetadata(
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &lhs,
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &rhs) {
  return lhs.parameter_count == rhs.parameter_count &&
         lhs.capture_count == rhs.capture_count &&
         lhs.body_statement_count == rhs.body_statement_count &&
         lhs.baseline_weight == rhs.baseline_weight &&
         lhs.capture_set_deterministic == rhs.capture_set_deterministic &&
         lhs.baseline_profile_is_normalized == rhs.baseline_profile_is_normalized &&
         lhs.baseline_profile == rhs.baseline_profile &&
         lhs.line == rhs.line &&
         lhs.column == rhs.column;
}

bool AreEquivalentBlockDeterminismPerfBaselineSites(
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &lhs,
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(),
                    lhs.end(),
                    rhs.begin(),
                    IsEquivalentBlockDeterminismPerfBaselineSiteMetadata);
}

bool IsEquivalentMessageSendSelectorLoweringSummary(const Objc3MessageSendSelectorLoweringSummary &lhs,
                                                    const Objc3MessageSendSelectorLoweringSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.unary_form_sites == rhs.unary_form_sites &&
         lhs.keyword_form_sites == rhs.keyword_form_sites &&
         lhs.selector_lowering_symbol_sites == rhs.selector_lowering_symbol_sites &&
         lhs.selector_lowering_piece_entries == rhs.selector_lowering_piece_entries &&
         lhs.selector_lowering_argument_piece_entries == rhs.selector_lowering_argument_piece_entries &&
         lhs.selector_lowering_normalized_sites == rhs.selector_lowering_normalized_sites &&
         lhs.selector_lowering_form_mismatch_sites == rhs.selector_lowering_form_mismatch_sites &&
         lhs.selector_lowering_arity_mismatch_sites == rhs.selector_lowering_arity_mismatch_sites &&
         lhs.selector_lowering_symbol_mismatch_sites == rhs.selector_lowering_symbol_mismatch_sites &&
         lhs.selector_lowering_missing_symbol_sites == rhs.selector_lowering_missing_symbol_sites &&
         lhs.selector_lowering_contract_violation_sites == rhs.selector_lowering_contract_violation_sites;
}

bool IsEquivalentDispatchAbiMarshallingSummary(const Objc3DispatchAbiMarshallingSummary &lhs,
                                               const Objc3DispatchAbiMarshallingSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_slots == rhs.receiver_slots &&
         lhs.selector_symbol_slots == rhs.selector_symbol_slots &&
         lhs.argument_slots == rhs.argument_slots &&
         lhs.keyword_argument_slots == rhs.keyword_argument_slots &&
         lhs.unary_argument_slots == rhs.unary_argument_slots &&
         lhs.arity_mismatch_sites == rhs.arity_mismatch_sites &&
         lhs.missing_selector_symbol_sites == rhs.missing_selector_symbol_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNilReceiverSemanticsFoldabilitySummary(
    const Objc3NilReceiverSemanticsFoldabilitySummary &lhs,
    const Objc3NilReceiverSemanticsFoldabilitySummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_nil_literal_sites == rhs.receiver_nil_literal_sites &&
         lhs.nil_receiver_semantics_enabled_sites == rhs.nil_receiver_semantics_enabled_sites &&
         lhs.nil_receiver_foldable_sites == rhs.nil_receiver_foldable_sites &&
         lhs.nil_receiver_runtime_dispatch_required_sites == rhs.nil_receiver_runtime_dispatch_required_sites &&
         lhs.non_nil_receiver_sites == rhs.non_nil_receiver_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentSuperDispatchMethodFamilySummary(const Objc3SuperDispatchMethodFamilySummary &lhs,
                                                  const Objc3SuperDispatchMethodFamilySummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_super_identifier_sites == rhs.receiver_super_identifier_sites &&
         lhs.super_dispatch_enabled_sites == rhs.super_dispatch_enabled_sites &&
         lhs.super_dispatch_requires_class_context_sites == rhs.super_dispatch_requires_class_context_sites &&
         lhs.method_family_init_sites == rhs.method_family_init_sites &&
         lhs.method_family_copy_sites == rhs.method_family_copy_sites &&
         lhs.method_family_mutable_copy_sites == rhs.method_family_mutable_copy_sites &&
         lhs.method_family_new_sites == rhs.method_family_new_sites &&
         lhs.method_family_none_sites == rhs.method_family_none_sites &&
         lhs.method_family_returns_retained_result_sites == rhs.method_family_returns_retained_result_sites &&
         lhs.method_family_returns_related_result_sites == rhs.method_family_returns_related_result_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentRuntimeLinkHostLinkSummary(const Objc3RuntimeLinkHostLinkSummary &lhs,
                                            const Objc3RuntimeLinkHostLinkSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.runtime_link_required_sites == rhs.runtime_link_required_sites &&
         lhs.runtime_link_elided_sites == rhs.runtime_link_elided_sites &&
         lhs.runtime_dispatch_arg_slots == rhs.runtime_dispatch_arg_slots &&
         lhs.runtime_dispatch_declaration_parameter_count == rhs.runtime_dispatch_declaration_parameter_count &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.runtime_dispatch_symbol == rhs.runtime_dispatch_symbol &&
         lhs.default_runtime_dispatch_symbol_binding == rhs.default_runtime_dispatch_symbol_binding;
}

bool IsEquivalentRetainReleaseOperationSummary(const Objc3RetainReleaseOperationSummary &lhs,
                                               const Objc3RetainReleaseOperationSummary &rhs) {
  return lhs.ownership_qualified_sites == rhs.ownership_qualified_sites &&
         lhs.retain_insertion_sites == rhs.retain_insertion_sites &&
         lhs.release_insertion_sites == rhs.release_insertion_sites &&
         lhs.autorelease_insertion_sites == rhs.autorelease_insertion_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentWeakUnownedSemanticsSummary(const Objc3WeakUnownedSemanticsSummary &lhs,
                                             const Objc3WeakUnownedSemanticsSummary &rhs) {
  return lhs.ownership_candidate_sites == rhs.ownership_candidate_sites &&
         lhs.weak_reference_sites == rhs.weak_reference_sites &&
         lhs.unowned_reference_sites == rhs.unowned_reference_sites &&
         lhs.unowned_safe_reference_sites == rhs.unowned_safe_reference_sites &&
         lhs.weak_unowned_conflict_sites == rhs.weak_unowned_conflict_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentArcDiagnosticsFixitSummary(const Objc3ArcDiagnosticsFixitSummary &lhs,
                                            const Objc3ArcDiagnosticsFixitSummary &rhs) {
  return lhs.ownership_arc_diagnostic_candidate_sites == rhs.ownership_arc_diagnostic_candidate_sites &&
         lhs.ownership_arc_fixit_available_sites == rhs.ownership_arc_fixit_available_sites &&
         lhs.ownership_arc_profiled_sites == rhs.ownership_arc_profiled_sites &&
         lhs.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             rhs.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
         lhs.ownership_arc_empty_fixit_hint_sites == rhs.ownership_arc_empty_fixit_hint_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAutoreleasePoolScopeSummary(const Objc3AutoreleasePoolScopeSummary &lhs,
                                             const Objc3AutoreleasePoolScopeSummary &rhs) {
  return lhs.scope_sites == rhs.scope_sites &&
         lhs.scope_symbolized_sites == rhs.scope_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.max_scope_depth == rhs.max_scope_depth;
}

}  // namespace

Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input) {
  Objc3SemaPassManagerResult result;
  const Objc3ParserSemaHandoffScaffold handoff = BuildObjc3ParserSemaHandoffScaffold(input);
  if (input.program == nullptr) {
    return result;
  }
  result.parser_contract_snapshot = handoff.parser_contract_snapshot;
  result.parser_sema_conformance_matrix = handoff.parser_sema_conformance_matrix;
  result.deterministic_parser_sema_conformance_matrix =
      handoff.parser_sema_conformance_matrix.deterministic;
  if (!result.deterministic_parser_sema_conformance_matrix) {
    return result;
  }
  result.parser_sema_conformance_corpus = handoff.parser_sema_conformance_corpus;
  result.deterministic_parser_sema_conformance_corpus =
      handoff.parser_sema_conformance_corpus.deterministic;
  if (!result.deterministic_parser_sema_conformance_corpus) {
    return result;
  }
  result.parser_sema_performance_quality_guardrails =
      handoff.parser_sema_performance_quality_guardrails;
  result.deterministic_parser_sema_performance_quality_guardrails =
      handoff.parser_sema_performance_quality_guardrails.deterministic;
  if (!result.deterministic_parser_sema_performance_quality_guardrails) {
    return result;
  }
  result.parser_sema_cross_lane_integration_sync =
      handoff.parser_sema_cross_lane_integration_sync;
  result.deterministic_parser_sema_cross_lane_integration_sync =
      handoff.parser_sema_cross_lane_integration_sync.deterministic;
  if (!result.deterministic_parser_sema_cross_lane_integration_sync) {
    return result;
  }
  result.parser_sema_docs_runbook_sync = handoff.parser_sema_docs_runbook_sync;
  result.deterministic_parser_sema_docs_runbook_sync =
      handoff.parser_sema_docs_runbook_sync.deterministic;
  if (!result.deterministic_parser_sema_docs_runbook_sync) {
    return result;
  }
  result.parser_sema_release_candidate_replay_dry_run =
      handoff.parser_sema_release_candidate_replay_dry_run;
  result.deterministic_parser_sema_release_candidate_replay_dry_run =
      handoff.parser_sema_release_candidate_replay_dry_run.deterministic;
  if (!result.deterministic_parser_sema_release_candidate_replay_dry_run) {
    return result;
  }
  result.parser_sema_advanced_core_shard1 =
      handoff.parser_sema_advanced_core_shard1;
  result.deterministic_parser_sema_advanced_core_shard1 =
      handoff.parser_sema_advanced_core_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_core_shard1) {
    return result;
  }
  result.parser_sema_advanced_edge_compatibility_shard1 =
      handoff.parser_sema_advanced_edge_compatibility_shard1;
  result.deterministic_parser_sema_advanced_edge_compatibility_shard1 =
      handoff.parser_sema_advanced_edge_compatibility_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_edge_compatibility_shard1) {
    return result;
  }
  result.parser_sema_advanced_diagnostics_shard1 =
      handoff.parser_sema_advanced_diagnostics_shard1;
  result.deterministic_parser_sema_advanced_diagnostics_shard1 =
      handoff.parser_sema_advanced_diagnostics_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_diagnostics_shard1) {
    return result;
  }
  result.parser_sema_advanced_conformance_shard1 =
      handoff.parser_sema_advanced_conformance_shard1;
  result.deterministic_parser_sema_advanced_conformance_shard1 =
      handoff.parser_sema_advanced_conformance_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_conformance_shard1) {
    return result;
  }
  result.parser_sema_advanced_integration_shard1 =
      handoff.parser_sema_advanced_integration_shard1;
  result.deterministic_parser_sema_advanced_integration_shard1 =
      handoff.parser_sema_advanced_integration_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_integration_shard1) {
    return result;
  }
  result.parser_sema_advanced_performance_shard1 =
      handoff.parser_sema_advanced_performance_shard1;
  result.deterministic_parser_sema_advanced_performance_shard1 =
      handoff.parser_sema_advanced_performance_shard1.deterministic;
  if (!result.deterministic_parser_sema_advanced_performance_shard1) {
    return result;
  }
  result.parser_sema_advanced_core_shard2 =
      handoff.parser_sema_advanced_core_shard2;
  result.deterministic_parser_sema_advanced_core_shard2 =
      handoff.parser_sema_advanced_core_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_core_shard2) {
    return result;
  }
  result.parser_sema_advanced_edge_compatibility_shard2 =
      handoff.parser_sema_advanced_edge_compatibility_shard2;
  result.deterministic_parser_sema_advanced_edge_compatibility_shard2 =
      handoff.parser_sema_advanced_edge_compatibility_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_edge_compatibility_shard2) {
    return result;
  }
  result.parser_sema_advanced_diagnostics_shard2 =
      handoff.parser_sema_advanced_diagnostics_shard2;
  result.deterministic_parser_sema_advanced_diagnostics_shard2 =
      handoff.parser_sema_advanced_diagnostics_shard2.deterministic;
  if (!result.deterministic_parser_sema_advanced_diagnostics_shard2) {
    return result;
  }
  result.parser_sema_integration_closeout_signoff =
      handoff.parser_sema_integration_closeout_signoff;
  result.deterministic_parser_sema_integration_closeout_signoff =
      handoff.parser_sema_integration_closeout_signoff.deterministic;
  if (!result.deterministic_parser_sema_integration_closeout_signoff) {
    return result;
  }
  result.deterministic_parser_sema_handoff = handoff.deterministic;
  if (!handoff.deterministic) {
    return result;
  }
  const bool parser_recovery_replay_ready =
      result.parser_sema_conformance_matrix.parser_recovery_replay_ready;
  const bool parser_recovery_replay_case_present =
      result.parser_sema_conformance_corpus.has_recovery_replay_case;
  const bool parser_recovery_replay_case_passed =
      result.parser_sema_conformance_corpus.recovery_replay_case_passed;
  const bool parser_recovery_replay_contract_satisfied =
      parser_recovery_replay_ready &&
      parser_recovery_replay_case_present &&
      parser_recovery_replay_case_passed &&
      result.parser_sema_conformance_corpus.required_case_count > 0u &&
      result.parser_sema_conformance_corpus.passed_case_count ==
          result.parser_sema_conformance_corpus.required_case_count &&
      result.parser_sema_conformance_corpus.failed_case_count == 0u;
  std::ostringstream recovery_replay_key_stream;
  recovery_replay_key_stream
      << "sema-pass-recovery:v1:"
      << "matrix_recovery_replay_ready="
      << (parser_recovery_replay_ready ? "true" : "false")
      << ";corpus_recovery_replay_case_present="
      << (parser_recovery_replay_case_present ? "true" : "false")
      << ";corpus_recovery_replay_case_passed="
      << (parser_recovery_replay_case_passed ? "true" : "false")
      << ";corpus_required_case_count="
      << result.parser_sema_conformance_corpus.required_case_count
      << ";corpus_passed_case_count="
      << result.parser_sema_conformance_corpus.passed_case_count
      << ";corpus_failed_case_count="
      << result.parser_sema_conformance_corpus.failed_case_count;
  const std::string recovery_replay_key = recovery_replay_key_stream.str();
  const bool recovery_replay_key_deterministic =
      result.deterministic_parser_sema_conformance_matrix &&
      result.deterministic_parser_sema_conformance_corpus &&
      !recovery_replay_key.empty();
  const bool recovery_determinism_hardening_satisfied =
      parser_recovery_replay_contract_satisfied &&
      recovery_replay_key_deterministic;

  result.executed = true;
  result.sema_pass_flow_summary.language_profile = input.language_profile;
  result.sema_pass_flow_summary.migration_legacy_literal_total = input.migration_hints.legacy_total();
  bool deterministic_semantic_diagnostics = handoff.deterministic;
  bool diagnostics_canonicalized = true;
  bool diagnostics_accounting_consistent = true;
  bool diagnostics_bus_publish_consistent = true;
  bool pass_order_matches_contract = true;
  std::size_t pass_iteration_index = 0u;
  std::size_t expected_diagnostics_size = 0u;
  for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {
    pass_order_matches_contract =
        pass_order_matches_contract &&
        pass_iteration_index < kObjc3SemaPassOrder.size() &&
        kObjc3SemaPassOrder[pass_iteration_index] == pass;
    ++pass_iteration_index;

    const std::size_t pass_index = static_cast<std::size_t>(pass);
    MarkObjc3SemaPassExecuted(result.sema_pass_flow_summary, pass);

    std::vector<std::string> pass_diagnostics;
    if (pass == Objc3SemaPassId::BuildIntegrationSurface) {
      result.integration_surface =
          // block-source-model-completion anchor: the semantic
          // integration surface receives the explicit source-only
          // block-literal admission bit so lane-A can publish the completed
          // source model without widening runnable block support.
          // block-source-storage-annotation anchor: byref/helper/
          // escape-shape source annotations remain parser-owned at this stage
          // and ride the same source-only admission path without claiming that
          // runnable block lowering or helper emission already exists.
          // block-runtime-semantic-rules freeze anchor: the pass
          // manager freezes this exact source-only admission boundary as the
          // current semantic contract while native emit paths still fail
          // closed on runnable block semantics.
          // capture-legality/escape/invocation implementation
          // anchor: lane-B keeps this same admission boundary but now expects
          // ValidateBodies to enforce live capture legality and local callable
          // block invocation typing inside the source-only path.
          // byref/copy-dispose/object-ownership anchor: the pass
          // manager still routes blocks through this source-only admission
          // path, but ValidateBodies now also owns
          // ownership-sensitive helper eligibility and non-owning mutation
          // rejection.
          // block-lowering-ABI/artifact-boundary freeze anchor:
          // this pass manager hands lane-C one deterministic set of capture,
          // invoke, storage-escape, and copy-dispose lowering surfaces, but
          // it still does not materialize emitted block-object records,
          // invoke-thunk bodies, byref cells, or helper function bodies.
          // byref-cell/copy-helper/dispose-helper anchor: lane-C now
          // consumes the same pass-manager handoff while ValidateBodies
          // populates the live byref layout and runtime helper fields needed
          // for emitted local nonescaping block lowering.
          // escaping-block runtime-hook anchor: the same
          // deterministic pass-manager handoff now also carries truthful
          // escape-to-heap source facts so lane-C can widen only the
          // readonly-scalar escaping slice through private runtime promotion
          // and invoke hooks.
          // block-runtime API/object-layout freeze anchor: this
          // pass-manager handoff remains the only semantic input to the
          // frozen private block-runtime ABI boundary; no parallel runtime
          // re-derivation path is permitted.
          // block-runtime allocation/copy-dispose/invoke anchor:
          // lane-D widens runtime execution for promoted block records without
          // widening the source-level sema contract that this handoff
          // publishes.
          // byref-forwarding/heap-promotion/ownership-interop anchor:
          // the same sema handoff now feeds runtime-owned forwarding-cell
          // promotion for escaping pointer-capture blocks without changing the
          // source-facing capture legality contract.
          // runnable-block-runtime gate anchor: lane-E now freezes
          // this exact source+sema handoff together with the C004 and D003
          // lowering/runtime proofs so runnable block closeout cannot drift
          // back to metadata-only evidence.
          // runnable-block execution-matrix anchor: lane-E now
          // closes the block-runtime tranche by consuming this same source+sema handoff together
          // with integrated executable block fixtures instead of widening the
          // semantic surface any further.
          BuildSemanticIntegrationSurface(
              *input.program,
              input.validation_options.allow_source_only_block_literals,
              input.validation_options.allow_source_only_defer_statements,
              input.validation_options.allow_source_only_error_runtime_surface,
              input.validation_options.arc_mode_enabled,
              pass_diagnostics);
    } else if (pass == Objc3SemaPassId::ValidateBodies) {
      ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);
      RefreshSemanticIntegrationSurfaceAfterBodyValidation(*input.program,
                                                           result.integration_surface);
    } else {
      ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);
    }
    CanonicalizePassDiagnostics(pass_diagnostics);
    const bool pass_diagnostics_canonical = IsCanonicalPassDiagnostics(pass_diagnostics);
    diagnostics_canonicalized = diagnostics_canonicalized && pass_diagnostics_canonical;
    deterministic_semantic_diagnostics =
        deterministic_semantic_diagnostics && pass_diagnostics_canonical;

    const std::size_t diagnostics_bus_count_before_publish = input.diagnostics_bus.Count();
    result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());
    expected_diagnostics_size += pass_diagnostics.size();
    diagnostics_accounting_consistent =
        diagnostics_accounting_consistent && result.diagnostics.size() == expected_diagnostics_size;
    input.diagnostics_bus.PublishBatch(pass_diagnostics);
    const std::size_t diagnostics_bus_count_after_publish = input.diagnostics_bus.Count();
    const bool pass_bus_publish_consistent = input.diagnostics_bus.diagnostics == nullptr ||
                                             diagnostics_bus_count_after_publish ==
                                                 diagnostics_bus_count_before_publish + pass_diagnostics.size();
    diagnostics_bus_publish_consistent = diagnostics_bus_publish_consistent && pass_bus_publish_consistent;
    result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();
    result.diagnostics_emitted_by_pass[pass_index] = pass_diagnostics.size();
  }
  const std::size_t diagnostics_emitted_total = std::accumulate(
      result.diagnostics_emitted_by_pass.begin(),
      result.diagnostics_emitted_by_pass.end(),
      static_cast<std::size_t>(0u));
  const bool diagnostics_emission_totals_consistent = diagnostics_emitted_total == result.diagnostics.size();
  const bool diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  const bool diagnostics_hardening_has_required_budgets =
      diagnostics_bus_publish_consistent && diagnostics_emission_totals_consistent &&
      diagnostics_after_pass_monotonic;
  result.diagnostics_accounting_consistent = diagnostics_accounting_consistent;
  result.diagnostics_bus_publish_consistent = diagnostics_bus_publish_consistent;
  result.diagnostics_canonicalized = diagnostics_canonicalized;
  result.diagnostics_hardening_satisfied =
      result.diagnostics_accounting_consistent &&
      result.diagnostics_bus_publish_consistent &&
      result.diagnostics_canonicalized &&
      diagnostics_hardening_has_required_budgets;
  result.deterministic_semantic_diagnostics =
      deterministic_semantic_diagnostics &&
      result.diagnostics_hardening_satisfied;
  if (!result.deterministic_semantic_diagnostics) {
    return result;
  }
  result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);
  result.deterministic_type_metadata_handoff =
      IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);
  result.deterministic_interface_implementation_handoff =
      result.type_metadata_handoff.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.declared_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.declared_interfaces &&
      result.integration_surface.interface_implementation_summary.declared_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.declared_implementations &&
      result.integration_surface.interface_implementation_summary.resolved_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_interfaces &&
      result.integration_surface.interface_implementation_summary.resolved_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_implementations &&
      result.integration_surface.interface_implementation_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.integration_surface.interface_implementation_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.integration_surface.interface_implementation_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols;
  result.deterministic_protocol_category_composition_handoff =
      result.type_metadata_handoff.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;
  result.deterministic_class_protocol_category_linking_handoff =
      result.type_metadata_handoff.class_protocol_category_linking_summary.deterministic &&
      result.integration_surface.class_protocol_category_linking_summary.deterministic &&
      IsEquivalentClassProtocolCategoryLinkingSummary(
          result.integration_surface.class_protocol_category_linking_summary,
          result.type_metadata_handoff.class_protocol_category_linking_summary) &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_interfaces <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_interfaces &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_implementations <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_implementations &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.total_composition_sites();
  result.selector_normalization_summary = result.integration_surface.selector_normalization_summary;
  result.deterministic_selector_normalization_handoff =
      result.type_metadata_handoff.selector_normalization_summary.deterministic &&
      result.integration_surface.selector_normalization_summary.deterministic &&
      IsEquivalentSelectorNormalizationSummary(result.integration_surface.selector_normalization_summary,
                                               result.type_metadata_handoff.selector_normalization_summary) &&
      result.type_metadata_handoff.selector_normalization_summary.normalized_methods <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total &&
      result.type_metadata_handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          result.type_metadata_handoff.selector_normalization_summary.selector_piece_entries &&
      result.type_metadata_handoff.selector_normalization_summary.contract_violations() <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total;
  result.property_attribute_summary = result.integration_surface.property_attribute_summary;
  result.deterministic_property_attribute_handoff =
      result.type_metadata_handoff.property_attribute_summary.deterministic &&
      result.integration_surface.property_attribute_summary.deterministic &&
      IsEquivalentPropertyAttributeSummary(result.integration_surface.property_attribute_summary,
                                           result.type_metadata_handoff.property_attribute_summary) &&
      result.type_metadata_handoff.property_attribute_summary.getter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total &&
      result.type_metadata_handoff.property_attribute_summary.setter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total;
  result.type_annotation_surface_summary = result.integration_surface.type_annotation_surface_summary;
  result.deterministic_type_annotation_surface_handoff =
      result.type_metadata_handoff.type_annotation_surface_summary.deterministic &&
      result.integration_surface.type_annotation_surface_summary.deterministic &&
      IsEquivalentTypeAnnotationSurfaceSummary(result.integration_surface.type_annotation_surface_summary,
                                               result.type_metadata_handoff.type_annotation_surface_summary) &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.generic_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.type_metadata_handoff.type_annotation_surface_summary.total_type_annotation_sites();
  result.lightweight_generic_constraint_summary =
      result.integration_surface.lightweight_generic_constraint_summary;
  result.deterministic_lightweight_generic_constraint_handoff =
      result.type_metadata_handoff.lightweight_generic_constraint_summary.deterministic &&
      result.integration_surface.lightweight_generic_constraint_summary.deterministic &&
      IsEquivalentLightweightGenericConstraintSummary(
          result.integration_surface.lightweight_generic_constraint_summary,
          result.type_metadata_handoff.lightweight_generic_constraint_summary) &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_suffix_sites &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_constraint_sites &&
      result.type_metadata_handoff.lightweight_generic_constraint_summary.contract_violation_sites <=
          result.type_metadata_handoff.lightweight_generic_constraint_summary.generic_constraint_sites;
  result.nullability_flow_warning_precision_summary =
      result.integration_surface.nullability_flow_warning_precision_summary;
  result.deterministic_nullability_flow_warning_precision_handoff =
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.deterministic &&
      result.integration_surface.nullability_flow_warning_precision_summary.deterministic &&
      IsEquivalentNullabilityFlowWarningPrecisionSummary(
          result.integration_surface.nullability_flow_warning_precision_summary,
          result.type_metadata_handoff.nullability_flow_warning_precision_summary) &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.normalized_sites <=
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.contract_violation_sites <=
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          result.type_metadata_handoff.nullability_flow_warning_precision_summary.nullable_suffix_sites +
              result.type_metadata_handoff.nullability_flow_warning_precision_summary.nonnull_suffix_sites;
  result.protocol_qualified_object_type_summary =
      result.integration_surface.protocol_qualified_object_type_summary;
  result.deterministic_protocol_qualified_object_type_handoff =
      result.type_metadata_handoff.protocol_qualified_object_type_summary.deterministic &&
      result.integration_surface.protocol_qualified_object_type_summary.deterministic &&
      IsEquivalentProtocolQualifiedObjectTypeSummary(
          result.integration_surface.protocol_qualified_object_type_summary,
          result.type_metadata_handoff.protocol_qualified_object_type_summary) &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.terminated_protocol_composition_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_composition_sites &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.normalized_protocol_composition_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
      result.type_metadata_handoff.protocol_qualified_object_type_summary.contract_violation_sites <=
          result.type_metadata_handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites;
  result.variance_bridge_cast_summary = result.integration_surface.variance_bridge_cast_summary;
  result.deterministic_variance_bridge_cast_handoff =
      result.type_metadata_handoff.variance_bridge_cast_summary.deterministic &&
      result.integration_surface.variance_bridge_cast_summary.deterministic &&
      IsEquivalentVarianceBridgeCastSummary(result.integration_surface.variance_bridge_cast_summary,
                                            result.type_metadata_handoff.variance_bridge_cast_summary) &&
      result.type_metadata_handoff.variance_bridge_cast_summary.protocol_composition_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.type_metadata_handoff.variance_bridge_cast_summary.normalized_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.type_metadata_handoff.variance_bridge_cast_summary.contract_violation_sites <=
          result.type_metadata_handoff.variance_bridge_cast_summary.variance_bridge_cast_sites;
  result.generic_metadata_abi_summary = result.integration_surface.generic_metadata_abi_summary;
  result.deterministic_generic_metadata_abi_handoff =
      result.type_metadata_handoff.generic_metadata_abi_summary.deterministic &&
      result.integration_surface.generic_metadata_abi_summary.deterministic &&
      IsEquivalentGenericMetadataAbiSummary(result.integration_surface.generic_metadata_abi_summary,
                                            result.type_metadata_handoff.generic_metadata_abi_summary) &&
      result.type_metadata_handoff.generic_metadata_abi_summary.generic_suffix_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.protocol_composition_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.normalized_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.type_metadata_handoff.generic_metadata_abi_summary.contract_violation_sites <=
          result.type_metadata_handoff.generic_metadata_abi_summary.generic_metadata_abi_sites;
  result.module_import_graph_summary = result.integration_surface.module_import_graph_summary;
  result.deterministic_module_import_graph_handoff =
      result.type_metadata_handoff.module_import_graph_summary.deterministic &&
      result.integration_surface.module_import_graph_summary.deterministic &&
      IsEquivalentModuleImportGraphSummary(result.integration_surface.module_import_graph_summary,
                                           result.type_metadata_handoff.module_import_graph_summary) &&
      result.type_metadata_handoff.module_import_graph_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.namespace_segment_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.normalized_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.contract_violation_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites;
  result.namespace_collision_shadowing_summary =
      result.integration_surface.namespace_collision_shadowing_summary;
  result.deterministic_namespace_collision_shadowing_handoff =
      result.type_metadata_handoff.namespace_collision_shadowing_summary.deterministic &&
      result.integration_surface.namespace_collision_shadowing_summary.deterministic &&
      IsEquivalentNamespaceCollisionShadowingSummary(
          result.integration_surface.namespace_collision_shadowing_summary,
          result.type_metadata_handoff.namespace_collision_shadowing_summary) &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .normalized_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites;
  result.public_private_api_partition_summary =
      result.integration_surface.public_private_api_partition_summary;
  result.deterministic_public_private_api_partition_handoff =
      result.type_metadata_handoff.public_private_api_partition_summary
              .deterministic &&
      result.integration_surface.public_private_api_partition_summary
              .deterministic &&
      IsEquivalentPublicPrivateApiPartitionSummary(
          result.integration_surface.public_private_api_partition_summary,
          result.type_metadata_handoff.public_private_api_partition_summary) &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .normalized_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites;
  result.incremental_module_cache_invalidation_summary =
      result.integration_surface.incremental_module_cache_invalidation_summary;
  result.deterministic_incremental_module_cache_invalidation_handoff =
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .deterministic &&
      result.integration_surface.incremental_module_cache_invalidation_summary
              .deterministic &&
      IsEquivalentIncrementalModuleCacheInvalidationSummary(
          result.integration_surface.incremental_module_cache_invalidation_summary,
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary) &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .normalized_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites;
  result.cross_module_conformance_summary =
      result.integration_surface.cross_module_conformance_summary;
  result.deterministic_cross_module_conformance_handoff =
      result.type_metadata_handoff.cross_module_conformance_summary
              .deterministic &&
      result.integration_surface.cross_module_conformance_summary.deterministic &&
      IsEquivalentCrossModuleConformanceSummary(
          result.integration_surface.cross_module_conformance_summary,
          result.type_metadata_handoff.cross_module_conformance_summary) &&
      result.type_metadata_handoff.cross_module_conformance_summary.namespace_segment_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.normalized_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.cache_invalidation_candidate_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary
              .normalized_sites +
              result.type_metadata_handoff.cross_module_conformance_summary
                  .cache_invalidation_candidate_sites ==
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.contract_violation_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites;
  result.throws_propagation_summary =
      result.integration_surface.throws_propagation_summary;
  result.deterministic_throws_propagation_handoff =
      result.type_metadata_handoff.throws_propagation_summary.deterministic &&
      result.integration_surface.throws_propagation_summary.deterministic &&
      IsEquivalentThrowsPropagationSummary(
          result.integration_surface.throws_propagation_summary,
          result.type_metadata_handoff.throws_propagation_summary) &&
      result.type_metadata_handoff.throws_propagation_summary.namespace_segment_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.normalized_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.cache_invalidation_candidate_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.normalized_sites +
              result.type_metadata_handoff.throws_propagation_summary
                  .cache_invalidation_candidate_sites ==
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.contract_violation_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites;
  result.actor_isolation_sendability_summary =
      result.integration_surface.actor_isolation_sendability_summary;
  result.deterministic_actor_isolation_sendability_handoff =
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .deterministic &&
      result.integration_surface.actor_isolation_sendability_summary
          .deterministic &&
      IsEquivalentActorIsolationSendabilitySummary(
          result.integration_surface.actor_isolation_sendability_summary,
          result.type_metadata_handoff.actor_isolation_sendability_summary) &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_decl_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_hop_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .sendable_annotation_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .non_sendable_crossing_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .isolation_boundary_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .normalized_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .normalized_sites +
              result.type_metadata_handoff.actor_isolation_sendability_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites;
  result.task_runtime_cancellation_summary =
      result.integration_surface.task_runtime_cancellation_summary;
  result.deterministic_task_runtime_cancellation_handoff =
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .deterministic &&
      result.integration_surface.task_runtime_cancellation_summary
          .deterministic &&
      IsEquivalentTaskRuntimeCancellationSummary(
          result.integration_surface.task_runtime_cancellation_summary,
          result.type_metadata_handoff.task_runtime_cancellation_summary) &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .runtime_hook_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
