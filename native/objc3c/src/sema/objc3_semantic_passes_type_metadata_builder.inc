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

    std::vector<std::string> property_names;
    property_names.reserve(interface_it->second.properties.size());
    for (const auto &property_entry : interface_it->second.properties) {
      property_names.push_back(property_entry.first);
    }
    std::sort(property_names.begin(), property_names.end());

    metadata.properties_lexicographic.reserve(property_names.size());
    for (const std::string &property_name : property_names) {
      const auto property_it = interface_it->second.properties.find(property_name);
      if (property_it == interface_it->second.properties.end()) {
        continue;
      }
      const Objc3PropertyInfo &source = property_it->second;
      Objc3SemanticPropertyTypeMetadata property_metadata;
      property_metadata.name = property_name;
      property_metadata.type = source.type;
      property_metadata.canonical_type = source.canonical_type;
      property_metadata.is_vector = source.is_vector;
      property_metadata.vector_base_spelling = source.vector_base_spelling;
      property_metadata.vector_lane_count = source.vector_lane_count;
      property_metadata.id_spelling = source.id_spelling;
      property_metadata.class_spelling = source.class_spelling;
      property_metadata.instancetype_spelling = source.instancetype_spelling;
      property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;
      property_metadata.has_generic_suffix = source.has_generic_suffix;
      property_metadata.has_pointer_declarator = source.has_pointer_declarator;
      property_metadata.has_nullability_suffix = source.has_nullability_suffix;
      property_metadata.has_ownership_qualifier = source.has_ownership_qualifier;
      property_metadata.has_invalid_generic_suffix = source.has_invalid_generic_suffix;
      property_metadata.has_invalid_pointer_declarator = source.has_invalid_pointer_declarator;
      property_metadata.has_invalid_nullability_suffix = source.has_invalid_nullability_suffix;
      property_metadata.has_invalid_ownership_qualifier = source.has_invalid_ownership_qualifier;
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.ownership_insert_retain = source.ownership_insert_retain;
      property_metadata.ownership_insert_release = source.ownership_insert_release;
      property_metadata.ownership_insert_autorelease = source.ownership_insert_autorelease;
      property_metadata.ownership_is_weak_reference = source.ownership_is_weak_reference;
      property_metadata.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
      property_metadata.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
      property_metadata.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
      property_metadata.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
      property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
      property_metadata.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
      property_metadata.ownership_lifetime_profile = source.ownership_lifetime_profile;
      property_metadata.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_retain = source.is_retain;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
      property_metadata.is_unowned = source.is_unowned;
      property_metadata.is_unsafe_unretained = source.is_unsafe_unretained;
      property_metadata.is_assign = source.is_assign;
      property_metadata.is_nullable = source.is_nullable;
      property_metadata.is_nonnull = source.is_nonnull;
      property_metadata.is_null_resettable = source.is_null_resettable;
      property_metadata.is_class = source.is_class;
      property_metadata.is_direct = source.is_direct;
      property_metadata.has_getter = source.has_getter;
      property_metadata.has_setter = source.has_setter;
      property_metadata.getter_selector = source.getter_selector;
      property_metadata.setter_selector = source.setter_selector;
      property_metadata.ivar_binding_symbol = source.ivar_binding_symbol;
      property_metadata.executable_synthesized_binding_kind =
          source.executable_synthesized_binding_kind;
      property_metadata.executable_synthesized_binding_symbol =
          source.executable_synthesized_binding_symbol;
      // runtime-backed ownership attribute surface anchor: sema
      // remains the sole authority for property/member ownership profiles.
      // Lowering and runtime-facing metadata must consume these already
      // normalized attribute/lifetime/hook/accessor strings directly rather
      // than reconstructing ownership out of source text or manifests.
      property_metadata.property_attribute_profile =
          source.property_attribute_profile;
      // synthesized accessor/property lowering anchor: sema remains
      // the only authority for effective accessor selectors and synthesized
      // binding identities that lane-C may turn into executable bodies.
      // runtime property/layout consumption freeze anchor: the live
      // runtime must consume these same emitted selector/binding/layout facts
      // without recovering property storage or allocator behavior from source.
      // instance-allocation-layout-runtime anchor: later runtime
      // allocation and per-instance slot storage must still consume only these
      // emitted selector/binding/layout facts without reopening sema.
      // property-metadata-reflection anchor: runtime reflection
      // helpers must also publish property/accessor/layout facts from this
      // emitted surface rather than re-querying sema or source text.
      // property-ivar-execution gate anchor: lane-E now consumes the
      // A002/B003/C003/D003 proof chain before runnable sample expansion, so
      // this emitted property surface remains the only authoritative handoff.
      // runnable property-ivar execution-matrix anchor: lane-E
      // broadens the frozen gate only through one live sample that still
      // consumes this emitted property surface.
      property_metadata.effective_getter_selector =
          source.effective_getter_selector;
      property_metadata.effective_setter_available =
          source.effective_setter_available;
      property_metadata.effective_setter_selector =
          source.effective_setter_selector;
      // ownership runtime hook emission anchor: lowering now treats
      // the sema-approved lifetime/runtime-hook/accessor-ownership trio as the
      // authoritative executable hook surface for synthesized property access.
      // runtime memory-management API freeze anchor: sema still only
      // publishes the canonical ownership profile surface, while the actual
      // retain/release/autorelease/weak helper ABI stays private to runtime
      // lowering and must not widen the public runtime header here.
      property_metadata.accessor_ownership_profile =
          source.accessor_ownership_profile;
      apply_arc_property_interaction_metadata(property_metadata);
      property_metadata.executable_ivar_layout_symbol =
          source.executable_ivar_layout_symbol;
      property_metadata.executable_ivar_layout_slot_index =
          source.executable_ivar_layout_slot_index;
      property_metadata.executable_ivar_layout_size_bytes =
          source.executable_ivar_layout_size_bytes;
      property_metadata.executable_ivar_layout_alignment_bytes =
          source.executable_ivar_layout_alignment_bytes;
      property_metadata.executable_ivar_layout_offset_bytes =
          source.executable_ivar_layout_offset_bytes;
      property_metadata.executable_ivar_layout_padding_bytes =
          source.executable_ivar_layout_padding_bytes;
      property_metadata.executable_ivar_layout_inherited_slot_count =
          source.executable_ivar_layout_inherited_slot_count;
      property_metadata.executable_ivar_layout_inherited_size_bytes =
          source.executable_ivar_layout_inherited_size_bytes;
      property_metadata.executable_ivar_layout_owner_size_bytes =
          source.executable_ivar_layout_owner_size_bytes;
      property_metadata.executable_ivar_init_order_index =
          source.executable_ivar_init_order_index;
      property_metadata.executable_ivar_destroy_order_index =
          source.executable_ivar_destroy_order_index;
      property_metadata.executable_ivar_layout_valid =
          source.executable_ivar_layout_valid;
      property_metadata.executable_ivar_layout_replay_key =
          source.executable_ivar_layout_replay_key;
      property_metadata.invalid_attribute_entries = source.invalid_attribute_entries;
      property_metadata.property_contract_violations = source.property_contract_violations;
      property_metadata.has_unknown_attribute = source.has_unknown_attribute;
      property_metadata.has_duplicate_attribute = source.has_duplicate_attribute;
      property_metadata.has_readwrite_conflict = source.has_readwrite_conflict;
      property_metadata.has_atomicity_conflict = source.has_atomicity_conflict;
      property_metadata.has_ownership_conflict = source.has_ownership_conflict;
      property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;
      property_metadata.has_accessor_selector_contract_violation = source.has_accessor_selector_contract_violation;
      property_metadata.has_invalid_attribute_contract = source.has_invalid_attribute_contract;
      metadata.properties_lexicographic.push_back(std::move(property_metadata));
    }

    std::vector<std::string> selectors;
    selectors.reserve(interface_it->second.methods.size());
    for (const auto &method_entry : interface_it->second.methods) {
      selectors.push_back(method_entry.first);
    }
    std::sort(selectors.begin(), selectors.end());

    metadata.methods_lexicographic.reserve(selectors.size());
    for (const std::string &selector : selectors) {
      const auto method_it = interface_it->second.methods.find(selector);
      if (method_it == interface_it->second.methods.end()) {
        continue;
      }
      const Objc3MethodInfo &source = method_it->second;
      Objc3SemanticMethodTypeMetadata method_metadata;
      method_metadata.selector = selector;
      method_metadata.selector_normalized = source.selector_normalized;
      method_metadata.selector_piece_count = source.selector_piece_count;
      method_metadata.selector_parameter_piece_count = source.selector_parameter_piece_count;
      method_metadata.selector_contract_normalized = source.selector_contract_normalized;
      method_metadata.selector_had_pieceless_form = source.selector_had_pieceless_form;
      method_metadata.selector_has_spelling_mismatch = source.selector_has_spelling_mismatch;
      method_metadata.selector_has_arity_mismatch = source.selector_has_arity_mismatch;
      method_metadata.selector_has_parameter_linkage_mismatch = source.selector_has_parameter_linkage_mismatch;
      method_metadata.selector_has_normalization_flag_mismatch = source.selector_has_normalization_flag_mismatch;
      method_metadata.selector_has_missing_piece_keyword = source.selector_has_missing_piece_keyword;
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_canonical_types = source.param_canonical_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_generic_suffix = source.param_has_generic_suffix;
      method_metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
      method_metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
      method_metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
      method_metadata.param_id_spelling = source.param_id_spelling;
      method_metadata.param_class_spelling = source.param_class_spelling;
      method_metadata.param_instancetype_spelling = source.param_instancetype_spelling;
      method_metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
      method_metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
      method_metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
      method_metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
      method_metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
      method_metadata.param_ownership_insert_release = source.param_ownership_insert_release;
      method_metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
      method_metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
      method_metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
      method_metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
      method_metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
      method_metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
      method_metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
      method_metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
      method_metadata.return_has_generic_suffix = source.return_has_generic_suffix;
      method_metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
      method_metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
      method_metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
      method_metadata.return_id_spelling = source.return_id_spelling;
      method_metadata.return_class_spelling = source.return_class_spelling;
      method_metadata.return_instancetype_spelling = source.return_instancetype_spelling;
      method_metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
      method_metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
      method_metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
      method_metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
      method_metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
      method_metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
      method_metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
      method_metadata.return_ownership_insert_release = source.return_ownership_insert_release;
      method_metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
      method_metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
      method_metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
      method_metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
      method_metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
      method_metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
      method_metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
      method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
      method_metadata.return_type = source.return_type;
      method_metadata.return_canonical_type = source.return_canonical_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
      method_metadata.return_has_protocol_composition = source.return_has_protocol_composition;
      method_metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
      method_metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
      method_metadata.async_continuation_profile_is_normalized =
          source.async_continuation_profile_is_normalized;
      method_metadata.deterministic_async_continuation_handoff =
          source.deterministic_async_continuation_handoff;
      method_metadata.async_continuation_sites =
          source.async_continuation_sites;
      method_metadata.async_keyword_sites = source.async_keyword_sites;
      method_metadata.async_function_sites = source.async_function_sites;
      method_metadata.continuation_allocation_sites =
          source.continuation_allocation_sites;
      method_metadata.continuation_resume_sites =
          source.continuation_resume_sites;
      method_metadata.continuation_suspend_sites =
          source.continuation_suspend_sites;
      method_metadata.async_state_machine_sites =
          source.async_state_machine_sites;
      method_metadata.async_continuation_normalized_sites =
          source.async_continuation_normalized_sites;
      method_metadata.async_continuation_gate_blocked_sites =
          source.async_continuation_gate_blocked_sites;
      method_metadata.async_continuation_contract_violation_sites =
          source.async_continuation_contract_violation_sites;
      method_metadata.actor_isolation_sendability_profile_is_normalized =
          source.actor_isolation_sendability_profile_is_normalized;
      method_metadata.deterministic_actor_isolation_sendability_handoff =
          source.deterministic_actor_isolation_sendability_handoff;
      method_metadata.actor_isolation_sendability_sites =
          source.actor_isolation_sendability_sites;
      method_metadata.actor_isolation_decl_sites =
          source.actor_isolation_decl_sites;
      method_metadata.actor_hop_sites = source.actor_hop_sites;
      method_metadata.sendable_annotation_sites =
          source.sendable_annotation_sites;
      method_metadata.non_sendable_crossing_sites =
          source.non_sendable_crossing_sites;
      method_metadata.isolation_boundary_sites = source.isolation_boundary_sites;
      method_metadata.actor_isolation_sendability_normalized_sites =
          source.actor_isolation_sendability_normalized_sites;
      method_metadata.actor_isolation_sendability_gate_blocked_sites =
          source.actor_isolation_sendability_gate_blocked_sites;
      method_metadata.actor_isolation_sendability_contract_violation_sites =
          source.actor_isolation_sendability_contract_violation_sites;
      method_metadata.task_runtime_cancellation_profile_is_normalized =
          source.task_runtime_cancellation_profile_is_normalized;
      method_metadata.deterministic_task_runtime_cancellation_handoff =
          source.deterministic_task_runtime_cancellation_handoff;
      method_metadata.task_runtime_interop_sites =
          source.task_runtime_interop_sites;
      method_metadata.runtime_hook_sites = source.runtime_hook_sites;
      method_metadata.cancellation_check_sites =
          source.cancellation_check_sites;
      method_metadata.cancellation_handler_sites =
          source.cancellation_handler_sites;
      method_metadata.suspension_point_sites = source.suspension_point_sites;
      method_metadata.cancellation_propagation_sites =
          source.cancellation_propagation_sites;
      method_metadata.task_runtime_cancellation_normalized_sites =
          source.task_runtime_cancellation_normalized_sites;
      method_metadata.task_runtime_cancellation_gate_blocked_sites =
          source.task_runtime_cancellation_gate_blocked_sites;
      method_metadata.task_runtime_cancellation_contract_violation_sites =
          source.task_runtime_cancellation_contract_violation_sites;
      method_metadata.concurrency_replay_race_guard_profile_is_normalized =
          source.concurrency_replay_race_guard_profile_is_normalized;
      method_metadata.deterministic_concurrency_replay_race_guard_handoff =
          source.deterministic_concurrency_replay_race_guard_handoff;
      method_metadata.concurrency_replay_race_guard_sites =
          source.concurrency_replay_race_guard_sites;
      method_metadata.concurrency_replay_sites = source.concurrency_replay_sites;
      method_metadata.replay_proof_sites = source.replay_proof_sites;
      method_metadata.race_guard_sites = source.race_guard_sites;
      method_metadata.task_handoff_sites = source.task_handoff_sites;
      method_metadata.actor_isolation_sites = source.actor_isolation_sites;
      method_metadata.deterministic_schedule_sites =
          source.deterministic_schedule_sites;
      method_metadata.concurrency_replay_guard_blocked_sites =
          source.concurrency_replay_guard_blocked_sites;
      method_metadata.concurrency_replay_contract_violation_sites =
          source.concurrency_replay_contract_violation_sites;
      method_metadata.ns_error_bridging_profile_is_normalized =
          source.ns_error_bridging_profile_is_normalized;
      method_metadata.deterministic_ns_error_bridging_lowering_handoff =
          source.deterministic_ns_error_bridging_lowering_handoff;
      method_metadata.ns_error_bridging_sites = source.ns_error_bridging_sites;
      method_metadata.ns_error_parameter_sites = source.ns_error_parameter_sites;
      method_metadata.ns_error_out_parameter_sites =
          source.ns_error_out_parameter_sites;
      method_metadata.ns_error_bridge_path_sites =
          source.ns_error_bridge_path_sites;
      method_metadata.failable_call_sites = source.failable_call_sites;
      method_metadata.ns_error_bridging_normalized_sites =
          source.ns_error_bridging_normalized_sites;
      method_metadata.ns_error_bridge_boundary_sites =
          source.ns_error_bridge_boundary_sites;
      method_metadata.ns_error_bridging_contract_violation_sites =
          source.ns_error_bridging_contract_violation_sites;
      method_metadata.is_class_method = source.is_class_method;
      method_metadata.has_definition = source.has_definition;
      metadata.methods_lexicographic.push_back(std::move(method_metadata));
    }

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

    std::vector<std::string> property_names;
    property_names.reserve(implementation_it->second.properties.size());
    for (const auto &property_entry : implementation_it->second.properties) {
      property_names.push_back(property_entry.first);
    }
    std::sort(property_names.begin(), property_names.end());

    metadata.properties_lexicographic.reserve(property_names.size());
    for (const std::string &property_name : property_names) {
      const auto property_it = implementation_it->second.properties.find(property_name);
      if (property_it == implementation_it->second.properties.end()) {
        continue;
      }
      const Objc3PropertyInfo &source = property_it->second;
      Objc3SemanticPropertyTypeMetadata property_metadata;
      property_metadata.name = property_name;
      property_metadata.type = source.type;
      property_metadata.canonical_type = source.canonical_type;
      property_metadata.is_vector = source.is_vector;
      property_metadata.vector_base_spelling = source.vector_base_spelling;
      property_metadata.vector_lane_count = source.vector_lane_count;
      property_metadata.id_spelling = source.id_spelling;
      property_metadata.class_spelling = source.class_spelling;
      property_metadata.instancetype_spelling = source.instancetype_spelling;
      property_metadata.object_pointer_type_spelling = source.object_pointer_type_spelling;
      property_metadata.has_generic_suffix = source.has_generic_suffix;
      property_metadata.has_pointer_declarator = source.has_pointer_declarator;
      property_metadata.has_nullability_suffix = source.has_nullability_suffix;
      property_metadata.has_ownership_qualifier = source.has_ownership_qualifier;
      property_metadata.has_invalid_generic_suffix = source.has_invalid_generic_suffix;
      property_metadata.has_invalid_pointer_declarator = source.has_invalid_pointer_declarator;
      property_metadata.has_invalid_nullability_suffix = source.has_invalid_nullability_suffix;
      property_metadata.has_invalid_ownership_qualifier = source.has_invalid_ownership_qualifier;
      property_metadata.has_invalid_type_suffix = source.has_invalid_type_suffix;
      property_metadata.ownership_insert_retain = source.ownership_insert_retain;
      property_metadata.ownership_insert_release = source.ownership_insert_release;
      property_metadata.ownership_insert_autorelease = source.ownership_insert_autorelease;
      property_metadata.ownership_is_weak_reference = source.ownership_is_weak_reference;
      property_metadata.ownership_is_unowned_reference = source.ownership_is_unowned_reference;
      property_metadata.ownership_is_unowned_safe_reference = source.ownership_is_unowned_safe_reference;
      property_metadata.ownership_arc_diagnostic_candidate = source.ownership_arc_diagnostic_candidate;
      property_metadata.ownership_arc_fixit_available = source.ownership_arc_fixit_available;
      property_metadata.ownership_arc_diagnostic_profile = source.ownership_arc_diagnostic_profile;
      property_metadata.ownership_arc_fixit_hint = source.ownership_arc_fixit_hint;
      property_metadata.ownership_lifetime_profile = source.ownership_lifetime_profile;
      property_metadata.ownership_runtime_hook_profile = source.ownership_runtime_hook_profile;
      property_metadata.attribute_entries = source.attribute_entries;
      property_metadata.attribute_names_lexicographic = source.attribute_names_lexicographic;
      property_metadata.is_readonly = source.is_readonly;
      property_metadata.is_readwrite = source.is_readwrite;
      property_metadata.is_atomic = source.is_atomic;
      property_metadata.is_nonatomic = source.is_nonatomic;
      property_metadata.is_copy = source.is_copy;
      property_metadata.is_retain = source.is_retain;
      property_metadata.is_strong = source.is_strong;
      property_metadata.is_weak = source.is_weak;
      property_metadata.is_unowned = source.is_unowned;
      property_metadata.is_unsafe_unretained = source.is_unsafe_unretained;
      property_metadata.is_assign = source.is_assign;
      property_metadata.is_nullable = source.is_nullable;
      property_metadata.is_nonnull = source.is_nonnull;
      property_metadata.is_null_resettable = source.is_null_resettable;
      property_metadata.is_class = source.is_class;
      property_metadata.is_direct = source.is_direct;
      property_metadata.has_getter = source.has_getter;
      property_metadata.has_setter = source.has_setter;
      property_metadata.getter_selector = source.getter_selector;
      property_metadata.setter_selector = source.setter_selector;
      property_metadata.ivar_binding_symbol = source.ivar_binding_symbol;
      property_metadata.executable_synthesized_binding_kind =
          source.executable_synthesized_binding_kind;
      property_metadata.executable_synthesized_binding_symbol =
          source.executable_synthesized_binding_symbol;
      // runtime-backed ownership attribute surface anchor: the
      // implementation-side property handoff preserves the same ownership
      // profiles so emitted runtime-backed descriptors can stay synchronized
      // across interface/implementation pairs without reinterpreting source.
      property_metadata.property_attribute_profile =
          source.property_attribute_profile;
      property_metadata.effective_getter_selector =
          source.effective_getter_selector;
      property_metadata.effective_setter_available =
          source.effective_setter_available;
      property_metadata.effective_setter_selector =
          source.effective_setter_selector;
      property_metadata.accessor_ownership_profile =
          source.accessor_ownership_profile;
      apply_arc_property_interaction_metadata(property_metadata);
      property_metadata.executable_ivar_layout_symbol =
          source.executable_ivar_layout_symbol;
      property_metadata.executable_ivar_layout_slot_index =
          source.executable_ivar_layout_slot_index;
      property_metadata.executable_ivar_layout_size_bytes =
          source.executable_ivar_layout_size_bytes;
      property_metadata.executable_ivar_layout_alignment_bytes =
          source.executable_ivar_layout_alignment_bytes;
      property_metadata.executable_ivar_layout_offset_bytes =
          source.executable_ivar_layout_offset_bytes;
      property_metadata.executable_ivar_layout_padding_bytes =
          source.executable_ivar_layout_padding_bytes;
      property_metadata.executable_ivar_layout_inherited_slot_count =
          source.executable_ivar_layout_inherited_slot_count;
      property_metadata.executable_ivar_layout_inherited_size_bytes =
          source.executable_ivar_layout_inherited_size_bytes;
      property_metadata.executable_ivar_layout_owner_size_bytes =
          source.executable_ivar_layout_owner_size_bytes;
      property_metadata.executable_ivar_init_order_index =
          source.executable_ivar_init_order_index;
      property_metadata.executable_ivar_destroy_order_index =
          source.executable_ivar_destroy_order_index;
      property_metadata.executable_ivar_layout_valid =
          source.executable_ivar_layout_valid;
      property_metadata.executable_ivar_layout_replay_key =
          source.executable_ivar_layout_replay_key;
      property_metadata.invalid_attribute_entries = source.invalid_attribute_entries;
      property_metadata.property_contract_violations = source.property_contract_violations;
      property_metadata.has_unknown_attribute = source.has_unknown_attribute;
      property_metadata.has_duplicate_attribute = source.has_duplicate_attribute;
      property_metadata.has_readwrite_conflict = source.has_readwrite_conflict;
      property_metadata.has_atomicity_conflict = source.has_atomicity_conflict;
      property_metadata.has_ownership_conflict = source.has_ownership_conflict;
      property_metadata.has_weak_unowned_conflict = source.has_weak_unowned_conflict;
      property_metadata.has_accessor_selector_contract_violation = source.has_accessor_selector_contract_violation;
      property_metadata.has_invalid_attribute_contract = source.has_invalid_attribute_contract;
      metadata.properties_lexicographic.push_back(std::move(property_metadata));
    }

    std::vector<std::string> selectors;
    selectors.reserve(implementation_it->second.methods.size());
    for (const auto &method_entry : implementation_it->second.methods) {
      selectors.push_back(method_entry.first);
    }
    std::sort(selectors.begin(), selectors.end());

    metadata.methods_lexicographic.reserve(selectors.size());
    for (const std::string &selector : selectors) {
      const auto method_it = implementation_it->second.methods.find(selector);
      if (method_it == implementation_it->second.methods.end()) {
        continue;
      }
      const Objc3MethodInfo &source = method_it->second;
      Objc3SemanticMethodTypeMetadata method_metadata;
      method_metadata.selector = selector;
      method_metadata.selector_normalized = source.selector_normalized;
      method_metadata.selector_piece_count = source.selector_piece_count;
      method_metadata.selector_parameter_piece_count = source.selector_parameter_piece_count;
      method_metadata.selector_contract_normalized = source.selector_contract_normalized;
      method_metadata.selector_had_pieceless_form = source.selector_had_pieceless_form;
      method_metadata.selector_has_spelling_mismatch = source.selector_has_spelling_mismatch;
      method_metadata.selector_has_arity_mismatch = source.selector_has_arity_mismatch;
      method_metadata.selector_has_parameter_linkage_mismatch = source.selector_has_parameter_linkage_mismatch;
      method_metadata.selector_has_normalization_flag_mismatch = source.selector_has_normalization_flag_mismatch;
      method_metadata.selector_has_missing_piece_keyword = source.selector_has_missing_piece_keyword;
      method_metadata.arity = source.arity;
      method_metadata.param_types = source.param_types;
      method_metadata.param_canonical_types = source.param_canonical_types;
      method_metadata.param_is_vector = source.param_is_vector;
      method_metadata.param_vector_base_spelling = source.param_vector_base_spelling;
      method_metadata.param_vector_lane_count = source.param_vector_lane_count;
      method_metadata.param_has_generic_suffix = source.param_has_generic_suffix;
      method_metadata.param_has_pointer_declarator = source.param_has_pointer_declarator;
      method_metadata.param_has_nullability_suffix = source.param_has_nullability_suffix;
      method_metadata.param_has_ownership_qualifier = source.param_has_ownership_qualifier;
      method_metadata.param_id_spelling = source.param_id_spelling;
      method_metadata.param_class_spelling = source.param_class_spelling;
      method_metadata.param_instancetype_spelling = source.param_instancetype_spelling;
      method_metadata.param_object_pointer_type_spelling = source.param_object_pointer_type_spelling;
      method_metadata.param_has_invalid_generic_suffix = source.param_has_invalid_generic_suffix;
      method_metadata.param_has_invalid_pointer_declarator = source.param_has_invalid_pointer_declarator;
      method_metadata.param_has_invalid_nullability_suffix = source.param_has_invalid_nullability_suffix;
      method_metadata.param_has_invalid_ownership_qualifier = source.param_has_invalid_ownership_qualifier;
      method_metadata.param_has_invalid_type_suffix = source.param_has_invalid_type_suffix;
      method_metadata.param_ownership_insert_retain = source.param_ownership_insert_retain;
      method_metadata.param_ownership_insert_release = source.param_ownership_insert_release;
      method_metadata.param_ownership_insert_autorelease = source.param_ownership_insert_autorelease;
      method_metadata.param_ownership_is_weak_reference = source.param_ownership_is_weak_reference;
      method_metadata.param_ownership_is_unowned_reference = source.param_ownership_is_unowned_reference;
      method_metadata.param_ownership_is_unowned_safe_reference = source.param_ownership_is_unowned_safe_reference;
      method_metadata.param_ownership_arc_diagnostic_candidate = source.param_ownership_arc_diagnostic_candidate;
      method_metadata.param_ownership_arc_fixit_available = source.param_ownership_arc_fixit_available;
      method_metadata.param_ownership_arc_diagnostic_profile = source.param_ownership_arc_diagnostic_profile;
      method_metadata.param_ownership_arc_fixit_hint = source.param_ownership_arc_fixit_hint;
      method_metadata.param_has_protocol_composition = source.param_has_protocol_composition;
      method_metadata.param_protocol_composition_lexicographic = source.param_protocol_composition_lexicographic;
      method_metadata.param_has_invalid_protocol_composition = source.param_has_invalid_protocol_composition;
      method_metadata.return_has_generic_suffix = source.return_has_generic_suffix;
      method_metadata.return_has_pointer_declarator = source.return_has_pointer_declarator;
      method_metadata.return_has_nullability_suffix = source.return_has_nullability_suffix;
      method_metadata.return_has_ownership_qualifier = source.return_has_ownership_qualifier;
      method_metadata.return_id_spelling = source.return_id_spelling;
      method_metadata.return_class_spelling = source.return_class_spelling;
      method_metadata.return_instancetype_spelling = source.return_instancetype_spelling;
      method_metadata.return_object_pointer_type_spelling = source.return_object_pointer_type_spelling;
      method_metadata.return_has_invalid_generic_suffix = source.return_has_invalid_generic_suffix;
      method_metadata.return_has_invalid_pointer_declarator = source.return_has_invalid_pointer_declarator;
      method_metadata.return_has_invalid_nullability_suffix = source.return_has_invalid_nullability_suffix;
      method_metadata.return_has_invalid_ownership_qualifier = source.return_has_invalid_ownership_qualifier;
      method_metadata.return_has_invalid_type_suffix = source.return_has_invalid_type_suffix;
      method_metadata.return_ownership_insert_retain = source.return_ownership_insert_retain;
      method_metadata.return_ownership_insert_release = source.return_ownership_insert_release;
      method_metadata.return_ownership_insert_autorelease = source.return_ownership_insert_autorelease;
      method_metadata.return_ownership_is_weak_reference = source.return_ownership_is_weak_reference;
      method_metadata.return_ownership_is_unowned_reference = source.return_ownership_is_unowned_reference;
      method_metadata.return_ownership_is_unowned_safe_reference = source.return_ownership_is_unowned_safe_reference;
      method_metadata.return_ownership_arc_diagnostic_candidate = source.return_ownership_arc_diagnostic_candidate;
      method_metadata.return_ownership_arc_fixit_available = source.return_ownership_arc_fixit_available;
      method_metadata.return_ownership_arc_diagnostic_profile = source.return_ownership_arc_diagnostic_profile;
      method_metadata.return_ownership_arc_fixit_hint = source.return_ownership_arc_fixit_hint;
      method_metadata.return_type = source.return_type;
      method_metadata.return_canonical_type = source.return_canonical_type;
      method_metadata.return_is_vector = source.return_is_vector;
      method_metadata.return_vector_base_spelling = source.return_vector_base_spelling;
      method_metadata.return_vector_lane_count = source.return_vector_lane_count;
      method_metadata.return_has_protocol_composition = source.return_has_protocol_composition;
      method_metadata.return_protocol_composition_lexicographic = source.return_protocol_composition_lexicographic;
      method_metadata.return_has_invalid_protocol_composition = source.return_has_invalid_protocol_composition;
      method_metadata.async_continuation_profile_is_normalized =
          source.async_continuation_profile_is_normalized;
      method_metadata.deterministic_async_continuation_handoff =
          source.deterministic_async_continuation_handoff;
      method_metadata.async_continuation_sites =
          source.async_continuation_sites;
      method_metadata.async_keyword_sites = source.async_keyword_sites;
      method_metadata.async_function_sites = source.async_function_sites;
      method_metadata.continuation_allocation_sites =
          source.continuation_allocation_sites;
      method_metadata.continuation_resume_sites =
          source.continuation_resume_sites;
      method_metadata.continuation_suspend_sites =
          source.continuation_suspend_sites;
      method_metadata.async_state_machine_sites =
          source.async_state_machine_sites;
      method_metadata.async_continuation_normalized_sites =
          source.async_continuation_normalized_sites;
      method_metadata.async_continuation_gate_blocked_sites =
          source.async_continuation_gate_blocked_sites;
      method_metadata.async_continuation_contract_violation_sites =
          source.async_continuation_contract_violation_sites;
      method_metadata.actor_isolation_sendability_profile_is_normalized =
          source.actor_isolation_sendability_profile_is_normalized;
      method_metadata.deterministic_actor_isolation_sendability_handoff =
          source.deterministic_actor_isolation_sendability_handoff;
      method_metadata.actor_isolation_sendability_sites =
          source.actor_isolation_sendability_sites;
      method_metadata.actor_isolation_decl_sites =
          source.actor_isolation_decl_sites;
      method_metadata.actor_hop_sites = source.actor_hop_sites;
      method_metadata.sendable_annotation_sites =
          source.sendable_annotation_sites;
      method_metadata.non_sendable_crossing_sites =
          source.non_sendable_crossing_sites;
      method_metadata.isolation_boundary_sites = source.isolation_boundary_sites;
      method_metadata.actor_isolation_sendability_normalized_sites =
          source.actor_isolation_sendability_normalized_sites;
      method_metadata.actor_isolation_sendability_gate_blocked_sites =
          source.actor_isolation_sendability_gate_blocked_sites;
      method_metadata.actor_isolation_sendability_contract_violation_sites =
          source.actor_isolation_sendability_contract_violation_sites;
      method_metadata.task_runtime_cancellation_profile_is_normalized =
          source.task_runtime_cancellation_profile_is_normalized;
      method_metadata.deterministic_task_runtime_cancellation_handoff =
          source.deterministic_task_runtime_cancellation_handoff;
      method_metadata.task_runtime_interop_sites =
          source.task_runtime_interop_sites;
      method_metadata.runtime_hook_sites = source.runtime_hook_sites;
      method_metadata.cancellation_check_sites =
          source.cancellation_check_sites;
      method_metadata.cancellation_handler_sites =
          source.cancellation_handler_sites;
      method_metadata.suspension_point_sites = source.suspension_point_sites;
      method_metadata.cancellation_propagation_sites =
          source.cancellation_propagation_sites;
      method_metadata.task_runtime_cancellation_normalized_sites =
          source.task_runtime_cancellation_normalized_sites;
      method_metadata.task_runtime_cancellation_gate_blocked_sites =
          source.task_runtime_cancellation_gate_blocked_sites;
      method_metadata.task_runtime_cancellation_contract_violation_sites =
          source.task_runtime_cancellation_contract_violation_sites;
      method_metadata.concurrency_replay_race_guard_profile_is_normalized =
          source.concurrency_replay_race_guard_profile_is_normalized;
      method_metadata.deterministic_concurrency_replay_race_guard_handoff =
          source.deterministic_concurrency_replay_race_guard_handoff;
      method_metadata.concurrency_replay_race_guard_sites =
          source.concurrency_replay_race_guard_sites;
      method_metadata.concurrency_replay_sites = source.concurrency_replay_sites;
      method_metadata.replay_proof_sites = source.replay_proof_sites;
      method_metadata.race_guard_sites = source.race_guard_sites;
      method_metadata.task_handoff_sites = source.task_handoff_sites;
      method_metadata.actor_isolation_sites = source.actor_isolation_sites;
      method_metadata.deterministic_schedule_sites =
          source.deterministic_schedule_sites;
      method_metadata.concurrency_replay_guard_blocked_sites =
          source.concurrency_replay_guard_blocked_sites;
      method_metadata.concurrency_replay_contract_violation_sites =
          source.concurrency_replay_contract_violation_sites;
      method_metadata.ns_error_bridging_profile_is_normalized =
          source.ns_error_bridging_profile_is_normalized;
      method_metadata.deterministic_ns_error_bridging_lowering_handoff =
          source.deterministic_ns_error_bridging_lowering_handoff;
      method_metadata.ns_error_bridging_sites = source.ns_error_bridging_sites;
      method_metadata.ns_error_parameter_sites = source.ns_error_parameter_sites;
      method_metadata.ns_error_out_parameter_sites =
          source.ns_error_out_parameter_sites;
      method_metadata.ns_error_bridge_path_sites =
          source.ns_error_bridge_path_sites;
      method_metadata.failable_call_sites = source.failable_call_sites;
      method_metadata.ns_error_bridging_normalized_sites =
          source.ns_error_bridging_normalized_sites;
      method_metadata.ns_error_bridge_boundary_sites =
          source.ns_error_bridge_boundary_sites;
      method_metadata.ns_error_bridging_contract_violation_sites =
          source.ns_error_bridging_contract_violation_sites;
      method_metadata.is_class_method = source.is_class_method;
      method_metadata.has_definition = source.has_definition;
      metadata.methods_lexicographic.push_back(std::move(method_metadata));
    }

    handoff.implementations_lexicographic.push_back(std::move(metadata));
  }

