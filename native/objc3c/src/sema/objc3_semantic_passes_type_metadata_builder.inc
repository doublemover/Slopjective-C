                                           container_name +
                                           "' must define a body"));
      }

      Objc3MethodInfo method_info =
          BuildMethodInfo(method_decl, selector_contract, arc_mode_enabled,
                          &generic_interface_names);
      const auto method_insert =
          implementation_info.methods.emplace(selector, std::move(method_info));
      if (!method_insert.second) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S200",
                                       "duplicate " + container_label +
                                           " selector '" + selector + "' in " +
                                           container_label + " '" +
                                           container_name + "'"));
        continue;
      }

      if (!is_category_implementation) {
        ++interface_implementation_summary.implementation_method_symbols;
      }
      if (matched_interface == nullptr) {
        continue;
      }

      const auto interface_method_it = matched_interface->methods.find(selector);
      if (interface_method_it == matched_interface->methods.end()) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: implementation selector '" + selector + "' in '" +
                                           implementation_decl.name + "' is not declared in interface"));
        continue;
      }

      if (!IsCompatibleMethodSignature(interface_method_it->second, method_insert.first->second)) {
        diagnostics.push_back(MakeDiag(method_decl.line, method_decl.column, "O3S206",
                                       "type mismatch: incompatible method signature for selector '" + selector +
                                           "' in implementation '" + implementation_decl.name + "'"));
        continue;
      }

      if (!is_category_implementation) {
        ++interface_implementation_summary.linked_implementation_symbols;
      }
    }

    if (!is_category_implementation) {
      surface.implementations.emplace(implementation_decl.name,
                                      std::move(implementation_info));
    } else {
      const std::string category_owner_identity =
          BuildCategoryOwnerIdentity(implementation_decl.name,
                                     implementation_decl.category_name);
      if (!surface.category_implementations
               .emplace(category_owner_identity, std::move(implementation_info))
               .second) {
        diagnostics.push_back(MakeDiag(implementation_decl.line,
                                       implementation_decl.column,
                                       "O3S200",
                                       "duplicate category implementation '" +
                                           container_name + "'"));
      }
    }
  }

  // protocol-conformance enforcement anchor: parser-owned
  // @required/@optional partitions are now enforced here against class and
  // category adoption before runtime metadata consumption can proceed.
  ValidateAndBuildCategoryMergeSurfaces(ast, surface, diagnostics);
  ValidateDeclaredProtocolConformance(ast, surface, protocol_definitions,
                                      surface.category_interfaces,
                                      surface.category_implementations,
                                      diagnostics);
  ValidateInteropInteropRuntimeParity(ast, diagnostics);
  ValidateInteropCppInteropInteractions(ast, diagnostics);
  ValidateInteropSwiftInteropIsolation(ast, diagnostics);
  ValidateInheritanceOverrideAndRealizationLegality(ast, surface, diagnostics);
  ValidateDispatchDispatchIntentCompatibility(ast, diagnostics);

  interface_implementation_summary.resolved_interfaces = surface.interfaces.size();
  interface_implementation_summary.resolved_implementations = surface.implementations.size();
  interface_implementation_summary.deterministic =
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.implementation_method_symbols &&
      interface_implementation_summary.linked_implementation_symbols <=
          interface_implementation_summary.interface_method_symbols;
  surface.interface_implementation_summary = interface_implementation_summary;
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
      BuildLightweightGenericConstraintSummaryFromTypeAnnotationSurfaceSummary(
          surface.type_annotation_surface_summary);
  surface.nullability_flow_warning_precision_summary =
      BuildNullabilityFlowWarningPrecisionSummaryFromTypeAnnotationSurfaceSummary(
          surface.type_annotation_surface_summary);
  surface.protocol_qualified_object_type_summary =
      BuildProtocolQualifiedObjectTypeSummaryFromTypeAnnotationSurfaceSummary(
          surface.type_annotation_surface_summary);
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
  // unsupported-feature enforcement anchor: accepted source sites
  // that would over-claim unsupported runtime-backed features must fail closed
  // in sema before lowering/runtime handoff proceeds.
  const Objc3UnsupportedFeatureClaimEnforcementStats
      unsupported_feature_enforcement =
          DiagnoseUnsupportedFeatureClaimSources(
              ast,
              allow_source_only_block_literals,
              allow_source_only_defer_statements,
              allow_source_only_error_runtime_surface,
              arc_mode_enabled,
              diagnostics);
  // semantic freeze anchor: sema owns the single fail-closed
  // legality summary that classifies live compatibility/migration selections,
  // source-only claim downgrades, and unsupported strictness/macro claim
  // rejections before later lowering/runtime/reporting lanes consume them.
  surface.compatibility_strictness_claim_semantics_summary =
      BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface(
          surface,
          unsupported_feature_enforcement);
  surface.built = true;
  return surface;
}

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface) {
  Objc3SemanticTypeMetadataHandoff handoff;
  handoff.global_names_lexicographic.reserve(surface.globals.size());
  for (const auto &global : surface.globals) {
    handoff.global_names_lexicographic.push_back(global.first);
  }
  std::sort(handoff.global_names_lexicographic.begin(), handoff.global_names_lexicographic.end());

#include "sema/objc3_semantic_passes_type_metadata_function_builder.inc"

  std::vector<std::string> interface_names;
  interface_names.reserve(surface.interfaces.size());
  for (const auto &entry : surface.interfaces) {
    interface_names.push_back(entry.first);
  }
  std::sort(interface_names.begin(), interface_names.end());

  handoff.interfaces_lexicographic.reserve(interface_names.size());
  // ARC lowering ABI/cleanup freeze anchor: sema continues to own
  // ARC legality, inferred lifetime packets, and synthesized property ownership
  // metadata only; it does not schedule cleanups or choose lowering helper
  // placement.
  // ARC automatic-insertion implementation anchor: sema now also
  // exports the supported ARC insertion flags and normalized property ownership
  // metadata that lane C consumes for helper placement, while helper emission
  // itself remains lowering-owned.
  const auto apply_arc_property_interaction_metadata =
      [](Objc3SemanticPropertyTypeMetadata &property_metadata) {
        const objc3c::support::Objc3PropertyOwnershipProfileKind
            ownership_profile_kind =
                objc3c::support::ApplyPropertyOwnershipProfileFallback(
                    property_metadata, false);
        if (ownership_profile_kind ==
            objc3c::support::Objc3PropertyOwnershipProfileKind::kWeak) {
          property_metadata.ownership_is_weak_reference = true;
        }
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(
            property_metadata);
      };
  for (const std::string &name : interface_names) {
    const auto interface_it = surface.interfaces.find(name);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }

    Objc3SemanticInterfaceTypeMetadata metadata;
    metadata.name = name;
    metadata.super_name = interface_it->second.super_name;
    metadata.generic_parameter_names_source_order =
        interface_it->second.generic_parameter_names_source_order;
    metadata.generic_parameter_variance_source_order =
        interface_it->second.generic_parameter_variance_source_order;
    metadata.adopted_protocols_lexicographic =
        interface_it->second.adopted_protocols_lexicographic;

#include "sema/objc3_semantic_passes_type_metadata_interface_properties.inc"

#include "sema/objc3_semantic_passes_type_metadata_interface_methods.inc"

    handoff.interfaces_lexicographic.push_back(std::move(metadata));
  }

  std::vector<std::string> implementation_names;
  implementation_names.reserve(surface.implementations.size());
  for (const auto &entry : surface.implementations) {
    implementation_names.push_back(entry.first);
  }
  std::sort(implementation_names.begin(), implementation_names.end());

  handoff.implementations_lexicographic.reserve(implementation_names.size());
  for (const std::string &name : implementation_names) {
    const auto implementation_it = surface.implementations.find(name);
    if (implementation_it == surface.implementations.end()) {
      continue;
    }

    Objc3SemanticImplementationTypeMetadata metadata;
    metadata.name = name;
    metadata.has_matching_interface = implementation_it->second.has_matching_interface;

#include "sema/objc3_semantic_passes_type_metadata_implementation_properties.inc"

#include "sema/objc3_semantic_passes_type_metadata_implementation_methods.inc"

    handoff.implementations_lexicographic.push_back(std::move(metadata));
  }

#include "sema/objc3_semantic_passes_type_metadata_interface_implementation_summary.inc"

#include "sema/objc3_semantic_passes_type_metadata_selector_summary.inc"
