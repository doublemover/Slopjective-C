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

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

void AppendMigrationAssistDiagnostics(const Objc3SemaPassManagerInput &input, std::vector<std::string> &diagnostics) {
  // M259-B001 runnable-core compatibility guard anchor: migration assist only
  // becomes a live fail-closed diagnostic surface in canonical mode.
  if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {
    return;
  }

  const auto append_for_literal = [&diagnostics](std::size_t count,
                                                 unsigned column,
                                                 const char *legacy_literal,
                                                 const char *canonical_literal) {
    if (count == 0) {
      return;
    }
    diagnostics.push_back(
        MakeDiag(1u,
                 column,
                 "O3S216",
                 "migration assist requires canonical literal '" + std::string(canonical_literal) +
                     "' instead of legacy '" + legacy_literal + "' (" + std::to_string(count) +
                     " occurrence(s))"));
  };

  append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");
  append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");
  append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");
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

// M261-A001 executable-block-source-closure anchor: sema parity for block
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

bool IsEquivalentRuntimeShimHostLinkSummary(const Objc3RuntimeShimHostLinkSummary &lhs,
                                            const Objc3RuntimeShimHostLinkSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.runtime_shim_required_sites == rhs.runtime_shim_required_sites &&
         lhs.runtime_shim_elided_sites == rhs.runtime_shim_elided_sites &&
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
  result.sema_pass_flow_summary.compatibility_mode = input.compatibility_mode;
  result.sema_pass_flow_summary.migration_assist_enabled = input.migration_assist;
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
          // M261-A002 block-source-model-completion anchor: the semantic
          // integration surface receives the explicit source-only
          // block-literal admission bit so lane-A can publish the completed
          // source model without widening runnable block support.
          // M261-A003 block-source-storage-annotation anchor: byref/helper/
          // escape-shape source annotations remain parser-owned at this stage
          // and ride the same source-only admission path without claiming that
          // runnable block lowering or helper emission already exists.
          // M261-B001 block-runtime-semantic-rules freeze anchor: the pass
          // manager freezes this exact source-only admission boundary as the
          // current semantic contract while native emit paths still fail
          // closed on runnable block semantics.
          // M261-B002 capture-legality/escape/invocation implementation
          // anchor: lane-B keeps this same admission boundary but now expects
          // ValidateBodies to enforce live capture legality and local callable
          // block invocation typing inside the source-only path.
          // M261-B003 byref/copy-dispose/object-ownership anchor: the pass
          // manager still routes blocks through this source-only admission
          // path, but ValidateBodies now also owns
          // ownership-sensitive helper eligibility and non-owning mutation
          // rejection.
          // M261-C001 block-lowering-ABI/artifact-boundary freeze anchor:
          // this pass manager hands lane-C one deterministic set of capture,
          // invoke, storage-escape, and copy-dispose lowering surfaces, but
          // it still does not materialize emitted block-object records,
          // invoke-thunk bodies, byref cells, or helper function bodies.
          // M261-C003 byref-cell/copy-helper/dispose-helper anchor: lane-C now
          // consumes the same pass-manager handoff while ValidateBodies
          // populates the live byref layout and runtime helper fields needed
          // for emitted local nonescaping block lowering.
          // M261-C004 escaping-block runtime-hook anchor: the same
          // deterministic pass-manager handoff now also carries truthful
          // escape-to-heap source facts so lane-C can widen only the
          // readonly-scalar escaping slice through private runtime promotion
          // and invoke hooks.
          // M261-D001 block-runtime API/object-layout freeze anchor: this
          // pass-manager handoff remains the only semantic input to the
          // frozen private block-runtime ABI boundary; no parallel runtime
          // re-derivation path is permitted.
          // M261-D002 block-runtime allocation/copy-dispose/invoke anchor:
          // lane-D widens runtime execution for promoted block records without
          // widening the source-level sema contract that this handoff
          // publishes.
          // M261-D003 byref-forwarding/heap-promotion/ownership-interop anchor:
          // the same sema handoff now feeds runtime-owned forwarding-cell
          // promotion for escaping pointer-capture blocks without changing the
          // source-facing capture legality contract.
          // M261-E001 runnable-block-runtime gate anchor: lane-E now freezes
          // this exact source+sema handoff together with the C004 and D003
          // lowering/runtime proofs so runnable block closeout cannot drift
          // back to metadata-only evidence.
          // M261-E002 runnable-block execution-matrix anchor: lane-E now
          // closes M261 by consuming this same source+sema handoff together
          // with integrated executable block fixtures instead of widening the
          // semantic surface any further.
          BuildSemanticIntegrationSurface(
              *input.program,
              input.compatibility_mode == Objc3SemaCompatibilityMode::Legacy,
              input.migration_assist,
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
      AppendMigrationAssistDiagnostics(input, pass_diagnostics);
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
              .cancellation_check_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .suspension_point_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_check_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_handler_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .normalized_sites +
              result.type_metadata_handoff.task_runtime_cancellation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites;
  result.concurrency_replay_race_guard_summary =
      result.integration_surface.concurrency_replay_race_guard_summary;
  result.deterministic_concurrency_replay_race_guard_handoff =
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic &&
      result.integration_surface.concurrency_replay_race_guard_summary
          .deterministic &&
      IsEquivalentConcurrencyReplayRaceGuardSummary(
          result.integration_surface.concurrency_replay_race_guard_summary,
          result.type_metadata_handoff
              .concurrency_replay_race_guard_summary) &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .replay_proof_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .race_guard_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .task_handoff_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .actor_isolation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .guard_blocked_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              result.type_metadata_handoff
                  .concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          result.type_metadata_handoff.concurrency_replay_race_guard_summary
              .concurrency_replay_sites;
  result.unsafe_pointer_extension_summary =
      result.integration_surface.unsafe_pointer_extension_summary;
  result.deterministic_unsafe_pointer_extension_handoff =
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .deterministic &&
      result.integration_surface.unsafe_pointer_extension_summary
          .deterministic &&
      IsEquivalentUnsafePointerExtensionSummary(
          result.integration_surface.unsafe_pointer_extension_summary,
          result.type_metadata_handoff.unsafe_pointer_extension_summary) &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_keyword_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .raw_pointer_type_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites +
              result.type_metadata_handoff.unsafe_pointer_extension_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites;
  result.inline_asm_intrinsic_governance_summary =
      result.integration_surface.inline_asm_intrinsic_governance_summary;
  result.deterministic_inline_asm_intrinsic_governance_handoff =
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .deterministic &&
      result.integration_surface.inline_asm_intrinsic_governance_summary
          .deterministic &&
      IsEquivalentInlineAsmIntrinsicGovernanceSummary(
          result.integration_surface.inline_asm_intrinsic_governance_summary,
          result.type_metadata_handoff
              .inline_asm_intrinsic_governance_summary) &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.type_metadata_handoff.throws_propagation_summary
              .cache_invalidation_candidate_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .unsafe_operation_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.type_metadata_handoff.unsafe_pointer_extension_summary
              .normalized_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites >=
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
                  .intrinsic_sites -
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .normalized_sites +
              result.type_metadata_handoff
                  .inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites;
  result.ns_error_bridging_summary =
      result.integration_surface.ns_error_bridging_summary;
  result.deterministic_ns_error_bridging_handoff =
      result.type_metadata_handoff.ns_error_bridging_summary.deterministic &&
      result.integration_surface.ns_error_bridging_summary.deterministic &&
      IsEquivalentNSErrorBridgingSummary(
          result.integration_surface.ns_error_bridging_summary,
          result.type_metadata_handoff.ns_error_bridging_summary) &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_out_parameter_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridge_path_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.failable_call_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .bridge_boundary_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary.normalized_sites +
              result.type_metadata_handoff.ns_error_bridging_summary
                  .bridge_boundary_sites ==
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites &&
      result.type_metadata_handoff.ns_error_bridging_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.ns_error_bridging_summary
              .ns_error_bridging_sites;
  result.error_diagnostics_recovery_summary =
      result.integration_surface.error_diagnostics_recovery_summary;
  result.deterministic_error_diagnostics_recovery_handoff =
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .deterministic &&
      result.integration_surface.error_diagnostics_recovery_summary
          .deterministic &&
      IsEquivalentErrorDiagnosticsRecoverySummary(
          result.integration_surface.error_diagnostics_recovery_summary,
          result.type_metadata_handoff.error_diagnostics_recovery_summary) &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_anchor_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .recovery_boundary_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .diagnostic_emit_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.type_metadata_handoff.error_diagnostics_recovery_summary
              .normalized_sites +
              result.type_metadata_handoff.error_diagnostics_recovery_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites;
  result.result_like_lowering_summary =
      result.integration_surface.result_like_lowering_summary;
  result.deterministic_result_like_lowering_handoff =
      result.type_metadata_handoff.result_like_lowering_summary.deterministic &&
      result.integration_surface.result_like_lowering_summary.deterministic &&
      IsEquivalentResultLikeLoweringSummary(
          result.integration_surface.result_like_lowering_summary,
          result.type_metadata_handoff.result_like_lowering_summary) &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_failure_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_branch_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_payload_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary.normalized_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .branch_merge_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .result_success_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .result_failure_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites &&
      result.type_metadata_handoff.result_like_lowering_summary
              .normalized_sites +
              result.type_metadata_handoff.result_like_lowering_summary
                  .branch_merge_sites ==
          result.type_metadata_handoff.result_like_lowering_summary
              .result_like_sites;
  result.unwind_cleanup_summary = result.integration_surface.unwind_cleanup_summary;
  result.deterministic_unwind_cleanup_handoff =
      result.type_metadata_handoff.unwind_cleanup_summary.deterministic &&
      result.integration_surface.unwind_cleanup_summary.deterministic &&
      IsEquivalentUnwindCleanupSummary(
          result.integration_surface.unwind_cleanup_summary,
          result.type_metadata_handoff.unwind_cleanup_summary) &&
      result.type_metadata_handoff.unwind_cleanup_summary.exceptional_exit_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_action_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_scope_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.cleanup_resume_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.fail_closed_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.type_metadata_handoff.unwind_cleanup_summary.normalized_sites +
              result.type_metadata_handoff.unwind_cleanup_summary
                  .fail_closed_sites ==
          result.type_metadata_handoff.unwind_cleanup_summary.unwind_cleanup_sites;
  result.async_continuation_summary =
      result.integration_surface.async_continuation_summary;
  result.deterministic_async_continuation_handoff =
      result.type_metadata_handoff.async_continuation_summary.deterministic &&
      result.integration_surface.async_continuation_summary.deterministic &&
      IsEquivalentAsyncContinuationSummary(
          result.integration_surface.async_continuation_summary,
          result.type_metadata_handoff.async_continuation_summary) &&
      result.type_metadata_handoff.async_continuation_summary
              .async_keyword_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_function_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_allocation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_resume_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .continuation_suspend_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .async_state_machine_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites &&
      result.type_metadata_handoff.async_continuation_summary.normalized_sites +
              result.type_metadata_handoff.async_continuation_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.async_continuation_summary
              .async_continuation_sites;
  result.await_lowering_suspension_state_lowering_summary =
      result.integration_surface.await_lowering_suspension_state_lowering_summary;
  result.deterministic_await_lowering_suspension_state_lowering_handoff =
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      result.integration_surface
              .await_lowering_suspension_state_lowering_summary.deterministic &&
      IsEquivalentAwaitLoweringSuspensionStateSummary(
          result.integration_surface
              .await_lowering_suspension_state_lowering_summary,
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary) &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_keyword_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.await_resume_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_continuation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .contract_violation_sites <=
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary.normalized_sites +
              result.type_metadata_handoff
                  .await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff
              .await_lowering_suspension_state_lowering_summary
              .await_suspension_sites;
  result.symbol_graph_scope_resolution_summary = result.integration_surface.symbol_graph_scope_resolution_summary;
  result.deterministic_symbol_graph_scope_resolution_handoff =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.deterministic &&
      result.integration_surface.symbol_graph_scope_resolution_summary.deterministic &&
      IsEquivalentSymbolGraphScopeResolutionSummary(result.integration_surface.symbol_graph_scope_resolution_summary,
                                                    result.type_metadata_handoff.symbol_graph_scope_resolution_summary) &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total();
  result.method_lookup_override_conflict_summary =
      result.integration_surface.method_lookup_override_conflict_summary;
  result.deterministic_method_lookup_override_conflict_handoff =
      result.type_metadata_handoff.method_lookup_override_conflict_summary.deterministic &&
      result.integration_surface.method_lookup_override_conflict_summary.deterministic &&
      IsEquivalentMethodLookupOverrideConflictSummary(
          result.integration_surface.method_lookup_override_conflict_summary,
          result.type_metadata_handoff.method_lookup_override_conflict_summary) &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_conflicts <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits;
  result.property_synthesis_ivar_binding_summary =
      result.integration_surface.property_synthesis_ivar_binding_summary;
  result.deterministic_property_synthesis_ivar_binding_handoff =
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.deterministic &&
      result.integration_surface.property_synthesis_ivar_binding_summary.deterministic &&
      IsEquivalentPropertySynthesisIvarBindingSummary(
          result.integration_surface.property_synthesis_ivar_binding_summary,
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary) &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites;
  result.id_class_sel_object_pointer_type_checking_summary =
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary;
  result.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
          result.integration_surface.id_class_sel_object_pointer_type_checking_summary,
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary) &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites;
  result.block_literal_capture_semantics_summary =
      result.integration_surface.block_literal_capture_semantics_summary;
  result.deterministic_block_literal_capture_semantics_handoff =
      result.type_metadata_handoff.block_literal_capture_semantics_summary.deterministic &&
      result.integration_surface.block_literal_capture_semantics_summary.deterministic &&
      IsEquivalentBlockLiteralCaptureSemanticsSummary(
          result.integration_surface.block_literal_capture_semantics_summary,
          result.type_metadata_handoff.block_literal_capture_semantics_summary) &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_empty_capture_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_non_normalized_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites;
  result.block_abi_invoke_trampoline_semantics_summary =
      result.integration_surface.block_abi_invoke_trampoline_semantics_summary;
  result.deterministic_block_abi_invoke_trampoline_handoff =
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      result.integration_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
          result.integration_surface.block_abi_invoke_trampoline_semantics_summary,
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary) &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .descriptor_symbolized_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .invoke_trampoline_symbolized_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .missing_invoke_trampoline_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .non_normalized_layout_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
                  .invoke_trampoline_symbolized_sites +
              result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
                  .missing_invoke_trampoline_sites ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .invoke_argument_slots_total ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .parameter_entries_total &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .capture_word_count_total ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.capture_entries_total;
  result.block_storage_escape_semantics_summary =
      result.integration_surface.block_storage_escape_semantics_summary;
  result.deterministic_block_storage_escape_handoff =
      result.type_metadata_handoff.block_storage_escape_semantics_summary.deterministic &&
      result.integration_surface.block_storage_escape_semantics_summary.deterministic &&
      IsEquivalentBlockStorageEscapeSemanticsSummary(
          result.integration_surface.block_storage_escape_semantics_summary,
          result.type_metadata_handoff.block_storage_escape_semantics_summary) &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_to_heap_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      // M261-C002 executable-block-object/invoke-thunk anchor: the current
      // runnable slice allows readonly scalar captures without forcing the old
      // mutable==capture or byref==capture equalities. Later C003 work widens
      // this to real byref/helper cases.
      result.type_metadata_handoff.block_storage_escape_semantics_summary.mutable_capture_count_total <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.byref_slot_count_total <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.mutable_capture_count_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites;
  result.block_copy_dispose_semantics_summary =
      result.integration_surface.block_copy_dispose_semantics_summary;
  result.deterministic_block_copy_dispose_handoff =
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.deterministic &&
      result.integration_surface.block_copy_dispose_semantics_summary.deterministic &&
      IsEquivalentBlockCopyDisposeSemanticsSummary(
          result.integration_surface.block_copy_dispose_semantics_summary,
          result.type_metadata_handoff.block_copy_dispose_semantics_summary) &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.profile_normalized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      // M261-C002 executable-block-object/invoke-thunk anchor: copy/dispose
      // determinism must also admit the readonly-capture slice even though no
      // helper bodies are emitted yet.
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.mutable_capture_count_total <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.byref_slot_count_total <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.mutable_capture_count_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_required_sites;
  result.block_determinism_perf_baseline_summary =
      result.integration_surface.block_determinism_perf_baseline_summary;
  result.deterministic_block_determinism_perf_baseline_handoff =
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.deterministic &&
      result.integration_surface.block_determinism_perf_baseline_summary.deterministic &&
      IsEquivalentBlockDeterminismPerfBaselineSummary(
          result.integration_surface.block_determinism_perf_baseline_summary,
          result.type_metadata_handoff.block_determinism_perf_baseline_summary) &&
      AreEquivalentBlockDeterminismPerfBaselineSites(
          result.integration_surface.block_determinism_perf_baseline_sites_lexicographic,
          result.type_metadata_handoff.block_determinism_perf_baseline_sites_lexicographic) &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.deterministic_capture_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.heavy_tier_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.normalized_profile_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites;
  const bool recovery_and_block_determinism_hardening_consistent =
      result.deterministic_error_diagnostics_recovery_handoff &&
      result.deterministic_block_determinism_perf_baseline_handoff;
  result.deterministic_type_metadata_handoff =
      result.deterministic_type_metadata_handoff &&
      recovery_and_block_determinism_hardening_consistent;
  if (!recovery_and_block_determinism_hardening_consistent) {
    return result;
  }
  result.message_send_selector_lowering_summary =
      result.integration_surface.message_send_selector_lowering_summary;
  result.deterministic_message_send_selector_lowering_handoff =
      result.type_metadata_handoff.message_send_selector_lowering_summary.deterministic &&
      result.integration_surface.message_send_selector_lowering_summary.deterministic &&
      IsEquivalentMessageSendSelectorLoweringSummary(
          result.integration_surface.message_send_selector_lowering_summary,
          result.type_metadata_handoff.message_send_selector_lowering_summary) &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.unary_form_sites +
              result.type_metadata_handoff.message_send_selector_lowering_summary.keyword_form_sites ==
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites;
  result.dispatch_abi_marshalling_summary = result.integration_surface.dispatch_abi_marshalling_summary;
  result.deterministic_dispatch_abi_marshalling_handoff =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.deterministic &&
      result.integration_surface.dispatch_abi_marshalling_summary.deterministic &&
      IsEquivalentDispatchAbiMarshallingSummary(
          result.integration_surface.dispatch_abi_marshalling_summary,
          result.type_metadata_handoff.dispatch_abi_marshalling_summary) &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.receiver_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites;
  result.nil_receiver_semantics_foldability_summary =
      result.integration_surface.nil_receiver_semantics_foldability_summary;
  result.deterministic_nil_receiver_semantics_foldability_handoff =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.deterministic &&
      result.integration_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      IsEquivalentNilReceiverSemanticsFoldabilitySummary(
          result.integration_surface.nil_receiver_semantics_foldability_summary,
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary) &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary
              .nil_receiver_runtime_dispatch_required_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.super_dispatch_method_family_summary =
      result.integration_surface.super_dispatch_method_family_summary;
  result.deterministic_super_dispatch_method_family_handoff =
      result.type_metadata_handoff.super_dispatch_method_family_summary.deterministic &&
      result.integration_surface.super_dispatch_method_family_summary.deterministic &&
      IsEquivalentSuperDispatchMethodFamilySummary(
          result.integration_surface.super_dispatch_method_family_summary,
          result.type_metadata_handoff.super_dispatch_method_family_summary) &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_new_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_none_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.contract_violation_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites;
  result.runtime_shim_host_link_summary = result.integration_surface.runtime_shim_host_link_summary;
  result.deterministic_runtime_shim_host_link_handoff =
      result.type_metadata_handoff.runtime_shim_host_link_summary.deterministic &&
      result.integration_surface.runtime_shim_host_link_summary.deterministic &&
      IsEquivalentRuntimeShimHostLinkSummary(
          result.integration_surface.runtime_shim_host_link_summary,
          result.type_metadata_handoff.runtime_shim_host_link_summary) &&
      result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_shim_required_sites +
              result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites &&
      result.type_metadata_handoff.runtime_shim_host_link_summary.contract_violation_sites <=
          result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites &&
      (result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites == 0 ||
       result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.type_metadata_handoff.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeShimHostLinkDefaultDispatchSymbol));
  result.retain_release_operation_summary = result.integration_surface.retain_release_operation_summary;
  result.deterministic_retain_release_operation_handoff =
      result.type_metadata_handoff.retain_release_operation_summary.deterministic &&
      result.integration_surface.retain_release_operation_summary.deterministic &&
      IsEquivalentRetainReleaseOperationSummary(
          result.integration_surface.retain_release_operation_summary,
          result.type_metadata_handoff.retain_release_operation_summary) &&
      // M262-D001 runtime ARC helper API surface anchor: sema continues to
      // publish ownership and helper-need summaries only; the concrete helper
      // ABI itself remains frozen in the runtime/lowering boundary.
      // M262-D002 runtime ARC helper implementation anchor: sema continues to
      // publish ownership and helper-need summaries only, while the supported
      // property/weak/autorelease-return ARC slice now reaches a live linked
      // helper/runtime boundary rather than a semantic promise here.
      // M262-D003 ownership-debug/runtime-validation anchor: sema still does
      // not own ARC debug counters or helper-traffic probes; it only preserves
      // the packets that the private runtime/testing surface consumes.
      result.type_metadata_handoff.retain_release_operation_summary.retain_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.release_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.autorelease_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites;
  result.weak_unowned_semantics_summary = result.integration_surface.weak_unowned_semantics_summary;
  result.deterministic_weak_unowned_semantics_handoff =
      result.type_metadata_handoff.weak_unowned_semantics_summary.deterministic &&
      result.integration_surface.weak_unowned_semantics_summary.deterministic &&
      IsEquivalentWeakUnownedSemanticsSummary(
          result.integration_surface.weak_unowned_semantics_summary,
          result.type_metadata_handoff.weak_unowned_semantics_summary) &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  // M262-A002 ARC mode-handling anchor: the handoff keeps ownership
  // qualifiers, weak/unowned summaries, ARC fix-it summaries, and
  // autoreleasepool scope accounting live and deterministic while explicit
  // ARC mode now widens executable ownership-qualified
  // function/method
  // signatures without claiming full ARC automation.
  // M262-B001 ARC semantic-rule freeze anchor: the pass-manager handoff still
  // treats ARC semantic legality as explicit-mode-only, preserving deterministic
  // weak/unowned and ARC-fixit surfaces while broader inference remains
  // deferred.
  // M262-B002 ARC inference/lifetime implementation anchor: the handoff now
  // treats explicit ARC mode as permission to infer strong-owned retain/release
  // activity for the supported unqualified object slice, while non-ARC still
  // preserves the zero-inference baseline and later ARC cleanup work remains
  // deferred.
  // M262-B003 ARC interaction-semantics expansion anchor: the handoff now also
  // preserves the semantic packets that distinguish weak/non-owning storage,
  // explicit autorelease returns, synthesized accessor ownership packets, and
  // block-capture interactions inside the supported ARC slice.
  // M262-C001 ARC lowering ABI/cleanup freeze anchor: this handoff remains
  // semantic-only and must not silently claim cleanup scheduling, helper-call
  // placement, or runtime ABI widening before the lane-C lowering issues land.
  // M262-C002 ARC automatic-insertion implementation anchor: the handoff now
  // also preserves the supported ARC insertion flags that lowering consumes for
  // helper placement, without moving retain/release/autorelease emission into
  // sema itself.
  // M262-C003 ARC cleanup/weak/lifetime implementation anchor: the handoff
  // continues to be semantic-only while preserving the packets lowering now
  // consumes for scope-exit cleanup ordering, weak current-property helper
  // usage, and deterministic block-capture lifetime cleanup.
  // M262-C004 ARC/block autorelease-return implementation anchor: the handoff
  // also preserves the explicit block-escape and return-autorelease packets,
  // while lane-C owns the branch-stable cleanup ordering needed to combine
  // them truthfully.
  // M262-E001 runnable-arc-runtime gate anchor: lane-E consumes this existing
  // source, semantic, lowering, and runtime proof chain and must not regress
  // into parser-only or metadata-only evidence.
  // M262-E002 runnable-arc-closeout anchor: the closeout matrix consumes this
  // unchanged proof chain plus integrated smoke instead of broadening parser
  // or semantic acceptance in lane E.
  result.arc_diagnostics_fixit_summary = result.integration_surface.arc_diagnostics_fixit_summary;
  result.deterministic_arc_diagnostics_fixit_handoff =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.deterministic &&
      result.integration_surface.arc_diagnostics_fixit_summary.deterministic &&
      IsEquivalentArcDiagnosticsFixitSummary(
          result.integration_surface.arc_diagnostics_fixit_summary,
          result.type_metadata_handoff.arc_diagnostics_fixit_summary) &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.autoreleasepool_scope_summary = result.integration_surface.autoreleasepool_scope_summary;
  result.deterministic_autoreleasepool_scope_handoff =
      result.type_metadata_handoff.autoreleasepool_scope_summary.deterministic &&
      result.integration_surface.autoreleasepool_scope_summary.deterministic &&
      IsEquivalentAutoreleasePoolScopeSummary(
          result.integration_surface.autoreleasepool_scope_summary,
          result.type_metadata_handoff.autoreleasepool_scope_summary) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.contract_violation_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      (result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites);
  result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);
  result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;
  result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);
  result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;
  result.sema_pass_flow_summary.diagnostics_after_pass = result.diagnostics_after_pass;
  result.sema_pass_flow_summary.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;
  result.sema_pass_flow_summary.diagnostics_accounting_consistent = result.diagnostics_accounting_consistent;
  result.sema_pass_flow_summary.diagnostics_bus_publish_consistent = result.diagnostics_bus_publish_consistent;
  result.sema_pass_flow_summary.diagnostics_canonicalized = result.diagnostics_canonicalized;
  result.sema_pass_flow_summary.diagnostics_hardening_satisfied = result.diagnostics_hardening_satisfied;
  result.sema_pass_flow_summary.parser_recovery_replay_ready = parser_recovery_replay_ready;
  result.sema_pass_flow_summary.parser_recovery_replay_case_present = parser_recovery_replay_case_present;
  result.sema_pass_flow_summary.parser_recovery_replay_case_passed = parser_recovery_replay_case_passed;
  result.sema_pass_flow_summary.recovery_replay_contract_satisfied =
      parser_recovery_replay_contract_satisfied;
  result.sema_pass_flow_summary.recovery_replay_key = recovery_replay_key;
  result.sema_pass_flow_summary.recovery_replay_key_deterministic = recovery_replay_key_deterministic;
  result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied =
      recovery_determinism_hardening_satisfied;
  FinalizeObjc3SemaPassFlowSummary(result.sema_pass_flow_summary,
                                   result.integration_surface,
                                   result.type_metadata_handoff,
                                   diagnostics_after_pass_monotonic,
                                   pass_order_matches_contract &&
                                       pass_iteration_index == kObjc3SemaPassOrder.size(),
                                   result.deterministic_semantic_diagnostics,
                                   result.deterministic_type_metadata_handoff);
  result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied =
      result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied &&
      result.sema_pass_flow_summary.diagnostics_hardening_satisfied &&
      result.sema_pass_flow_summary.robustness_guardrails_satisfied;

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
  result.parity_surface.parser_sema_advanced_edge_compatibility_shard1 =
      result.parser_sema_advanced_edge_compatibility_shard1;
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
  result.parity_surface.parser_sema_advanced_edge_compatibility_shard2 =
      result.parser_sema_advanced_edge_compatibility_shard2;
  result.parity_surface.parser_sema_advanced_diagnostics_shard2 =
      result.parser_sema_advanced_diagnostics_shard2;
  result.parity_surface.parser_sema_integration_closeout_signoff =
      result.parser_sema_integration_closeout_signoff;
  result.parity_surface.sema_pass_flow_summary = result.sema_pass_flow_summary;
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
          .runtime_hook_sites;
  result.parity_surface
      .task_runtime_cancellation_cancellation_check_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .cancellation_check_sites;
  result.parity_surface
      .task_runtime_cancellation_cancellation_handler_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .cancellation_handler_sites;
  result.parity_surface
      .task_runtime_cancellation_suspension_point_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .suspension_point_sites;
  result.parity_surface
      .task_runtime_cancellation_cancellation_propagation_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .cancellation_propagation_sites;
  result.parity_surface.task_runtime_cancellation_normalized_sites_total =
      result.parity_surface.task_runtime_cancellation_summary.normalized_sites;
  result.parity_surface.task_runtime_cancellation_gate_blocked_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .gate_blocked_sites;
  result.parity_surface
      .task_runtime_cancellation_contract_violation_sites_total =
      result.parity_surface.task_runtime_cancellation_summary
          .contract_violation_sites;
  result.parity_surface.concurrency_replay_race_guard_summary =
      result.type_metadata_handoff.concurrency_replay_race_guard_summary;
  result.parity_surface.concurrency_replay_race_guard_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .concurrency_replay_race_guard_sites;
  result.parity_surface
      .concurrency_replay_race_guard_concurrency_replay_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .concurrency_replay_sites;
  result.parity_surface.concurrency_replay_race_guard_replay_proof_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .replay_proof_sites;
  result.parity_surface.concurrency_replay_race_guard_race_guard_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .race_guard_sites;
  result.parity_surface.concurrency_replay_race_guard_task_handoff_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .task_handoff_sites;
  result.parity_surface
      .concurrency_replay_race_guard_actor_isolation_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .actor_isolation_sites;
  result.parity_surface
      .concurrency_replay_race_guard_deterministic_schedule_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .deterministic_schedule_sites;
  result.parity_surface.concurrency_replay_race_guard_guard_blocked_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .guard_blocked_sites;
  result.parity_surface
      .concurrency_replay_race_guard_contract_violation_sites_total =
      result.parity_surface.concurrency_replay_race_guard_summary
          .contract_violation_sites;
  result.parity_surface.unsafe_pointer_extension_summary =
      result.type_metadata_handoff.unsafe_pointer_extension_summary;
  result.parity_surface.unsafe_pointer_extension_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .unsafe_pointer_extension_sites;
  result.parity_surface.unsafe_pointer_extension_unsafe_keyword_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .unsafe_keyword_sites;
  result.parity_surface.unsafe_pointer_extension_pointer_arithmetic_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .pointer_arithmetic_sites;
  result.parity_surface.unsafe_pointer_extension_raw_pointer_type_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .raw_pointer_type_sites;
  result.parity_surface.unsafe_pointer_extension_unsafe_operation_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .unsafe_operation_sites;
  result.parity_surface.unsafe_pointer_extension_normalized_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites;
  result.parity_surface.unsafe_pointer_extension_gate_blocked_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .gate_blocked_sites;
  result.parity_surface.unsafe_pointer_extension_contract_violation_sites_total =
      result.parity_surface.unsafe_pointer_extension_summary
          .contract_violation_sites;
  result.parity_surface.inline_asm_intrinsic_governance_summary =
      result.type_metadata_handoff.inline_asm_intrinsic_governance_summary;
  result.parity_surface.inline_asm_intrinsic_governance_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .inline_asm_intrinsic_sites;
  result.parity_surface.inline_asm_intrinsic_governance_inline_asm_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .inline_asm_sites;
  result.parity_surface.inline_asm_intrinsic_governance_intrinsic_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .intrinsic_sites;
  result.parity_surface
      .inline_asm_intrinsic_governance_governed_intrinsic_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .governed_intrinsic_sites;
  result.parity_surface
      .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .privileged_intrinsic_sites;
  result.parity_surface.inline_asm_intrinsic_governance_normalized_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .normalized_sites;
  result.parity_surface.inline_asm_intrinsic_governance_gate_blocked_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .gate_blocked_sites;
  result.parity_surface
      .inline_asm_intrinsic_governance_contract_violation_sites_total =
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .contract_violation_sites;
  result.parity_surface.ns_error_bridging_summary =
      result.type_metadata_handoff.ns_error_bridging_summary;
  result.parity_surface.ns_error_bridging_sites_total =
      result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites;
  result.parity_surface.ns_error_bridging_ns_error_parameter_sites_total =
      result.parity_surface.ns_error_bridging_summary.ns_error_parameter_sites;
  result.parity_surface.ns_error_bridging_ns_error_out_parameter_sites_total =
      result.parity_surface.ns_error_bridging_summary
          .ns_error_out_parameter_sites;
  result.parity_surface.ns_error_bridging_ns_error_bridge_path_sites_total =
      result.parity_surface.ns_error_bridging_summary.ns_error_bridge_path_sites;
  result.parity_surface.ns_error_bridging_failable_call_sites_total =
      result.parity_surface.ns_error_bridging_summary.failable_call_sites;
  result.parity_surface.ns_error_bridging_normalized_sites_total =
      result.parity_surface.ns_error_bridging_summary.normalized_sites;
  result.parity_surface.ns_error_bridging_bridge_boundary_sites_total =
      result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites;
  result.parity_surface.ns_error_bridging_contract_violation_sites_total =
      result.parity_surface.ns_error_bridging_summary.contract_violation_sites;
  result.parity_surface.error_diagnostics_recovery_summary =
      result.type_metadata_handoff.error_diagnostics_recovery_summary;
  result.parity_surface.error_diagnostics_recovery_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .error_diagnostics_recovery_sites;
  result.parity_surface
      .error_diagnostics_recovery_diagnostic_emit_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .diagnostic_emit_sites;
  result.parity_surface
      .error_diagnostics_recovery_recovery_anchor_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .recovery_anchor_sites;
  result.parity_surface
      .error_diagnostics_recovery_recovery_boundary_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .recovery_boundary_sites;
  result.parity_surface
      .error_diagnostics_recovery_fail_closed_diagnostic_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .fail_closed_diagnostic_sites;
  result.parity_surface.error_diagnostics_recovery_normalized_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .normalized_sites;
  result.parity_surface.error_diagnostics_recovery_gate_blocked_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .gate_blocked_sites;
  result.parity_surface
      .error_diagnostics_recovery_contract_violation_sites_total =
      result.parity_surface.error_diagnostics_recovery_summary
          .contract_violation_sites;
  result.parity_surface.result_like_lowering_summary =
      result.type_metadata_handoff.result_like_lowering_summary;
  result.parity_surface.result_like_lowering_sites_total =
      result.parity_surface.result_like_lowering_summary.result_like_sites;
  result.parity_surface.result_like_lowering_result_success_sites_total =
      result.parity_surface.result_like_lowering_summary.result_success_sites;
  result.parity_surface.result_like_lowering_result_failure_sites_total =
      result.parity_surface.result_like_lowering_summary.result_failure_sites;
  result.parity_surface.result_like_lowering_result_branch_sites_total =
      result.parity_surface.result_like_lowering_summary.result_branch_sites;
  result.parity_surface.result_like_lowering_result_payload_sites_total =
      result.parity_surface.result_like_lowering_summary.result_payload_sites;
  result.parity_surface.result_like_lowering_normalized_sites_total =
      result.parity_surface.result_like_lowering_summary.normalized_sites;
  result.parity_surface.result_like_lowering_branch_merge_sites_total =
      result.parity_surface.result_like_lowering_summary.branch_merge_sites;
  result.parity_surface.result_like_lowering_contract_violation_sites_total =
      result.parity_surface.result_like_lowering_summary.contract_violation_sites;
  result.parity_surface.unwind_cleanup_summary =
      result.type_metadata_handoff.unwind_cleanup_summary;
  result.parity_surface.unwind_cleanup_sites_total =
      result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites;
  result.parity_surface.unwind_cleanup_exceptional_exit_sites_total =
      result.parity_surface.unwind_cleanup_summary.exceptional_exit_sites;
  result.parity_surface.unwind_cleanup_action_sites_total =
      result.parity_surface.unwind_cleanup_summary.cleanup_action_sites;
  result.parity_surface.unwind_cleanup_scope_sites_total =
      result.parity_surface.unwind_cleanup_summary.cleanup_scope_sites;
  result.parity_surface.unwind_cleanup_resume_sites_total =
      result.parity_surface.unwind_cleanup_summary.cleanup_resume_sites;
  result.parity_surface.unwind_cleanup_normalized_sites_total =
      result.parity_surface.unwind_cleanup_summary.normalized_sites;
  result.parity_surface.unwind_cleanup_fail_closed_sites_total =
      result.parity_surface.unwind_cleanup_summary.fail_closed_sites;
  result.parity_surface.unwind_cleanup_contract_violation_sites_total =
      result.parity_surface.unwind_cleanup_summary.contract_violation_sites;
  result.parity_surface.await_lowering_suspension_state_lowering_summary =
      result.type_metadata_handoff.await_lowering_suspension_state_lowering_summary;
  result.parity_surface.await_lowering_suspension_state_lowering_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_suspension_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_await_keyword_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_keyword_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_await_suspension_point_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_suspension_point_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_await_resume_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_resume_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_await_state_machine_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_state_machine_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_await_continuation_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .await_continuation_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_normalized_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .normalized_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_gate_blocked_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .gate_blocked_sites;
  result.parity_surface
      .await_lowering_suspension_state_lowering_contract_violation_sites_total =
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .contract_violation_sites;
  result.parity_surface.symbol_graph_scope_resolution_summary =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary;
  result.parity_surface.symbol_graph_global_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes;
  result.parity_surface.symbol_graph_function_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes;
  result.parity_surface.symbol_graph_interface_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  result.parity_surface.symbol_graph_interface_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  result.parity_surface.symbol_graph_interface_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  result.parity_surface.symbol_graph_top_level_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  result.parity_surface.symbol_graph_nested_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols;
  result.parity_surface.symbol_graph_scope_frames_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total;
  result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  result.parity_surface.symbol_graph_method_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites;
  result.parity_surface.symbol_graph_method_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits;
  result.parity_surface.symbol_graph_method_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses;
  result.parity_surface.method_lookup_override_conflict_summary =
      result.type_metadata_handoff.method_lookup_override_conflict_summary;
  result.parity_surface.method_lookup_override_conflict_lookup_sites_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites;
  result.parity_surface.method_lookup_override_conflict_lookup_hits_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits;
  result.parity_surface.method_lookup_override_conflict_lookup_misses_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses;
  result.parity_surface.method_lookup_override_conflict_override_sites_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites;
  result.parity_surface.method_lookup_override_conflict_override_hits_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits;
  result.parity_surface.method_lookup_override_conflict_override_misses_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses;
  result.parity_surface.method_lookup_override_conflict_override_conflicts_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts;
  result.parity_surface.method_lookup_override_conflict_unresolved_base_interfaces_total =
      result.parity_surface.method_lookup_override_conflict_summary.unresolved_base_interfaces;
  result.parity_surface.property_synthesis_ivar_binding_summary =
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary;
  result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites;
  result.parity_surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings;
  result.parity_surface.property_synthesis_ivar_binding_default_ivar_bindings_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_sites_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_resolved_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_missing_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts;
  result.parity_surface.id_class_sel_object_pointer_type_checking_summary =
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary;
  result.parity_surface.id_class_sel_object_pointer_param_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites;
  result.parity_surface.id_class_sel_object_pointer_param_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites;
  result.parity_surface.id_class_sel_object_pointer_return_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites;
  result.parity_surface.id_class_sel_object_pointer_return_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites;
  result.parity_surface.id_class_sel_object_pointer_property_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites;
  result.parity_surface.id_class_sel_object_pointer_property_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites;
  result.parity_surface.block_literal_capture_semantics_summary =
      result.type_metadata_handoff.block_literal_capture_semantics_summary;
  result.parity_surface.block_literal_capture_semantics_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites;
  result.parity_surface.block_literal_capture_semantics_parameter_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_parameter_entries;
  result.parity_surface.block_literal_capture_semantics_capture_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_capture_entries;
  result.parity_surface.block_literal_capture_semantics_body_statement_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_body_statement_entries;
  result.parity_surface.block_literal_capture_semantics_empty_capture_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites;
  result.parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites;
  result.parity_surface.block_literal_capture_semantics_non_normalized_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites;
  result.parity_surface.block_literal_capture_semantics_contract_violation_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites;
  result.parity_surface.block_abi_invoke_trampoline_semantics_summary =
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary;
  result.parity_surface.block_abi_invoke_trampoline_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites;
  result.parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total;
  result.parity_surface.block_abi_invoke_trampoline_capture_word_count_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total;
  result.parity_surface.block_abi_invoke_trampoline_parameter_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_capture_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_body_statement_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites;
  result.parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites;
  result.parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites;
  result.parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites;
  result.parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites;
  result.parity_surface.block_storage_escape_semantics_summary =
      result.type_metadata_handoff.block_storage_escape_semantics_summary;
  result.parity_surface.block_storage_escape_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites;
  result.parity_surface.block_storage_escape_mutable_capture_count_total =
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total;
  result.parity_surface.block_storage_escape_byref_slot_count_total =
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total;
  result.parity_surface.block_storage_escape_parameter_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.parameter_entries_total;
  result.parity_surface.block_storage_escape_capture_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total;
  result.parity_surface.block_storage_escape_body_statement_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_storage_escape_requires_byref_cells_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites;
  result.parity_surface.block_storage_escape_escape_analysis_enabled_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites;
  result.parity_surface.block_storage_escape_escape_to_heap_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites;
  result.parity_surface.block_storage_escape_escape_profile_normalized_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites;
  result.parity_surface.block_storage_escape_byref_layout_symbolized_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites;
  result.parity_surface.block_storage_escape_contract_violation_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites;
  result.parity_surface.block_copy_dispose_semantics_summary =
      result.type_metadata_handoff.block_copy_dispose_semantics_summary;
  result.parity_surface.block_copy_dispose_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites;
  result.parity_surface.block_copy_dispose_mutable_capture_count_total =
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total;
  result.parity_surface.block_copy_dispose_byref_slot_count_total =
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total;
  result.parity_surface.block_copy_dispose_parameter_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.parameter_entries_total;
  result.parity_surface.block_copy_dispose_capture_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total;
  result.parity_surface.block_copy_dispose_body_statement_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_copy_dispose_copy_helper_required_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites;
  result.parity_surface.block_copy_dispose_dispose_helper_required_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites;
  result.parity_surface.block_copy_dispose_profile_normalized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites;
  result.parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites;
  result.parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites;
  result.parity_surface.block_copy_dispose_contract_violation_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites;
  result.parity_surface.block_determinism_perf_baseline_summary =
      result.type_metadata_handoff.block_determinism_perf_baseline_summary;
  result.parity_surface.block_determinism_perf_baseline_sites_total =
      result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites;
  result.parity_surface.block_determinism_perf_baseline_weight_total =
      result.parity_surface.block_determinism_perf_baseline_summary.baseline_weight_total;
  result.parity_surface.block_determinism_perf_baseline_parameter_entries_total =
      result.parity_surface.block_determinism_perf_baseline_summary.parameter_entries_total;
  result.parity_surface.block_determinism_perf_baseline_capture_entries_total =
      result.parity_surface.block_determinism_perf_baseline_summary.capture_entries_total;
  result.parity_surface.block_determinism_perf_baseline_body_statement_entries_total =
      result.parity_surface.block_determinism_perf_baseline_summary.body_statement_entries_total;
  result.parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total =
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic_capture_sites;
  result.parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total =
      result.parity_surface.block_determinism_perf_baseline_summary.heavy_tier_sites;
  result.parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total =
      result.parity_surface.block_determinism_perf_baseline_summary.normalized_profile_sites;
  result.parity_surface.block_determinism_perf_baseline_contract_violation_sites_total =
      result.parity_surface.block_determinism_perf_baseline_summary.contract_violation_sites;
  result.parity_surface.message_send_selector_lowering_summary =
      result.type_metadata_handoff.message_send_selector_lowering_summary;
  result.parity_surface.message_send_selector_lowering_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.message_send_sites;
  result.parity_surface.message_send_selector_lowering_unary_form_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites;
  result.parity_surface.message_send_selector_lowering_keyword_form_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites;
  result.parity_surface.message_send_selector_lowering_symbol_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites;
  result.parity_surface.message_send_selector_lowering_piece_entries_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries;
  result.parity_surface.message_send_selector_lowering_argument_piece_entries_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries;
  result.parity_surface.message_send_selector_lowering_normalized_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites;
  result.parity_surface.message_send_selector_lowering_form_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_arity_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_symbol_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_missing_symbol_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites;
  result.parity_surface.message_send_selector_lowering_contract_violation_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites;
  result.parity_surface.dispatch_abi_marshalling_summary =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary;
  result.parity_surface.dispatch_abi_marshalling_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites;
  result.parity_surface.dispatch_abi_marshalling_receiver_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots;
  result.parity_surface.dispatch_abi_marshalling_selector_symbol_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots;
  result.parity_surface.dispatch_abi_marshalling_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.argument_slots;
  result.parity_surface.dispatch_abi_marshalling_keyword_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots;
  result.parity_surface.dispatch_abi_marshalling_unary_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots;
  result.parity_surface.dispatch_abi_marshalling_arity_mismatch_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites;
  result.parity_surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites;
  result.parity_surface.dispatch_abi_marshalling_contract_violation_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites;
  result.parity_surface.nil_receiver_semantics_foldability_summary =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary;
  result.parity_surface.nil_receiver_semantics_foldability_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites;
  result.parity_surface.nil_receiver_semantics_foldability_enabled_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites;
  result.parity_surface.nil_receiver_semantics_foldability_foldable_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites;
  result.parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites;
  result.parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites;
  result.parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites;
  result.parity_surface.super_dispatch_method_family_summary =
      result.type_metadata_handoff.super_dispatch_method_family_summary;
  result.parity_surface.super_dispatch_method_family_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.message_send_sites;
  result.parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites;
  result.parity_surface.super_dispatch_method_family_enabled_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites;
  result.parity_surface.super_dispatch_method_family_requires_class_context_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites;
  result.parity_surface.super_dispatch_method_family_init_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites;
  result.parity_surface.super_dispatch_method_family_copy_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites;
  result.parity_surface.super_dispatch_method_family_mutable_copy_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites;
  result.parity_surface.super_dispatch_method_family_new_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites;
  result.parity_surface.super_dispatch_method_family_none_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites;
  result.parity_surface.super_dispatch_method_family_returns_retained_result_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites;
  result.parity_surface.super_dispatch_method_family_returns_related_result_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites;
  result.parity_surface.super_dispatch_method_family_contract_violation_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites;
  result.parity_surface.runtime_shim_host_link_summary =
      result.type_metadata_handoff.runtime_shim_host_link_summary;
  result.parity_surface.runtime_shim_host_link_message_send_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.message_send_sites;
  result.parity_surface.runtime_shim_host_link_required_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites;
  result.parity_surface.runtime_shim_host_link_elided_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_arg_slots_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count;
  result.parity_surface.runtime_shim_host_link_contract_violation_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_symbol =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol;
  result.parity_surface.runtime_shim_host_link_default_runtime_dispatch_symbol_binding =
      result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding;
  result.parity_surface.retain_release_operation_summary =
      result.type_metadata_handoff.retain_release_operation_summary;
  result.parity_surface.retain_release_operation_ownership_qualified_sites_total =
      result.parity_surface.retain_release_operation_summary.ownership_qualified_sites;
  result.parity_surface.retain_release_operation_retain_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites;
  result.parity_surface.retain_release_operation_release_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.release_insertion_sites;
  result.parity_surface.retain_release_operation_autorelease_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites;
  result.parity_surface.retain_release_operation_contract_violation_sites_total =
      result.parity_surface.retain_release_operation_summary.contract_violation_sites;
  result.parity_surface.weak_unowned_semantics_summary =
      result.type_metadata_handoff.weak_unowned_semantics_summary;
  result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites;
  result.parity_surface.weak_unowned_semantics_weak_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.weak_reference_sites;
  result.parity_surface.weak_unowned_semantics_unowned_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites;
  result.parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites;
  result.parity_surface.weak_unowned_semantics_conflict_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  result.parity_surface.weak_unowned_semantics_contract_violation_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites;
  result.parity_surface.arc_diagnostics_fixit_summary =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary;
  result.parity_surface.ownership_arc_diagnostic_candidate_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites;
  result.parity_surface.ownership_arc_fixit_available_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites;
  result.parity_surface.ownership_arc_profiled_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites;
  result.parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  result.parity_surface.ownership_arc_empty_fixit_hint_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites;
  result.parity_surface.ownership_arc_contract_violation_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.parity_surface.autoreleasepool_scope_summary = result.type_metadata_handoff.autoreleasepool_scope_summary;
  result.parity_surface.autoreleasepool_scope_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.scope_sites;
  result.parity_surface.autoreleasepool_scope_symbolized_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites;
  result.parity_surface.autoreleasepool_scope_contract_violation_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites;
  result.parity_surface.autoreleasepool_scope_max_depth_total =
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth;
  result.parity_surface.diagnostics_accounting_consistent = result.diagnostics_accounting_consistent;
  result.parity_surface.diagnostics_bus_publish_consistent = result.diagnostics_bus_publish_consistent;
  result.parity_surface.diagnostics_canonicalized = result.diagnostics_canonicalized;
  result.parity_surface.diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  result.parity_surface.diagnostics_hardening_satisfied =
      result.diagnostics_hardening_satisfied &&
      result.parity_surface.diagnostics_accounting_consistent &&
      result.parity_surface.diagnostics_bus_publish_consistent &&
      result.parity_surface.diagnostics_canonicalized &&
      result.parity_surface.diagnostics_after_pass_monotonic;
  result.parity_surface.pass_flow_recovery_replay_contract_satisfied =
      result.sema_pass_flow_summary.recovery_replay_contract_satisfied;
  result.parity_surface.pass_flow_recovery_replay_key =
      result.sema_pass_flow_summary.recovery_replay_key;
  result.parity_surface.pass_flow_recovery_replay_key_deterministic =
      result.sema_pass_flow_summary.recovery_replay_key_deterministic;
  result.parity_surface.pass_flow_recovery_determinism_hardening_satisfied =
      result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied;
  result.parity_surface.deterministic_parser_sema_conformance_matrix =
      result.deterministic_parser_sema_conformance_matrix &&
      result.parity_surface.parser_sema_conformance_matrix.deterministic;
  result.parity_surface.deterministic_parser_sema_conformance_corpus =
      result.deterministic_parser_sema_conformance_corpus &&
      result.parity_surface.parser_sema_conformance_corpus.deterministic;
  result.parity_surface.deterministic_parser_sema_performance_quality_guardrails =
      result.deterministic_parser_sema_performance_quality_guardrails &&
      result.parity_surface.parser_sema_performance_quality_guardrails.deterministic;
  result.parity_surface.deterministic_parser_sema_cross_lane_integration_sync =
      result.deterministic_parser_sema_cross_lane_integration_sync &&
      result.parity_surface.parser_sema_cross_lane_integration_sync.deterministic;
  result.parity_surface.deterministic_parser_sema_docs_runbook_sync =
      result.deterministic_parser_sema_docs_runbook_sync &&
      result.parity_surface.parser_sema_docs_runbook_sync.deterministic;
  result.parity_surface.deterministic_parser_sema_release_candidate_replay_dry_run =
      result.deterministic_parser_sema_release_candidate_replay_dry_run &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_core_shard1 =
      result.deterministic_parser_sema_advanced_core_shard1 &&
      result.parity_surface.parser_sema_advanced_core_shard1.deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_edge_compatibility_shard1 =
      result.deterministic_parser_sema_advanced_edge_compatibility_shard1 &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard1 =
      result.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_conformance_shard1 =
      result.deterministic_parser_sema_advanced_conformance_shard1 &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_integration_shard1 =
      result.deterministic_parser_sema_advanced_integration_shard1 &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_performance_shard1 =
      result.deterministic_parser_sema_advanced_performance_shard1 &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_core_shard2 =
      result.deterministic_parser_sema_advanced_core_shard2 &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_edge_compatibility_shard2 =
      result.deterministic_parser_sema_advanced_edge_compatibility_shard2 &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .deterministic;
  result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard2 =
      result.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .deterministic;
  result.parity_surface.deterministic_parser_sema_integration_closeout_signoff =
      result.deterministic_parser_sema_integration_closeout_signoff &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .deterministic;
  result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;
  result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;
  result.parity_surface.deterministic_interface_implementation_handoff =
      result.deterministic_interface_implementation_handoff &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.resolved_interfaces ==
          result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.interface_implementation_summary.resolved_implementations ==
          result.parity_surface.type_metadata_implementation_entries;
  result.parity_surface.deterministic_protocol_category_composition_handoff =
      result.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites <=
          result.parity_surface.protocol_category_composition_summary.total_composition_sites();
  result.parity_surface.deterministic_class_protocol_category_linking_handoff =
      result.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.declared_interfaces ==
          result.parity_surface.interface_implementation_summary.declared_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_interfaces ==
          result.parity_surface.interface_implementation_summary.resolved_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.declared_implementations ==
          result.parity_surface.interface_implementation_summary.declared_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_implementations ==
          result.parity_surface.interface_implementation_summary.resolved_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.interface_method_symbols ==
          result.parity_surface.interface_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.parity_surface.implementation_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.parity_surface.linked_implementation_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.parity_surface.class_protocol_category_linking_summary.total_composition_sites() &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic;
  result.parity_surface.deterministic_selector_normalization_handoff =
      result.deterministic_selector_normalization_handoff &&
      result.parity_surface.selector_normalization_summary.methods_total ==
          result.parity_surface.selector_normalization_methods_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods ==
          result.parity_surface.selector_normalization_normalized_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_piece_entries ==
          result.parity_surface.selector_normalization_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries ==
          result.parity_surface.selector_normalization_parameter_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods ==
          result.parity_surface.selector_normalization_pieceless_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches ==
          result.parity_surface.selector_normalization_spelling_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches ==
          result.parity_surface.selector_normalization_arity_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches ==
          result.parity_surface.selector_normalization_parameter_linkage_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches ==
          result.parity_surface.selector_normalization_flag_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          result.parity_surface.selector_normalization_missing_keyword_pieces_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries <=
          result.parity_surface.selector_normalization_summary.selector_piece_entries &&
      result.parity_surface.selector_normalization_summary.contract_violations() <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.deterministic;
  result.parity_surface.deterministic_property_attribute_handoff =
      result.deterministic_property_attribute_handoff &&
      result.parity_surface.property_attribute_summary.properties_total ==
          result.parity_surface.property_attribute_properties_total &&
      result.parity_surface.property_attribute_summary.attribute_entries ==
          result.parity_surface.property_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.readonly_modifiers ==
          result.parity_surface.property_attribute_readonly_modifiers_total &&
      result.parity_surface.property_attribute_summary.readwrite_modifiers ==
          result.parity_surface.property_attribute_readwrite_modifiers_total &&
      result.parity_surface.property_attribute_summary.atomic_modifiers ==
          result.parity_surface.property_attribute_atomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.nonatomic_modifiers ==
          result.parity_surface.property_attribute_nonatomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.copy_modifiers ==
          result.parity_surface.property_attribute_copy_modifiers_total &&
      result.parity_surface.property_attribute_summary.strong_modifiers ==
          result.parity_surface.property_attribute_strong_modifiers_total &&
      result.parity_surface.property_attribute_summary.weak_modifiers ==
          result.parity_surface.property_attribute_weak_modifiers_total &&
      result.parity_surface.property_attribute_summary.assign_modifiers ==
          result.parity_surface.property_attribute_assign_modifiers_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers ==
          result.parity_surface.property_attribute_getter_modifiers_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers ==
          result.parity_surface.property_attribute_setter_modifiers_total &&
      result.parity_surface.property_attribute_summary.invalid_attribute_entries ==
          result.parity_surface.property_attribute_invalid_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.property_contract_violations ==
          result.parity_surface.property_attribute_contract_violations_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.deterministic;
  result.parity_surface.deterministic_type_annotation_surface_handoff =
      result.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites ==
          result.parity_surface.type_annotation_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites ==
          result.parity_surface.type_annotation_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites ==
          result.parity_surface.type_annotation_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          result.parity_surface.type_annotation_ownership_qualifier_sites_total &&
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites ==
          result.parity_surface.type_annotation_object_pointer_type_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          result.parity_surface.type_annotation_invalid_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
          result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
          result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
          result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.generic_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.parity_surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      result.parity_surface.type_annotation_surface_summary.deterministic;
  result.parity_surface.deterministic_lightweight_generic_constraint_handoff =
      result.deterministic_lightweight_generic_constraint_handoff &&
      result.parity_surface.lightweight_generic_constraint_summary.generic_constraint_sites ==
          result.parity_surface.lightweight_generic_constraint_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.generic_suffix_sites ==
          result.parity_surface.lightweight_generic_constraint_generic_suffix_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.object_pointer_type_sites ==
          result.parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites ==
          result.parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.pointer_declarator_sites ==
          result.parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.normalized_constraint_sites ==
          result.parity_surface.lightweight_generic_constraint_normalized_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.contract_violation_sites ==
          result.parity_surface.lightweight_generic_constraint_contract_violation_sites_total &&
      result.parity_surface.lightweight_generic_constraint_summary.terminated_generic_suffix_sites <=
          result.parity_surface.lightweight_generic_constraint_summary.generic_suffix_sites &&
      result.parity_surface.lightweight_generic_constraint_summary.normalized_constraint_sites <=
          result.parity_surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
      result.parity_surface.lightweight_generic_constraint_summary.contract_violation_sites <=
          result.parity_surface.lightweight_generic_constraint_summary.generic_constraint_sites &&
      result.parity_surface.lightweight_generic_constraint_summary.deterministic;
  result.parity_surface.deterministic_nullability_flow_warning_precision_handoff =
      result.deterministic_nullability_flow_warning_precision_handoff &&
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_flow_sites ==
          result.parity_surface.nullability_flow_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.object_pointer_type_sites ==
          result.parity_surface.nullability_flow_object_pointer_type_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          result.parity_surface.nullability_flow_nullability_suffix_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.nullable_suffix_sites ==
          result.parity_surface.nullability_flow_nullable_suffix_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites ==
          result.parity_surface.nullability_flow_nonnull_suffix_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.normalized_sites ==
          result.parity_surface.nullability_flow_normalized_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.contract_violation_sites ==
          result.parity_surface.nullability_flow_contract_violation_sites_total &&
      result.parity_surface.nullability_flow_warning_precision_summary.normalized_sites <=
          result.parity_surface.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.parity_surface.nullability_flow_warning_precision_summary.contract_violation_sites <=
          result.parity_surface.nullability_flow_warning_precision_summary.nullability_flow_sites &&
      result.parity_surface.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
          result.parity_surface.nullability_flow_warning_precision_summary.nullable_suffix_sites +
              result.parity_surface.nullability_flow_warning_precision_summary.nonnull_suffix_sites &&
      result.parity_surface.nullability_flow_warning_precision_summary.deterministic;
  result.parity_surface.deterministic_protocol_qualified_object_type_handoff =
      result.deterministic_protocol_qualified_object_type_handoff &&
      result.parity_surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites ==
          result.parity_surface.protocol_qualified_object_type_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.protocol_composition_sites ==
          result.parity_surface.protocol_qualified_object_type_protocol_composition_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.object_pointer_type_sites ==
          result.parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites ==
          result.parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.pointer_declarator_sites ==
          result.parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites ==
          result.parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.contract_violation_sites ==
          result.parity_surface.protocol_qualified_object_type_contract_violation_sites_total &&
      result.parity_surface.protocol_qualified_object_type_summary.terminated_protocol_composition_sites <=
          result.parity_surface.protocol_qualified_object_type_summary.protocol_composition_sites &&
      result.parity_surface.protocol_qualified_object_type_summary.normalized_protocol_composition_sites <=
          result.parity_surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
      result.parity_surface.protocol_qualified_object_type_summary.contract_violation_sites <=
          result.parity_surface.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
      result.parity_surface.protocol_qualified_object_type_summary.deterministic;
  result.parity_surface.deterministic_variance_bridge_cast_handoff =
      result.deterministic_variance_bridge_cast_handoff &&
      result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites ==
          result.parity_surface.variance_bridge_cast_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.protocol_composition_sites ==
          result.parity_surface.variance_bridge_cast_protocol_composition_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.ownership_qualifier_sites ==
          result.parity_surface.variance_bridge_cast_ownership_qualifier_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.object_pointer_type_sites ==
          result.parity_surface.variance_bridge_cast_object_pointer_type_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.pointer_declarator_sites ==
          result.parity_surface.variance_bridge_cast_pointer_declarator_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.normalized_sites ==
          result.parity_surface.variance_bridge_cast_normalized_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.contract_violation_sites ==
          result.parity_surface.variance_bridge_cast_contract_violation_sites_total &&
      result.parity_surface.variance_bridge_cast_summary.protocol_composition_sites <=
          result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.parity_surface.variance_bridge_cast_summary.normalized_sites <=
          result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.parity_surface.variance_bridge_cast_summary.contract_violation_sites <=
          result.parity_surface.variance_bridge_cast_summary.variance_bridge_cast_sites &&
      result.parity_surface.variance_bridge_cast_summary.deterministic;
  result.parity_surface.deterministic_generic_metadata_abi_handoff =
      result.deterministic_generic_metadata_abi_handoff &&
      result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites ==
          result.parity_surface.generic_metadata_abi_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.generic_suffix_sites ==
          result.parity_surface.generic_metadata_abi_generic_suffix_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.protocol_composition_sites ==
          result.parity_surface.generic_metadata_abi_protocol_composition_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.ownership_qualifier_sites ==
          result.parity_surface.generic_metadata_abi_ownership_qualifier_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.object_pointer_type_sites ==
          result.parity_surface.generic_metadata_abi_object_pointer_type_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.pointer_declarator_sites ==
          result.parity_surface.generic_metadata_abi_pointer_declarator_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.normalized_sites ==
          result.parity_surface.generic_metadata_abi_normalized_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.contract_violation_sites ==
          result.parity_surface.generic_metadata_abi_contract_violation_sites_total &&
      result.parity_surface.generic_metadata_abi_summary.generic_suffix_sites <=
          result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.parity_surface.generic_metadata_abi_summary.protocol_composition_sites <=
          result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.parity_surface.generic_metadata_abi_summary.normalized_sites <=
          result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.parity_surface.generic_metadata_abi_summary.contract_violation_sites <=
          result.parity_surface.generic_metadata_abi_summary.generic_metadata_abi_sites &&
      result.parity_surface.generic_metadata_abi_summary.deterministic;
  result.parity_surface.deterministic_module_import_graph_handoff =
      result.deterministic_module_import_graph_handoff &&
      result.parity_surface.module_import_graph_summary.module_import_graph_sites ==
          result.parity_surface.module_import_graph_sites_total &&
      result.parity_surface.module_import_graph_summary.import_edge_candidate_sites ==
          result.parity_surface.module_import_graph_import_edge_candidate_sites_total &&
      result.parity_surface.module_import_graph_summary.namespace_segment_sites ==
          result.parity_surface.module_import_graph_namespace_segment_sites_total &&
      result.parity_surface.module_import_graph_summary.object_pointer_type_sites ==
          result.parity_surface.module_import_graph_object_pointer_type_sites_total &&
      result.parity_surface.module_import_graph_summary.pointer_declarator_sites ==
          result.parity_surface.module_import_graph_pointer_declarator_sites_total &&
      result.parity_surface.module_import_graph_summary.normalized_sites ==
          result.parity_surface.module_import_graph_normalized_sites_total &&
      result.parity_surface.module_import_graph_summary.contract_violation_sites ==
          result.parity_surface.module_import_graph_contract_violation_sites_total &&
      result.parity_surface.module_import_graph_summary.import_edge_candidate_sites <=
          result.parity_surface.module_import_graph_summary.module_import_graph_sites &&
      result.parity_surface.module_import_graph_summary.namespace_segment_sites <=
          result.parity_surface.module_import_graph_summary.module_import_graph_sites &&
      result.parity_surface.module_import_graph_summary.normalized_sites <=
          result.parity_surface.module_import_graph_summary.module_import_graph_sites &&
      result.parity_surface.module_import_graph_summary.contract_violation_sites <=
          result.parity_surface.module_import_graph_summary.module_import_graph_sites &&
      result.parity_surface.module_import_graph_summary.deterministic;
  result.parity_surface.deterministic_namespace_collision_shadowing_handoff =
      result.deterministic_namespace_collision_shadowing_handoff &&
      result.parity_surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites ==
          result.parity_surface.namespace_collision_shadowing_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .namespace_segment_sites ==
          result.parity_surface
              .namespace_collision_shadowing_namespace_segment_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites ==
          result.parity_surface
              .namespace_collision_shadowing_import_edge_candidate_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .object_pointer_type_sites ==
          result.parity_surface
              .namespace_collision_shadowing_object_pointer_type_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .pointer_declarator_sites ==
          result.parity_surface
              .namespace_collision_shadowing_pointer_declarator_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary.normalized_sites ==
          result.parity_surface.namespace_collision_shadowing_normalized_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .contract_violation_sites ==
          result.parity_surface
              .namespace_collision_shadowing_contract_violation_sites_total &&
      result.parity_surface.namespace_collision_shadowing_summary
              .namespace_segment_sites <=
          result.parity_surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.parity_surface.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          result.parity_surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.parity_surface.namespace_collision_shadowing_summary.normalized_sites <=
          result.parity_surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.parity_surface.namespace_collision_shadowing_summary
              .contract_violation_sites <=
          result.parity_surface.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.parity_surface.namespace_collision_shadowing_summary.deterministic;
  result.parity_surface.deterministic_public_private_api_partition_handoff =
      result.deterministic_public_private_api_partition_handoff &&
      result.parity_surface.public_private_api_partition_summary
              .public_private_api_partition_sites ==
          result.parity_surface.public_private_api_partition_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .namespace_segment_sites ==
          result.parity_surface
              .public_private_api_partition_namespace_segment_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .import_edge_candidate_sites ==
          result.parity_surface
              .public_private_api_partition_import_edge_candidate_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .object_pointer_type_sites ==
          result.parity_surface
              .public_private_api_partition_object_pointer_type_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .pointer_declarator_sites ==
          result.parity_surface
              .public_private_api_partition_pointer_declarator_sites_total &&
      result.parity_surface.public_private_api_partition_summary.normalized_sites ==
          result.parity_surface.public_private_api_partition_normalized_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .contract_violation_sites ==
          result.parity_surface
              .public_private_api_partition_contract_violation_sites_total &&
      result.parity_surface.public_private_api_partition_summary
              .namespace_segment_sites <=
          result.parity_surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.parity_surface.public_private_api_partition_summary
              .import_edge_candidate_sites <=
          result.parity_surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.parity_surface.public_private_api_partition_summary.normalized_sites <=
          result.parity_surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.parity_surface.public_private_api_partition_summary
              .contract_violation_sites <=
          result.parity_surface.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.parity_surface.public_private_api_partition_summary.deterministic;
  result.parity_surface.deterministic_incremental_module_cache_invalidation_handoff =
      result.deterministic_incremental_module_cache_invalidation_handoff &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites ==
          result.parity_surface.incremental_module_cache_invalidation_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_namespace_segment_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_import_edge_candidate_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .object_pointer_type_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_object_pointer_type_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .pointer_declarator_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_pointer_declarator_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .normalized_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_normalized_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites ==
          result.parity_surface
              .incremental_module_cache_invalidation_contract_violation_sites_total &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .normalized_sites <=
          result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.parity_surface.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          result.parity_surface.incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.parity_surface.incremental_module_cache_invalidation_summary
          .deterministic;
  result.parity_surface.deterministic_cross_module_conformance_handoff =
      result.deterministic_cross_module_conformance_handoff &&
      result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites ==
          result.parity_surface.cross_module_conformance_sites_total &&
      result.parity_surface.cross_module_conformance_summary.namespace_segment_sites ==
          result.parity_surface.cross_module_conformance_namespace_segment_sites_total &&
      result.parity_surface.cross_module_conformance_summary.import_edge_candidate_sites ==
          result.parity_surface.cross_module_conformance_import_edge_candidate_sites_total &&
      result.parity_surface.cross_module_conformance_summary.object_pointer_type_sites ==
          result.parity_surface.cross_module_conformance_object_pointer_type_sites_total &&
      result.parity_surface.cross_module_conformance_summary.pointer_declarator_sites ==
          result.parity_surface.cross_module_conformance_pointer_declarator_sites_total &&
      result.parity_surface.cross_module_conformance_summary.normalized_sites ==
          result.parity_surface.cross_module_conformance_normalized_sites_total &&
      result.parity_surface.cross_module_conformance_summary.cache_invalidation_candidate_sites ==
          result.parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total &&
      result.parity_surface.cross_module_conformance_summary.contract_violation_sites ==
          result.parity_surface.cross_module_conformance_contract_violation_sites_total &&
      result.parity_surface.cross_module_conformance_summary.namespace_segment_sites <=
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.import_edge_candidate_sites <=
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.normalized_sites <=
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.cache_invalidation_candidate_sites <=
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.normalized_sites +
              result.parity_surface.cross_module_conformance_summary
                  .cache_invalidation_candidate_sites ==
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.contract_violation_sites <=
          result.parity_surface.cross_module_conformance_summary.cross_module_conformance_sites &&
      result.parity_surface.cross_module_conformance_summary.deterministic;
  result.parity_surface.deterministic_throws_propagation_handoff =
      result.deterministic_throws_propagation_handoff &&
      result.parity_surface.throws_propagation_summary.throws_propagation_sites ==
          result.parity_surface.throws_propagation_sites_total &&
      result.parity_surface.throws_propagation_summary.namespace_segment_sites ==
          result.parity_surface.throws_propagation_namespace_segment_sites_total &&
      result.parity_surface.throws_propagation_summary.import_edge_candidate_sites ==
          result.parity_surface.throws_propagation_import_edge_candidate_sites_total &&
      result.parity_surface.throws_propagation_summary.object_pointer_type_sites ==
          result.parity_surface.throws_propagation_object_pointer_type_sites_total &&
      result.parity_surface.throws_propagation_summary.pointer_declarator_sites ==
          result.parity_surface.throws_propagation_pointer_declarator_sites_total &&
      result.parity_surface.throws_propagation_summary.normalized_sites ==
          result.parity_surface.throws_propagation_normalized_sites_total &&
      result.parity_surface.throws_propagation_summary.cache_invalidation_candidate_sites ==
          result.parity_surface.throws_propagation_cache_invalidation_candidate_sites_total &&
      result.parity_surface.throws_propagation_summary.contract_violation_sites ==
          result.parity_surface.throws_propagation_contract_violation_sites_total &&
      result.parity_surface.throws_propagation_summary.namespace_segment_sites <=
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.import_edge_candidate_sites <=
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.normalized_sites <=
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.cache_invalidation_candidate_sites <=
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.normalized_sites +
              result.parity_surface.throws_propagation_summary
                  .cache_invalidation_candidate_sites ==
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.contract_violation_sites <=
          result.parity_surface.throws_propagation_summary.throws_propagation_sites &&
      result.parity_surface.throws_propagation_summary.deterministic;
  result.parity_surface.deterministic_actor_isolation_sendability_handoff =
      result.deterministic_actor_isolation_sendability_handoff &&
      result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites ==
          result.parity_surface.actor_isolation_sendability_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_decl_sites ==
          result.parity_surface.actor_isolation_decl_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary.actor_hop_sites ==
          result.parity_surface.actor_hop_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .sendable_annotation_sites ==
          result.parity_surface.sendable_annotation_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites ==
          result.parity_surface.non_sendable_crossing_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .isolation_boundary_sites ==
          result.parity_surface
              .actor_isolation_sendability_isolation_boundary_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .normalized_sites ==
          result.parity_surface
              .actor_isolation_sendability_normalized_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .gate_blocked_sites ==
          result.parity_surface
              .actor_isolation_sendability_gate_blocked_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .contract_violation_sites ==
          result.parity_surface
              .actor_isolation_sendability_contract_violation_sites_total &&
      result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_decl_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .actor_hop_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .sendable_annotation_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .isolation_boundary_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .normalized_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .contract_violation_sites <=
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary
              .normalized_sites +
              result.parity_surface.actor_isolation_sendability_summary
                  .gate_blocked_sites ==
          result.parity_surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.parity_surface.actor_isolation_sendability_summary.deterministic;
  result.parity_surface.deterministic_task_runtime_cancellation_handoff =
      result.deterministic_task_runtime_cancellation_handoff &&
      result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites ==
          result.parity_surface.task_runtime_cancellation_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .runtime_hook_sites ==
          result.parity_surface
              .task_runtime_cancellation_runtime_hook_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_check_sites ==
          result.parity_surface
              .task_runtime_cancellation_cancellation_check_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_handler_sites ==
          result.parity_surface
              .task_runtime_cancellation_cancellation_handler_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .suspension_point_sites ==
          result.parity_surface
              .task_runtime_cancellation_suspension_point_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites ==
          result.parity_surface
              .task_runtime_cancellation_cancellation_propagation_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .normalized_sites ==
          result.parity_surface.task_runtime_cancellation_normalized_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .gate_blocked_sites ==
          result.parity_surface
              .task_runtime_cancellation_gate_blocked_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .contract_violation_sites ==
          result.parity_surface
              .task_runtime_cancellation_contract_violation_sites_total &&
      result.parity_surface.task_runtime_cancellation_summary
              .runtime_hook_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_check_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_handler_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .suspension_point_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .cancellation_check_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .cancellation_handler_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .normalized_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .gate_blocked_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .contract_violation_sites <=
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary
              .normalized_sites +
              result.parity_surface.task_runtime_cancellation_summary
                  .gate_blocked_sites ==
          result.parity_surface.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.parity_surface.task_runtime_cancellation_summary.deterministic;
  result.parity_surface.deterministic_concurrency_replay_race_guard_handoff =
      result.deterministic_concurrency_replay_race_guard_handoff &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites ==
          result.parity_surface.concurrency_replay_race_guard_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_concurrency_replay_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .replay_proof_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_replay_proof_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .race_guard_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_race_guard_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .task_handoff_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_task_handoff_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .actor_isolation_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_actor_isolation_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_deterministic_schedule_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .guard_blocked_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_guard_blocked_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .contract_violation_sites ==
          result.parity_surface
              .concurrency_replay_race_guard_contract_violation_sites_total &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .replay_proof_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .race_guard_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .task_handoff_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
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
  result.parity_surface.deterministic_unsafe_pointer_extension_handoff =
      result.deterministic_unsafe_pointer_extension_handoff &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites ==
          result.parity_surface.unsafe_pointer_extension_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
          result.parity_surface
              .unsafe_pointer_extension_unsafe_keyword_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites ==
          result.parity_surface
              .unsafe_pointer_extension_pointer_arithmetic_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
          result.parity_surface
              .unsafe_pointer_extension_raw_pointer_type_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites ==
          result.parity_surface
              .unsafe_pointer_extension_unsafe_operation_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites ==
          result.parity_surface.unsafe_pointer_extension_normalized_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          result.parity_surface
              .unsafe_pointer_extension_gate_blocked_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary
              .contract_violation_sites ==
          result.parity_surface
              .unsafe_pointer_extension_contract_violation_sites_total &&
      result.parity_surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .pointer_arithmetic_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .raw_pointer_type_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary
              .contract_violation_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.normalized_sites +
              result.parity_surface.unsafe_pointer_extension_summary
                  .gate_blocked_sites ==
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      result.parity_surface.unsafe_pointer_extension_summary.deterministic;
  result.parity_surface.deterministic_inline_asm_intrinsic_governance_handoff =
      result.deterministic_inline_asm_intrinsic_governance_handoff &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_inline_asm_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_normalized_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_gate_blocked_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites ==
          result.parity_surface
              .inline_asm_intrinsic_governance_contract_violation_sites_total &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites ==
          result.parity_surface.throws_propagation_summary
              .cache_invalidation_candidate_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .intrinsic_sites ==
          result.parity_surface.unsafe_pointer_extension_summary
              .unsafe_operation_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          result.parity_surface.throws_propagation_summary.normalized_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          result.parity_surface.unsafe_pointer_extension_summary
              .normalized_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites >=
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites -
              result.parity_surface
                  .inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .gate_blocked_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
                  .intrinsic_sites -
              result.parity_surface
                  .inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
              .normalized_sites +
              result.parity_surface.inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          result.parity_surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      result.parity_surface.inline_asm_intrinsic_governance_summary
          .deterministic;
  result.parity_surface.deterministic_ns_error_bridging_handoff =
      result.deterministic_ns_error_bridging_handoff &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites ==
          result.parity_surface.ns_error_bridging_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_parameter_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_parameter_sites_total &&
      result.parity_surface.ns_error_bridging_summary
              .ns_error_out_parameter_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_out_parameter_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
          result.parity_surface
              .ns_error_bridging_ns_error_bridge_path_sites_total &&
      result.parity_surface.ns_error_bridging_summary.failable_call_sites ==
          result.parity_surface.ns_error_bridging_failable_call_sites_total &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites ==
          result.parity_surface.ns_error_bridging_normalized_sites_total &&
      result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites ==
          result.parity_surface.ns_error_bridging_bridge_boundary_sites_total &&
      result.parity_surface.ns_error_bridging_summary.contract_violation_sites ==
          result.parity_surface.ns_error_bridging_contract_violation_sites_total &&
      result.parity_surface.ns_error_bridging_summary.ns_error_parameter_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary
              .ns_error_out_parameter_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.failable_call_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.normalized_sites +
              result.parity_surface.ns_error_bridging_summary.bridge_boundary_sites ==
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.contract_violation_sites <=
          result.parity_surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      result.parity_surface.ns_error_bridging_summary.deterministic;
  result.parity_surface.deterministic_error_diagnostics_recovery_handoff =
      result.deterministic_error_diagnostics_recovery_handoff &&
      result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites ==
          result.parity_surface.error_diagnostics_recovery_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites ==
          result.parity_surface
              .error_diagnostics_recovery_diagnostic_emit_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_anchor_sites ==
          result.parity_surface
              .error_diagnostics_recovery_recovery_anchor_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_boundary_sites ==
          result.parity_surface
              .error_diagnostics_recovery_recovery_boundary_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites ==
          result.parity_surface
              .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites ==
          result.parity_surface.error_diagnostics_recovery_normalized_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .gate_blocked_sites ==
          result.parity_surface.error_diagnostics_recovery_gate_blocked_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .contract_violation_sites ==
          result.parity_surface
              .error_diagnostics_recovery_contract_violation_sites_total &&
      result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_anchor_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .recovery_boundary_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .fail_closed_diagnostic_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .diagnostic_emit_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .gate_blocked_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .contract_violation_sites <=
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary
              .normalized_sites +
              result.parity_surface.error_diagnostics_recovery_summary
                  .gate_blocked_sites ==
          result.parity_surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      result.parity_surface.error_diagnostics_recovery_summary.deterministic;
  result.parity_surface.deterministic_result_like_lowering_handoff =
      result.deterministic_result_like_lowering_handoff &&
      result.parity_surface.result_like_lowering_summary.result_like_sites ==
          result.parity_surface.result_like_lowering_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_success_sites ==
          result.parity_surface
              .result_like_lowering_result_success_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_failure_sites ==
          result.parity_surface
              .result_like_lowering_result_failure_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_branch_sites ==
          result.parity_surface
              .result_like_lowering_result_branch_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_payload_sites ==
          result.parity_surface
              .result_like_lowering_result_payload_sites_total &&
      result.parity_surface.result_like_lowering_summary.normalized_sites ==
          result.parity_surface.result_like_lowering_normalized_sites_total &&
      result.parity_surface.result_like_lowering_summary.branch_merge_sites ==
          result.parity_surface.result_like_lowering_branch_merge_sites_total &&
      result.parity_surface.result_like_lowering_summary.contract_violation_sites ==
          result.parity_surface
              .result_like_lowering_contract_violation_sites_total &&
      result.parity_surface.result_like_lowering_summary.result_success_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_failure_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_branch_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_payload_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.normalized_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.branch_merge_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.contract_violation_sites <=
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.result_success_sites +
              result.parity_surface.result_like_lowering_summary
                  .result_failure_sites ==
          result.parity_surface.result_like_lowering_summary.normalized_sites &&
      result.parity_surface.result_like_lowering_summary.normalized_sites +
              result.parity_surface.result_like_lowering_summary
                  .branch_merge_sites ==
          result.parity_surface.result_like_lowering_summary.result_like_sites &&
      result.parity_surface.result_like_lowering_summary.deterministic;
  result.parity_surface.deterministic_unwind_cleanup_handoff =
      result.deterministic_unwind_cleanup_handoff &&
      result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites ==
          result.parity_surface.unwind_cleanup_sites_total &&
      result.parity_surface.unwind_cleanup_summary.exceptional_exit_sites ==
          result.parity_surface.unwind_cleanup_exceptional_exit_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_action_sites ==
          result.parity_surface.unwind_cleanup_action_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_scope_sites ==
          result.parity_surface.unwind_cleanup_scope_sites_total &&
      result.parity_surface.unwind_cleanup_summary.cleanup_resume_sites ==
          result.parity_surface.unwind_cleanup_resume_sites_total &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites ==
          result.parity_surface.unwind_cleanup_normalized_sites_total &&
      result.parity_surface.unwind_cleanup_summary.fail_closed_sites ==
          result.parity_surface.unwind_cleanup_fail_closed_sites_total &&
      result.parity_surface.unwind_cleanup_summary.contract_violation_sites ==
          result.parity_surface.unwind_cleanup_contract_violation_sites_total &&
      result.parity_surface.unwind_cleanup_summary.exceptional_exit_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_action_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_scope_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.cleanup_resume_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.fail_closed_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.contract_violation_sites <=
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.normalized_sites +
              result.parity_surface.unwind_cleanup_summary.fail_closed_sites ==
          result.parity_surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      result.parity_surface.unwind_cleanup_summary.deterministic;
  result.parity_surface.deterministic_async_continuation_handoff =
      result.deterministic_async_continuation_handoff &&
      result.parity_surface.async_continuation_summary.async_continuation_sites ==
          result.parity_surface.async_continuation_sites_total &&
      result.parity_surface.async_continuation_summary.async_keyword_sites ==
          result.parity_surface.async_continuation_async_keyword_sites_total &&
      result.parity_surface.async_continuation_summary.async_function_sites ==
          result.parity_surface.async_continuation_async_function_sites_total &&
      result.parity_surface.async_continuation_summary
              .continuation_allocation_sites ==
          result.parity_surface.async_continuation_allocation_sites_total &&
      result.parity_surface.async_continuation_summary.continuation_resume_sites ==
          result.parity_surface.async_continuation_resume_sites_total &&
      result.parity_surface.async_continuation_summary
              .continuation_suspend_sites ==
          result.parity_surface.async_continuation_suspend_sites_total &&
      result.parity_surface.async_continuation_summary.async_state_machine_sites ==
          result.parity_surface.async_continuation_state_machine_sites_total &&
      result.parity_surface.async_continuation_summary.normalized_sites ==
          result.parity_surface.async_continuation_normalized_sites_total &&
      result.parity_surface.async_continuation_summary.gate_blocked_sites ==
          result.parity_surface.async_continuation_gate_blocked_sites_total &&
      result.parity_surface.async_continuation_summary
              .contract_violation_sites ==
          result.parity_surface.async_continuation_contract_violation_sites_total &&
      result.parity_surface.async_continuation_summary.async_keyword_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.async_function_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_allocation_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_resume_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .continuation_suspend_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.async_state_machine_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.normalized_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.gate_blocked_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary
              .contract_violation_sites <=
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.normalized_sites +
              result.parity_surface.async_continuation_summary.gate_blocked_sites ==
          result.parity_surface.async_continuation_summary
              .async_continuation_sites &&
      result.parity_surface.async_continuation_summary.deterministic;
  result.parity_surface
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      result.deterministic_await_lowering_suspension_state_lowering_handoff &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_keyword_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_resume_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_resume_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_continuation_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_await_continuation_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_normalized_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .contract_violation_sites ==
          result.parity_surface
              .await_lowering_suspension_state_lowering_contract_violation_sites_total &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_keyword_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_resume_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_state_machine_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_continuation_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_point_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .gate_blocked_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .contract_violation_sites <=
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
              .normalized_sites +
              result.parity_surface.await_lowering_suspension_state_lowering_summary
                  .gate_blocked_sites ==
          result.parity_surface.await_lowering_suspension_state_lowering_summary
              .await_suspension_sites &&
      result.parity_surface.await_lowering_suspension_state_lowering_summary
          .deterministic;
  result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =
      result.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          result.parity_surface.symbol_graph_global_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          result.parity_surface.symbol_graph_function_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          result.parity_surface.symbol_graph_top_level_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          result.parity_surface.symbol_graph_nested_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          result.parity_surface.symbol_graph_scope_frames_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          result.parity_surface.symbol_graph_method_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          result.parity_surface.symbol_graph_method_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_method_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.parity_surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic;
  result.parity_surface.deterministic_method_lookup_override_conflict_handoff =
      result.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_lookup_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_lookup_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_lookup_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_override_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_override_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_override_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts ==
          result.parity_surface.method_lookup_override_conflict_override_conflicts_total &&
      result.parity_surface.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
          result.parity_surface.method_lookup_override_conflict_unresolved_base_interfaces_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      result.parity_surface.method_lookup_override_conflict_summary.deterministic;
  result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff =
      result.deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_default_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
              result.parity_surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.deterministic;
  result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  result.parity_surface.deterministic_block_literal_capture_semantics_handoff =
      result.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites ==
          result.parity_surface.block_literal_capture_semantics_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_parameter_entries ==
          result.parity_surface.block_literal_capture_semantics_parameter_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_capture_entries ==
          result.parity_surface.block_literal_capture_semantics_capture_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_body_statement_entries ==
          result.parity_surface.block_literal_capture_semantics_body_statement_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_empty_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites ==
          result.parity_surface.block_literal_capture_semantics_non_normalized_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_literal_capture_semantics_contract_violation_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff =
      result.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
          result.parity_surface.block_abi_invoke_trampoline_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_word_count_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_body_statement_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
          result.parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
              result.parity_surface.block_abi_invoke_trampoline_semantics_summary
                  .missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_storage_escape_handoff =
      result.deterministic_block_storage_escape_handoff &&
      result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites ==
          result.parity_surface.block_storage_escape_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_storage_escape_mutable_capture_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_storage_escape_byref_slot_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_storage_escape_parameter_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total ==
          result.parity_surface.block_storage_escape_capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_storage_escape_body_statement_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
          result.parity_surface.block_storage_escape_requires_byref_cells_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_escape_analysis_enabled_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites ==
          result.parity_surface.block_storage_escape_escape_to_heap_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites ==
          result.parity_surface.block_storage_escape_escape_profile_normalized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites ==
          result.parity_surface.block_storage_escape_byref_layout_symbolized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_storage_escape_contract_violation_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total <=
          result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total <=
          result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_copy_dispose_handoff =
      result.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites ==
          result.parity_surface.block_copy_dispose_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_copy_dispose_mutable_capture_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_copy_dispose_byref_slot_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_copy_dispose_parameter_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total ==
          result.parity_surface.block_copy_dispose_capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_copy_dispose_body_statement_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites ==
          result.parity_surface.block_copy_dispose_profile_normalized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_copy_dispose_contract_violation_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total <=
          result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total <=
          result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_determinism_perf_baseline_handoff =
      result.deterministic_block_determinism_perf_baseline_handoff &&
      result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites ==
          result.parity_surface.block_determinism_perf_baseline_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.baseline_weight_total ==
          result.parity_surface.block_determinism_perf_baseline_weight_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.parameter_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_parameter_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.capture_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_capture_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.body_statement_entries_total ==
          result.parity_surface.block_determinism_perf_baseline_body_statement_entries_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic_capture_sites ==
          result.parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.heavy_tier_sites ==
          result.parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.normalized_profile_sites ==
          result.parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.contract_violation_sites ==
          result.parity_surface.block_determinism_perf_baseline_contract_violation_sites_total &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic_capture_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.heavy_tier_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.normalized_profile_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.contract_violation_sites <=
          result.parity_surface.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.parity_surface.block_determinism_perf_baseline_summary.deterministic;
  result.parity_surface.deterministic_message_send_selector_lowering_handoff =
      result.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.message_send_selector_lowering_summary.message_send_sites ==
          result.parity_surface.message_send_selector_lowering_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites ==
          result.parity_surface.message_send_selector_lowering_unary_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_keyword_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
          result.parity_surface.message_send_selector_lowering_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
          result.parity_surface.message_send_selector_lowering_argument_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
          result.parity_surface.message_send_selector_lowering_normalized_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_form_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_arity_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_missing_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
          result.parity_surface.message_send_selector_lowering_contract_violation_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites +
              result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.deterministic;
  result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =
      result.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites ==
          result.parity_surface.dispatch_abi_marshalling_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_receiver_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots ==
          result.parity_surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
          result.parity_surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites ==
          result.parity_surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.deterministic;
  result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =
      result.deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.deterministic;
  result.parity_surface.deterministic_super_dispatch_method_family_handoff =
      result.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.super_dispatch_method_family_summary.message_send_sites ==
          result.parity_surface.super_dispatch_method_family_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
          result.parity_surface.super_dispatch_method_family_enabled_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_requires_class_context_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites ==
          result.parity_surface.super_dispatch_method_family_init_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites ==
          result.parity_surface.super_dispatch_method_family_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
          result.parity_surface.super_dispatch_method_family_mutable_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites ==
          result.parity_surface.super_dispatch_method_family_new_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_none_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_retained_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_related_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites ==
          result.parity_surface.super_dispatch_method_family_contract_violation_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.deterministic;
  result.parity_surface.deterministic_runtime_shim_host_link_handoff =
      result.deterministic_runtime_shim_host_link_handoff &&
      result.parity_surface.runtime_shim_host_link_summary.message_send_sites ==
          result.parity_surface.runtime_shim_host_link_message_send_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites ==
          result.parity_surface.runtime_shim_host_link_required_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.parity_surface.runtime_shim_host_link_elided_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_arg_slots_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total &&
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites ==
          result.parity_surface.runtime_shim_host_link_contract_violation_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_symbol &&
      result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
          result.parity_surface.runtime_shim_host_link_default_runtime_dispatch_symbol_binding &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites +
              result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.parity_surface.runtime_shim_host_link_summary.message_send_sites &&
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites <=
          result.parity_surface.runtime_shim_host_link_summary.message_send_sites &&
      (result.parity_surface.runtime_shim_host_link_summary.message_send_sites == 0 ||
       result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeShimHostLinkDefaultDispatchSymbol)) &&
      result.parity_surface.runtime_shim_host_link_summary.deterministic;
  result.parity_surface.deterministic_retain_release_operation_handoff =
      result.deterministic_retain_release_operation_handoff &&
      result.parity_surface.retain_release_operation_summary.ownership_qualified_sites ==
          result.parity_surface.retain_release_operation_ownership_qualified_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites ==
          result.parity_surface.retain_release_operation_retain_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites ==
          result.parity_surface.retain_release_operation_release_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites ==
          result.parity_surface.retain_release_operation_autorelease_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.contract_violation_sites ==
          result.parity_surface.retain_release_operation_contract_violation_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.deterministic;
  result.parity_surface.deterministic_weak_unowned_semantics_handoff =
      result.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites ==
          result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_reference_sites ==
          result.parity_surface.weak_unowned_semantics_weak_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
          result.parity_surface.weak_unowned_semantics_conflict_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites ==
          result.parity_surface.weak_unowned_semantics_contract_violation_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic;
  result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =
      result.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
          result.parity_surface.ownership_arc_diagnostic_candidate_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
          result.parity_surface.ownership_arc_fixit_available_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
          result.parity_surface.ownership_arc_profiled_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          result.parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
          result.parity_surface.ownership_arc_empty_fixit_hint_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites ==
          result.parity_surface.ownership_arc_contract_violation_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic;
  result.parity_surface.deterministic_autoreleasepool_scope_handoff =
      result.deterministic_autoreleasepool_scope_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.scope_sites ==
          result.parity_surface.autoreleasepool_scope_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites ==
          result.parity_surface.autoreleasepool_scope_symbolized_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites ==
          result.parity_surface.autoreleasepool_scope_contract_violation_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth ==
          result.parity_surface.autoreleasepool_scope_max_depth_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      (result.parity_surface.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.parity_surface.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.parity_surface.autoreleasepool_scope_summary.scope_sites) &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic;
  result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering = result.vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;
  result.parity_surface.ready =
      result.executed &&
      result.parity_surface.deterministic_parser_sema_conformance_matrix &&
      result.parity_surface.deterministic_parser_sema_conformance_corpus &&
      result.parity_surface.deterministic_parser_sema_performance_quality_guardrails &&
      result.parity_surface.deterministic_parser_sema_cross_lane_integration_sync &&
      result.parity_surface.deterministic_parser_sema_docs_runbook_sync &&
      result.parity_surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_edge_compatibility_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_integration_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_performance_shard1 &&
      result.parity_surface.deterministic_parser_sema_advanced_core_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_edge_compatibility_shard2 &&
      result.parity_surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      result.parity_surface.deterministic_parser_sema_integration_closeout_signoff &&
      IsReadyObjc3SemaPassFlowSummary(result.parity_surface.sema_pass_flow_summary) &&
      result.parity_surface.parser_sema_conformance_matrix.deterministic &&
      result.parity_surface.parser_sema_conformance_corpus.deterministic &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .deterministic &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .deterministic &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .deterministic &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .deterministic &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .deterministic &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .deterministic &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .deterministic &&
      result.parity_surface.parser_sema_integration_closeout_signoff
          .deterministic &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .required_guardrail_count == 7u &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .passed_guardrail_count ==
          result.parity_surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .failed_guardrail_count == 0u &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      result.parity_surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .required_sync_count == 4u &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .passed_sync_count ==
          result.parity_surface.parser_sema_cross_lane_integration_sync
              .required_sync_count &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .matrix_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .corpus_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      result.parity_surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .passed_sync_count ==
          result.parity_surface.parser_sema_docs_runbook_sync
              .required_sync_count &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .cross_lane_integration_sync_ready &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_docs_runbook_sync
          .parity_surface_sync &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .passed_sync_count ==
          result.parity_surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_release_candidate_replay_dry_run
          .replay_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_core_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .advanced_core_shard1_ready &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_diagnostics_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .advanced_edge_compatibility_shard1_ready &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_conformance_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_conformance_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_integration_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_integration_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_performance_shard1
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_performance_shard1
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_core_shard2
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_core_shard2
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .advanced_core_shard2_ready &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .pass_manager_contract_surface_sync &&
      result.parity_surface.parser_sema_advanced_edge_compatibility_shard2
          .shard_surface_sync &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .required_sync_count == 3u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .passed_sync_count ==
          result.parity_surface.parser_sema_advanced_diagnostics_shard2
              .required_sync_count &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .failed_sync_count == 0u &&
      result.parity_surface.parser_sema_advanced_diagnostics_shard2
          .advanced_edge_compatibility_shard2_ready &&
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
      result.parity_surface.runtime_shim_host_link_summary.deterministic &&
      result.parity_surface.deterministic_runtime_shim_host_link_handoff &&
      result.parity_surface.retain_release_operation_summary.deterministic &&
      result.parity_surface.deterministic_retain_release_operation_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic &&
      result.parity_surface.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      result.parity_surface.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic &&
      result.parity_surface.deterministic_autoreleasepool_scope_handoff;
  return result;
}
