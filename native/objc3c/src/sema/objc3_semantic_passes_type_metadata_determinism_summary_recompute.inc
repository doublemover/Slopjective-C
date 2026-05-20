  std::size_t interface_method_symbols = 0;
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interface_method_symbols += metadata.methods_lexicographic.size();
  }
  std::size_t implementation_method_symbols = 0;
  for (const auto &metadata : handoff.implementations_lexicographic) {
    implementation_method_symbols += metadata.methods_lexicographic.size();
  }
  const Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_summary =
      BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(handoff);
  const Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary =
      BuildLightweightGenericConstraintSummaryFromCanonicalTypes(handoff);
  const Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary =
      BuildNullabilityFlowWarningPrecisionSummaryFromCanonicalTypes(handoff);
  const Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary =
      BuildProtocolQualifiedObjectTypeSummaryFromCanonicalTypes(handoff);
  const Objc3VarianceBridgeCastSummary variance_bridge_cast_summary =
      BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(
          handoff.type_annotation_surface_summary,
          handoff.protocol_qualified_object_type_summary);
  const Objc3GenericMetadataAbiSummary generic_metadata_abi_summary =
      BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(
          handoff.type_annotation_surface_summary,
          handoff.lightweight_generic_constraint_summary,
          handoff.variance_bridge_cast_summary);
  const Objc3ModuleImportGraphSummary module_import_graph_summary =
      BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(
          handoff.type_annotation_surface_summary,
          handoff.generic_metadata_abi_summary);
  const Objc3NamespaceCollisionShadowingSummary
      namespace_collision_shadowing_summary =
          BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(
              handoff.module_import_graph_summary);
  const Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary =
      BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(
          handoff.namespace_collision_shadowing_summary);
  const Objc3IncrementalModuleCacheInvalidationSummary
      incremental_module_cache_invalidation_summary =
          BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(
              handoff.public_private_api_partition_summary);
  const Objc3CrossModuleConformanceSummary cross_module_conformance_summary =
      BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(
          handoff.incremental_module_cache_invalidation_summary);
  const Objc3ThrowsPropagationSummary throws_propagation_summary =
      BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(
          handoff.cross_module_conformance_summary);
  const Objc3AsyncContinuationSummary async_continuation_summary =
      BuildAsyncContinuationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3AwaitLoweringSuspensionStateSummary
      await_lowering_suspension_state_lowering_summary =
          handoff.await_lowering_suspension_state_lowering_summary;
  const Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary =
      BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(handoff);
  const Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary =
      BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3ConcurrencyReplayRaceGuardSummary
      concurrency_replay_race_guard_summary =
          BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(handoff);
  const Objc3NSErrorBridgingSummary ns_error_bridging_summary =
      BuildNSErrorBridgingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3ResultLikeLoweringSummary result_like_lowering_summary =
      handoff.result_like_lowering_summary;
  const Objc3ErrorDiagnosticsRecoverySummary
      error_diagnostics_recovery_summary =
          handoff.error_diagnostics_recovery_summary;
  const Objc3UnwindCleanupSummary unwind_cleanup_summary =
      handoff.unwind_cleanup_summary;
  const Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(handoff);
  // property-ivar executable-source-closure anchor:
  // freeze the source-surface replay key above layout/accessor realization.
  const Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary =
      BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff(handoff);
  const Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromTypeMetadataHandoff(handoff);
  const Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary =
      BuildDispatchAbiMarshallingSummaryFromTypeMetadataHandoff(handoff);
  const Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromTypeMetadataHandoff(handoff);
  const Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromTypeMetadataHandoff(handoff);
  const Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary =
      BuildRuntimeLinkHostLinkSummaryFromTypeMetadataHandoff(handoff);
  const Objc3RetainReleaseOperationSummary retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromTypeMetadataHandoff(handoff);
  const Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromTypeMetadataHandoff(handoff);
  const Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary =
      BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(
          handoff.type_annotation_surface_summary,
          handoff.weak_unowned_semantics_summary);
  const Objc3InlineAsmIntrinsicGovernanceSummary
      inline_asm_intrinsic_governance_summary =
          BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(
              handoff.unsafe_pointer_extension_summary,
              handoff.throws_propagation_summary);
  // semantic anchor: Part 12 advanced diagnostic taxonomy,
  // portability publication, and feature-specific fix-it synthesis all consume
  // this existing deterministic ARC/fix-it semantic slice as the live
  // baseline.
  const Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromTypeMetadataHandoff(handoff);
  const Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromTypeMetadataHandoff(handoff);
  const Objc3InterfaceImplementationSummary &summary = handoff.interface_implementation_summary;
  const Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(summary, protocol_category_summary);