#include "sema/objc3_semantic_passes_type_metadata_interface_implementation_summary.inc"

  handoff.selector_normalization_summary = Objc3SelectorNormalizationSummary{};
  const auto accumulate_method_selector_metadata =
      [&handoff](const Objc3SemanticMethodTypeMetadata &metadata) {
        ++handoff.selector_normalization_summary.methods_total;
        if (metadata.selector_contract_normalized) {
          ++handoff.selector_normalization_summary.normalized_methods;
        }
        handoff.selector_normalization_summary.selector_piece_entries += metadata.selector_piece_count;
        handoff.selector_normalization_summary.selector_parameter_piece_entries +=
            metadata.selector_parameter_piece_count;
        if (metadata.selector_had_pieceless_form) {
          ++handoff.selector_normalization_summary.selector_pieceless_methods;
        }
        if (metadata.selector_has_spelling_mismatch) {
          ++handoff.selector_normalization_summary.selector_spelling_mismatches;
        }
        if (metadata.selector_has_arity_mismatch) {
          ++handoff.selector_normalization_summary.selector_arity_mismatches;
        }
        if (metadata.selector_has_parameter_linkage_mismatch) {
          ++handoff.selector_normalization_summary.selector_parameter_linkage_mismatches;
        }
        if (metadata.selector_has_normalization_flag_mismatch) {
          ++handoff.selector_normalization_summary.selector_normalization_flag_mismatches;
        }
        if (metadata.selector_has_missing_piece_keyword) {
          ++handoff.selector_normalization_summary.selector_missing_keyword_pieces;
        }
        if (metadata.selector_normalized.empty() ||
            metadata.selector_parameter_piece_count > metadata.selector_piece_count) {
          handoff.selector_normalization_summary.deterministic = false;
        }
      };
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      accumulate_method_selector_metadata(method_metadata);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
