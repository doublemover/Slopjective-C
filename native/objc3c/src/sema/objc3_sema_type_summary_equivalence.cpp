#include "sema/objc3_sema_type_summary_equivalence.h"

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

bool IsEquivalentVarianceBridgeCastSummary(const Objc3VarianceBridgeCastSummary &lhs,
                                           const Objc3VarianceBridgeCastSummary &rhs) {
  return lhs.variance_bridge_cast_sites == rhs.variance_bridge_cast_sites &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentGenericMetadataAbiSummary(const Objc3GenericMetadataAbiSummary &lhs,
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

bool IsEquivalentModuleImportGraphSummary(const Objc3ModuleImportGraphSummary &lhs,
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
  return lhs.namespace_collision_shadowing_sites == rhs.namespace_collision_shadowing_sites &&
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
  return lhs.public_private_api_partition_sites == rhs.public_private_api_partition_sites &&
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
  return lhs.incremental_module_cache_invalidation_sites == rhs.incremental_module_cache_invalidation_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites == rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentCrossModuleConformanceSummary(const Objc3CrossModuleConformanceSummary &lhs,
                                               const Objc3CrossModuleConformanceSummary &rhs) {
  return lhs.cross_module_conformance_sites == rhs.cross_module_conformance_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites == rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentThrowsPropagationSummary(const Objc3ThrowsPropagationSummary &lhs,
                                          const Objc3ThrowsPropagationSummary &rhs) {
  return lhs.throws_propagation_sites == rhs.throws_propagation_sites &&
         lhs.namespace_segment_sites == rhs.namespace_segment_sites &&
         lhs.import_edge_candidate_sites == rhs.import_edge_candidate_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.cache_invalidation_candidate_sites == rhs.cache_invalidation_candidate_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAsyncContinuationSummary(const Objc3AsyncContinuationSummary &lhs,
                                          const Objc3AsyncContinuationSummary &rhs) {
  return lhs.async_continuation_sites == rhs.async_continuation_sites &&
         lhs.async_keyword_sites == rhs.async_keyword_sites &&
         lhs.async_function_sites == rhs.async_function_sites &&
         lhs.continuation_allocation_sites == rhs.continuation_allocation_sites &&
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
  return lhs.actor_isolation_sendability_sites == rhs.actor_isolation_sendability_sites &&
         lhs.actor_isolation_decl_sites == rhs.actor_isolation_decl_sites &&
         lhs.actor_hop_sites == rhs.actor_hop_sites &&
         lhs.sendable_annotation_sites == rhs.sendable_annotation_sites &&
         lhs.non_sendable_crossing_sites == rhs.non_sendable_crossing_sites &&
         lhs.isolation_boundary_sites == rhs.isolation_boundary_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentTaskRuntimeCancellationSummary(const Objc3TaskRuntimeCancellationSummary &lhs,
                                                const Objc3TaskRuntimeCancellationSummary &rhs) {
  return lhs.task_runtime_interop_sites == rhs.task_runtime_interop_sites &&
         lhs.runtime_hook_sites == rhs.runtime_hook_sites &&
         lhs.cancellation_check_sites == rhs.cancellation_check_sites &&
         lhs.cancellation_handler_sites == rhs.cancellation_handler_sites &&
         lhs.suspension_point_sites == rhs.suspension_point_sites &&
         lhs.cancellation_propagation_sites == rhs.cancellation_propagation_sites &&
         lhs.normalized_sites == rhs.normalized_sites &&
         lhs.gate_blocked_sites == rhs.gate_blocked_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}
