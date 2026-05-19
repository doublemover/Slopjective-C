  handoff.lightweight_generic_constraint_summary =
      BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  handoff.nullability_flow_warning_precision_summary =
      BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  handoff.protocol_qualified_object_type_summary =
      BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(
          handoff.type_annotation_surface_summary);
  handoff.variance_bridge_cast_summary =
      BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(
          handoff.type_annotation_surface_summary,
          handoff.protocol_qualified_object_type_summary);
  handoff.generic_metadata_abi_summary =
      BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(
          handoff.type_annotation_surface_summary,
          handoff.lightweight_generic_constraint_summary,
          handoff.variance_bridge_cast_summary);
  handoff.module_import_graph_summary =
      BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(
          handoff.type_annotation_surface_summary,
          handoff.generic_metadata_abi_summary);
  handoff.namespace_collision_shadowing_summary =
      BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(
          handoff.module_import_graph_summary);
  handoff.public_private_api_partition_summary =
      BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(
          handoff.namespace_collision_shadowing_summary);
  handoff.incremental_module_cache_invalidation_summary =
      BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(
          handoff.public_private_api_partition_summary);
  handoff.cross_module_conformance_summary =
      BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(
          handoff.incremental_module_cache_invalidation_summary);
  handoff.throws_propagation_summary =
      BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(
          handoff.cross_module_conformance_summary);
  handoff.async_continuation_summary =
      BuildAsyncContinuationSummaryFromTypeMetadataHandoff(handoff);
  handoff.await_lowering_suspension_state_lowering_summary =
      surface.await_lowering_suspension_state_lowering_summary;
  handoff.actor_isolation_sendability_summary =
      BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(handoff);
  handoff.task_runtime_cancellation_summary =
      BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff(handoff);
  handoff.concurrency_replay_race_guard_summary =
      BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(handoff);
  handoff.ns_error_bridging_summary =
      BuildNSErrorBridgingSummaryFromTypeMetadataHandoff(handoff);
  handoff.result_like_lowering_summary = surface.result_like_lowering_summary;
  handoff.error_diagnostics_recovery_summary =
      surface.error_diagnostics_recovery_summary;
  handoff.unwind_cleanup_summary =
      BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
          handoff.throws_propagation_summary,
          handoff.result_like_lowering_summary);
  handoff.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  handoff.method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  handoff.property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  handoff.id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_literal_capture_sites_lexicographic =
      surface.block_literal_capture_sites_lexicographic;
  handoff.block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_abi_invoke_trampoline_sites_lexicographic =
      surface.block_abi_invoke_trampoline_sites_lexicographic;
  handoff.block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_storage_escape_sites_lexicographic =
      surface.block_storage_escape_sites_lexicographic;
  handoff.block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_copy_dispose_sites_lexicographic =
      surface.block_copy_dispose_sites_lexicographic;
  handoff.block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.block_determinism_perf_baseline_sites_lexicographic =
      surface.block_determinism_perf_baseline_sites_lexicographic;
  handoff.block_determinism_perf_baseline_summary =
      BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff(handoff);
  handoff.message_send_selector_lowering_sites_lexicographic =
      surface.message_send_selector_lowering_sites_lexicographic;
  handoff.message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(handoff);
  handoff.dispatch_abi_marshalling_summary =
      BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(handoff);
  handoff.nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(handoff);
  handoff.super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(handoff);
  handoff.runtime_link_host_link_summary =
      BuildRuntimeLinkHostLinkSummaryFromTypeMetadataHandoff(handoff);
  handoff.retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(handoff);
  handoff.weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(handoff);
  handoff.unsafe_pointer_extension_summary =
      BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(
          handoff.type_annotation_surface_summary,
          handoff.weak_unowned_semantics_summary);
  handoff.inline_asm_intrinsic_governance_summary =
      BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(
          handoff.unsafe_pointer_extension_summary,
          handoff.throws_propagation_summary);
  handoff.arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(handoff);
  handoff.autoreleasepool_scope_sites_lexicographic =
      surface.autoreleasepool_scope_sites_lexicographic;
  handoff.autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(handoff);
