  surface.bootstrap_legality_failure_contract_summary =
      BuildBootstrapLegalityFailureContractSummaryFromIntegrationSurface(
          surface);
  surface.bootstrap_legality_semantics_summary =
      BuildBootstrapLegalitySemanticsSummaryFromIntegrationSurface(surface);
  surface.bootstrap_failure_restart_semantics_summary =
      BuildBootstrapFailureRestartSemanticsSummaryFromIntegrationSurface(
          surface);
  // executable source-closure freeze anchor: lane-A freezes one
  // deterministic source closure over interface/implementation summaries plus
  // protocol/category composition and class/protocol/category linking before
  // later realization, conformance, and category-merge work make this
  // surface executable at runtime.
  // class/metaclass completion anchor: sema preserves the same
  // declaration-owned class/metaclass parent identities and method-owner split
  // so later realization/runtime lanes can consume one stable source model.
  // protocol/category completion anchor: sema preserves the same
  // protocol/category composition and class/protocol/category linking summaries
  // so attachment and conformance closure consume one stable source model.
  // object-model semantic-rule freeze anchor: sema remains the
  // authority for realization legality, inheritance legality, override
  // compatibility, declared protocol conformance, and deterministic category
  // merge policy; parser and IR only preserve the source identities this
  // semantic boundary consumes.
  // executable object artifact lowering freeze anchor: sema owns the
  // realized-object legality and canonical owner identities that lane-C binds
  // into emitted method-list/class/category records; sema must not synthesize
  // object-artifact slots or implementation symbol attachments itself.
  // executable method-body binding anchor: sema still stops at
  // canonical method owner identities and realized legality; IR/object
  // emission must fail closed if those implementation-owned method identities
  // do not attach to exactly one concrete LLVM body symbol.
  // executable realization-record expansion anchor: sema is also the
  // last stop for canonical superclass, class-object, metaclass-object,
  // protocol-inheritance, and category-attachment identities before lowering
  // serializes them into realization-ready records.
  // class-realization-runtime freeze anchor: sema remains the owner
  // of realized-class legality, category merge/conflict decisions, protocol
  // conformance legality, and canonical inheritance identities. Runtime class
  // realization must consume that closure rather than reinterpreting source.
  // metaclass-graph-root-class anchor: sema therefore preserves
  // root-class empties and canonical superclass/metaclass owner identities as
  // the only legal input for runtime-owned realized graph publication.
  // category-attachment-protocol-conformance anchor: sema also owns
  // the adopted-protocol closure and deterministic category merge surface that
  // runtime queries now consume from emitted refs rather than rebuilding from
  // source or diagnostics.
  // canonical-runnable-object-sample anchor: sema additionally owns
  // the fail-closed admission boundary for runtime-built alloc/new/init on
  // concrete object samples, including inherited class-method lookup through
  // the canonical superclass chain.
  // class-protocol-category conformance gate anchor: lane-E freezes
  // the aggregated evidence boundary over these sema-owned inheritance,
  // conformance, and category-merge decisions before execution-matrix
  // expansion in the later closeout step.
  // runnable class-protocol-category execution-matrix anchor: the
  // live matrix consumes these sema-owned inheritance, protocol-conformance,
  // and category-merge decisions directly when replaying the linked
  // inheritance executable case.
  surface.protocol_category_composition_summary = BuildProtocolCategoryCompositionSummaryFromSurface(surface);
  surface.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(surface.interface_implementation_summary,
                                              surface.protocol_category_composition_summary);
  surface.selector_normalization_summary = BuildSelectorNormalizationSummaryFromSurface(surface);
  surface.property_attribute_summary = BuildPropertyAttributeSummaryFromSurface(surface);
  surface.type_annotation_surface_summary = BuildTypeAnnotationSurfaceSummaryFromIntegrationSurface(surface);
  surface.lightweight_generic_constraint_summary =
      BuildLightweightGenericConstraintSummaryFromCanonicalTypes(surface);
  surface.nullability_flow_warning_precision_summary =
      BuildNullabilityFlowWarningPrecisionSummaryFromCanonicalTypes(surface);
  surface.protocol_qualified_object_type_summary =
      BuildProtocolQualifiedObjectTypeSummaryFromCanonicalTypes(surface);
  surface.variance_bridge_cast_summary =
      BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(
          surface.type_annotation_surface_summary,
          surface.protocol_qualified_object_type_summary);
  surface.generic_metadata_abi_summary =
      BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(
          surface.type_annotation_surface_summary,
          surface.lightweight_generic_constraint_summary,
          surface.variance_bridge_cast_summary);
  surface.module_import_graph_summary =
      BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(
          surface.type_annotation_surface_summary,
          surface.generic_metadata_abi_summary);
  surface.namespace_collision_shadowing_summary =
      BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(
          surface.module_import_graph_summary);
  surface.public_private_api_partition_summary =
      BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(
          surface.namespace_collision_shadowing_summary);
  surface.incremental_module_cache_invalidation_summary =
      BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(
          surface.public_private_api_partition_summary);
  surface.cross_module_conformance_summary =
      BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(
          surface.incremental_module_cache_invalidation_summary);
  surface.throws_propagation_summary =
      BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(
          surface.cross_module_conformance_summary);
  surface.async_continuation_summary =
      BuildAsyncContinuationSummaryFromIntegrationSurface(surface);
  surface.await_lowering_suspension_state_lowering_summary =
      BuildAwaitLoweringSuspensionStateSummaryFromProgramAst(ast);
  surface.actor_isolation_sendability_summary =
      BuildActorIsolationSendabilitySummaryFromIntegrationSurface(surface);
  surface.task_runtime_cancellation_summary =
      BuildTaskRuntimeCancellationSummaryFromIntegrationSurface(surface);
  surface.concurrency_replay_race_guard_summary =
      BuildConcurrencyReplayRaceGuardSummaryFromIntegrationSurface(surface);
  surface.result_like_lowering_summary =
      BuildResultLikeLoweringSummaryFromProgramAst(ast);
  surface.error_diagnostics_recovery_summary =
      BuildErrorDiagnosticsRecoverySummaryFromProgramAst(ast);
  surface.unwind_cleanup_summary =
      BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
          surface.throws_propagation_summary,
          surface.result_like_lowering_summary);
  surface.ns_error_bridging_summary =
      BuildNSErrorBridgingSummaryFromIntegrationSurface(surface);
  surface.symbol_graph_scope_resolution_summary = BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(surface);
  surface.method_lookup_override_conflict_summary =
      BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(surface);
  surface.property_synthesis_ivar_binding_summary =
      BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(surface);
  surface.id_class_sel_object_pointer_type_checking_summary =
      BuildIdClassSelObjectPointerTypeCheckingSummaryFromIntegrationSurface(surface);
  surface.block_literal_capture_sites_lexicographic =
      BuildBlockLiteralCaptureSiteMetadataLexicographic(ast);
  surface.block_literal_capture_semantics_summary =
      BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_abi_invoke_trampoline_sites_lexicographic =
      BuildBlockAbiInvokeTrampolineSiteMetadataLexicographic(ast);
  surface.block_abi_invoke_trampoline_semantics_summary =
      BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_storage_escape_sites_lexicographic =
      BuildBlockStorageEscapeSiteMetadataLexicographic(ast);
  surface.block_storage_escape_semantics_summary =
      BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_copy_dispose_sites_lexicographic =
      BuildBlockCopyDisposeSiteMetadataLexicographic(ast);
  surface.block_copy_dispose_semantics_summary =
      BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface(surface);
  surface.block_determinism_perf_baseline_sites_lexicographic =
      BuildBlockDeterminismPerfBaselineSiteMetadataLexicographic(ast);
  surface.block_determinism_perf_baseline_summary =
      BuildBlockDeterminismPerfBaselineSummaryFromIntegrationSurface(surface);
  surface.message_send_selector_lowering_sites_lexicographic =
      BuildMessageSendSelectorLoweringSiteMetadataLexicographic(ast);
  surface.message_send_selector_lowering_summary =
      BuildMessageSendSelectorLoweringSummaryFromIntegrationSurface(surface);
  surface.dispatch_abi_marshalling_summary = BuildDispatchAbiMarshallingSummaryFromIntegrationSurface(surface);
  surface.nil_receiver_semantics_foldability_summary =
      BuildNilReceiverSemanticsFoldabilitySummaryFromIntegrationSurface(surface);
  surface.super_dispatch_method_family_summary =
      BuildSuperDispatchMethodFamilySummaryFromIntegrationSurface(surface);
  surface.runtime_link_host_link_summary =
      BuildRuntimeLinkHostLinkSummaryFromIntegrationSurface(surface);
  surface.retain_release_operation_summary =
      BuildRetainReleaseOperationSummaryFromIntegrationSurface(surface);
  surface.weak_unowned_semantics_summary =
      BuildWeakUnownedSemanticsSummaryFromIntegrationSurface(surface);
  surface.unsafe_pointer_extension_summary =
      BuildUnsafePointerExtensionSummaryFromTypeAnnotationAndWeakUnownedSummaries(
          surface.type_annotation_surface_summary,
          surface.weak_unowned_semantics_summary);
  surface.inline_asm_intrinsic_governance_summary =
      BuildInlineAsmIntrinsicGovernanceSummaryFromUnsafePointerAndThrowsSummaries(
          surface.unsafe_pointer_extension_summary,
          surface.throws_propagation_summary);
  surface.arc_diagnostics_fixit_summary =
      BuildArcDiagnosticsFixitSummaryFromIntegrationSurface(surface);
  surface.autoreleasepool_scope_sites_lexicographic =
      BuildAutoreleasePoolScopeSiteMetadataLexicographic(ast);
  surface.autoreleasepool_scope_summary =
      BuildAutoreleasePoolScopeSummaryFromIntegrationSurface(surface);
