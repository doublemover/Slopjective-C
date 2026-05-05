                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_conformance_corpus_key.empty()) {
      out << "; ir_emission_core_feature_conformance_corpus = "
          << frontend_metadata_
                 .ir_emission_core_feature_conformance_corpus_key
          << "\n";
    }
    out << "; ir_emission_core_feature_conformance_corpus_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_conformance_corpus_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_performance_quality_guardrails_key.empty()) {
      out << "; ir_emission_core_feature_performance_quality_guardrails = "
          << frontend_metadata_
                 .ir_emission_core_feature_performance_quality_guardrails_key
          << "\n";
    }
    out << "; ir_emission_core_feature_performance_quality_guardrails_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_performance_quality_guardrails_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_cross_lane_integration_sync_key.empty()) {
      out << "; ir_emission_core_feature_cross_lane_integration_sync = "
          << frontend_metadata_
                 .ir_emission_core_feature_cross_lane_integration_sync_key
          << "\n";
    }
    out << "; ir_emission_core_feature_cross_lane_integration_sync_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_cross_lane_integration_sync_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_core_shard1 = "
          << frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_core_shard1_ready = "
        << (frontend_metadata_.ir_emission_core_feature_advanced_core_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_edge_compatibility_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_edge_compatibility_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_edge_compatibility_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_edge_compatibility_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_edge_compatibility_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_diagnostics_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_diagnostics_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_diagnostics_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_diagnostics_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_diagnostics_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_conformance_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_conformance_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_conformance_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_conformance_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_conformance_shard1_ready
                ? "true"
                : "false")
        << "\n";
    if (!frontend_metadata_
             .ir_emission_core_feature_advanced_integration_shard1_key.empty()) {
      out << "; ir_emission_core_feature_advanced_integration_shard1 = "
          << frontend_metadata_
                 .ir_emission_core_feature_advanced_integration_shard1_key
          << "\n";
    }
    out << "; ir_emission_core_feature_advanced_integration_shard1_ready = "
        << (frontend_metadata_
                    .ir_emission_core_feature_advanced_integration_shard1_ready
                ? "true"
                : "false")
        << "\n";
    out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";
    out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)
        << ", language_profile=" << frontend_metadata_.language_profile
        << ", arc_mode=" << frontend_metadata_.arc_mode
        << ", legacy_literal_diagnostics=" << (frontend_metadata_.legacy_literal_diagnostics ? "true" : "false")
        << ", migration_legacy_total=" << frontend_metadata_.migration_legacy_total() << "\n";
    out << "; frontend_objc_interface_implementation_profile = declared_interfaces="
        << frontend_metadata_.declared_interfaces
        << ", declared_implementations=" << frontend_metadata_.declared_implementations
        << ", resolved_interface_symbols=" << frontend_metadata_.resolved_interface_symbols
        << ", resolved_implementation_symbols=" << frontend_metadata_.resolved_implementation_symbols
        << ", interface_method_symbols=" << frontend_metadata_.interface_method_symbols
        << ", implementation_method_symbols=" << frontend_metadata_.implementation_method_symbols
        << ", linked_implementation_symbols=" << frontend_metadata_.linked_implementation_symbols
        << ", deterministic_interface_implementation_handoff="
        << (frontend_metadata_.deterministic_interface_implementation_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_protocol_category_profile = declared_protocols="
        << frontend_metadata_.declared_protocols
        << ", declared_categories=" << frontend_metadata_.declared_categories
        << ", resolved_protocol_symbols=" << frontend_metadata_.resolved_protocol_symbols
        << ", resolved_category_symbols=" << frontend_metadata_.resolved_category_symbols
        << ", protocol_method_symbols=" << frontend_metadata_.protocol_method_symbols
        << ", category_method_symbols=" << frontend_metadata_.category_method_symbols
        << ", linked_category_symbols=" << frontend_metadata_.linked_category_symbols
        << ", deterministic_protocol_category_handoff="
        << (frontend_metadata_.deterministic_protocol_category_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces="
        << frontend_metadata_.declared_class_interfaces
        << ", declared_class_implementations=" << frontend_metadata_.declared_class_implementations
        << ", resolved_class_interfaces=" << frontend_metadata_.resolved_class_interfaces
        << ", resolved_class_implementations=" << frontend_metadata_.resolved_class_implementations
        << ", linked_class_method_symbols=" << frontend_metadata_.linked_class_method_symbols
        << ", linked_category_method_symbols=" << frontend_metadata_.linked_category_method_symbols
        << ", protocol_composition_sites=" << frontend_metadata_.protocol_composition_sites
        << ", protocol_composition_symbols=" << frontend_metadata_.protocol_composition_symbols
        << ", category_composition_sites=" << frontend_metadata_.category_composition_sites
        << ", category_composition_symbols=" << frontend_metadata_.category_composition_symbols
        << ", invalid_protocol_composition_sites=" << frontend_metadata_.invalid_protocol_composition_sites
        << ", deterministic_class_protocol_category_linking_handoff="
        << (frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_selector_normalization_profile = method_declaration_entries="
        << frontend_metadata_.selector_method_declaration_entries
        << ", normalized_method_declarations=" << frontend_metadata_.selector_normalized_method_declarations
        << ", selector_piece_entries=" << frontend_metadata_.selector_piece_entries
        << ", selector_piece_parameter_links=" << frontend_metadata_.selector_piece_parameter_links
        << ", deterministic_selector_normalization_handoff="
        << (frontend_metadata_.deterministic_selector_normalization_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_property_attribute_profile = property_declaration_entries="
        << frontend_metadata_.property_declaration_entries
        << ", property_attribute_entries=" << frontend_metadata_.property_attribute_entries
        << ", property_attribute_value_entries=" << frontend_metadata_.property_attribute_value_entries
        << ", property_accessor_modifier_entries=" << frontend_metadata_.property_accessor_modifier_entries
        << ", property_getter_selector_entries=" << frontend_metadata_.property_getter_selector_entries
        << ", property_setter_selector_entries=" << frontend_metadata_.property_setter_selector_entries
        << ", deterministic_property_attribute_handoff="
        << (frontend_metadata_.deterministic_property_attribute_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_id_class_sel_object_pointer_typecheck_profile = id_typecheck_sites="
        << frontend_metadata_.id_typecheck_sites
        << ", class_typecheck_sites=" << frontend_metadata_.class_typecheck_sites
        << ", sel_typecheck_sites=" << frontend_metadata_.sel_typecheck_sites
        << ", object_pointer_typecheck_sites=" << frontend_metadata_.object_pointer_typecheck_sites
        << ", total_typecheck_sites=" << frontend_metadata_.id_class_sel_object_pointer_typecheck_sites_total
        << ", deterministic_id_class_sel_object_pointer_typecheck_handoff="
        << (frontend_metadata_.deterministic_id_class_sel_object_pointer_typecheck_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_dispatch_surface_classification_profile = instance_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_instance_sites
        << ", class_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_class_sites
        << ", super_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_super_sites
        << ", direct_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_direct_sites
        << ", dynamic_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_dynamic_sites
        << ", instance_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_instance_entrypoint_family
        << ", class_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_class_entrypoint_family
        << ", super_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_super_entrypoint_family
        << ", direct_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_direct_entrypoint_family
        << ", dynamic_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_dynamic_entrypoint_family
        << ", deterministic_dispatch_surface_classification_handoff="
        << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_message_send_selector_lowering_profile = message_send_sites="
        << frontend_metadata_.message_send_selector_lowering_sites
        << ", unary_selector_sites=" << frontend_metadata_.message_send_selector_lowering_unary_sites
        << ", keyword_selector_sites=" << frontend_metadata_.message_send_selector_lowering_keyword_sites
        << ", selector_piece_sites=" << frontend_metadata_.message_send_selector_lowering_selector_piece_sites
        << ", argument_expression_sites="
        << frontend_metadata_.message_send_selector_lowering_argument_expression_sites
        << ", receiver_expression_sites=" << frontend_metadata_.message_send_selector_lowering_receiver_sites
        << ", selector_literal_entries="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_entries
        << ", selector_literal_characters="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_characters
        << ", deterministic_message_send_selector_lowering_handoff="
        << (frontend_metadata_.deterministic_message_send_selector_lowering_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_dispatch_abi_marshalling_profile = message_send_sites="
        << frontend_metadata_.dispatch_abi_marshalling_message_send_sites
        << ", receiver_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_receiver_slots_marshaled
        << ", selector_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_selector_slots_marshaled
        << ", argument_value_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_value_slots_marshaled
        << ", argument_padding_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_padding_slots_marshaled
        << ", argument_total_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_total_slots_marshaled
        << ", total_marshaled_slots=" << frontend_metadata_.dispatch_abi_marshalling_total_marshaled_slots
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.dispatch_abi_marshalling_runtime_dispatch_arg_slots
        << ", deterministic_dispatch_abi_marshalling_handoff="
        << (frontend_metadata_.deterministic_dispatch_abi_marshalling_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_nil_receiver_semantics_foldability_profile = message_send_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_message_send_sites
        << ", receiver_nil_literal_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_receiver_nil_literal_sites
        << ", nil_receiver_semantics_enabled_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_enabled_sites
        << ", nil_receiver_foldable_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_foldable_sites
        << ", nil_receiver_runtime_dispatch_required_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_runtime_dispatch_required_sites
        << ", non_nil_receiver_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_non_nil_receiver_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_contract_violation_sites
        << ", deterministic_nil_receiver_semantics_foldability_handoff="
        << (frontend_metadata_.deterministic_nil_receiver_semantics_foldability_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_super_dispatch_method_family_profile = message_send_sites="
        << frontend_metadata_.super_dispatch_method_family_message_send_sites
        << ", receiver_super_identifier_sites="
        << frontend_metadata_.super_dispatch_method_family_receiver_super_identifier_sites
        << ", super_dispatch_enabled_sites=" << frontend_metadata_.super_dispatch_method_family_enabled_sites
        << ", super_dispatch_requires_class_context_sites="
        << frontend_metadata_.super_dispatch_method_family_requires_class_context_sites
        << ", method_family_init_sites=" << frontend_metadata_.super_dispatch_method_family_init_sites
        << ", method_family_copy_sites=" << frontend_metadata_.super_dispatch_method_family_copy_sites
        << ", method_family_mutable_copy_sites="
        << frontend_metadata_.super_dispatch_method_family_mutable_copy_sites
        << ", method_family_new_sites=" << frontend_metadata_.super_dispatch_method_family_new_sites
        << ", method_family_none_sites=" << frontend_metadata_.super_dispatch_method_family_none_sites
        << ", method_family_returns_retained_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_retained_result_sites
        << ", method_family_returns_related_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_related_result_sites
        << ", contract_violation_sites="
        << frontend_metadata_.super_dispatch_method_family_contract_violation_sites
        << ", deterministic_super_dispatch_method_family_handoff="
        << (frontend_metadata_.deterministic_super_dispatch_method_family_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_runtime_link_host_link_profile = message_send_sites="
        << frontend_metadata_.runtime_link_host_link_message_send_sites
        << ", runtime_link_required_sites=" << frontend_metadata_.runtime_link_host_link_required_sites
        << ", runtime_link_elided_sites=" << frontend_metadata_.runtime_link_host_link_elided_sites
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.runtime_link_host_link_runtime_dispatch_arg_slots
        << ", runtime_dispatch_declaration_parameter_count="
        << frontend_metadata_.runtime_link_host_link_runtime_dispatch_declaration_parameter_count
        << ", runtime_dispatch_symbol=" << frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol
        << ", default_runtime_dispatch_symbol_binding="
        << (frontend_metadata_.runtime_link_host_link_default_runtime_dispatch_symbol_binding ? "true" : "false")
        << ", contract_violation_sites="
        << frontend_metadata_.runtime_link_host_link_contract_violation_sites
        << ", deterministic_runtime_link_host_link_handoff="
        << (frontend_metadata_.deterministic_runtime_link_host_link_handoff ? "true" : "false")
        << "\n";
    // runtime-backed-object-ownership freeze anchor: the current
    // runnable object slice preserves ownership through property/accessor
    // metadata profiles plus these legacy ownership lowering summaries. No
    // live ARC runtime retain/release/autorelease execution hooks are emitted
    // here yet.
    // retainable-object semantic-rule freeze anchor: retain/release,
    // autoreleasepool, and destruction-order behavior are still represented by
    // deterministic summary lanes only; runtime-backed property/member
    // ownership metadata and storage legality are now live sema-enforced
    // surfaces before metadata emission.
    out << "; retainable_object_semantic_rules_freeze = "
        << Objc3RetainableObjectSemanticRulesFreezeSummary() << "\n";
    // runtime-backed storage ownership legality anchor: explicit
    // object-property ownership qualifiers now participate in live semantic
    // legality, so the IR closeout surface publishes the exact owned/weak/
    // unowned contract now enforced before metadata emission.
    out << "; runtime_backed_storage_ownership_legality = "
        << Objc3RuntimeBackedStorageOwnershipLegalitySummary() << "\n";
    // autoreleasepool/destruction-order semantic expansion anchor:
    // lane-B still fail-closes autoreleasepool, but the IR closeout surface
    // now publishes the ownership-sensitive destruction-order contract that
    // distinguishes plain pool rejection from owned runtime-backed object
    // storage edges.
    out << "; runtime_backed_autoreleasepool_destruction_order = "
        << Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() << "\n";
    // ownership-lowering baseline freeze anchor: the current IR
    // surface keeps ownership qualifier, retain/release, autoreleasepool, and
    // weak/unowned lowering on deterministic summary lanes only.
    // No live runtime ownership hooks are emitted here before the next runtime step.
    out << "; ownership_lowering_baseline = "
        << Objc3OwnershipLoweringBaselineSummary() << "\n";
    if (synthesized_property_accessor_count_ > 0u) {
      // runtime hook emission anchor: synthesized accessors now
      // execute through runtime-owned helper entrypoints that consume the
      // current dispatch-frame property context rather than the old summary-only
      // ownership lane, and owned and weak execution paths use the live runtime
      // hooks below.
      out << "; ownership_runtime_hook_emission = "
          << Objc3OwnershipRuntimeHookEmissionSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_ << "\n";
      // runtime memory-management API freeze anchor: the public
      // runtime ABI remains narrow while lane-C emits the private helper
      // surface consumed by synthesized ownership accessors.
      out << "; runtime_memory_management_api = "
          << Objc3RuntimeMemoryManagementApiSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_ << "\n";
    }
    if (synthesized_property_accessor_count_ > 0u ||
        frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
        frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
            0u ||
        frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
        frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
      // runtime memory-management implementation anchor: emitted IR
      // now carries the private autoreleasepool push/pop runtime surface and
      // the live refcount/weak/autoreleasepool execution model that native mode
      // consumes for runtime-backed object programs.
      out << "; runtime_memory_management_implementation = "
          << Objc3RuntimeMemoryManagementImplementationSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_
          << ";autoreleasepool_scope_sites="
          << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
          << "\n";
    }
    // ownership-runtime-gate freeze anchor: lane-E freezes the
    // supported ownership runtime slice and its explicit non-goals as a
    // dedicated emitted boundary so later smoke/docs work can validate the
    // correct baseline without rediscovering it from prose alone.
    // ownership-smoke closeout anchor: the runnable smoke matrix
    // consumes this exact emitted gate boundary as the integrated ownership
    // proof surface for closeout.
    out << "; ownership_runtime_gate = "
        << Objc3OwnershipRuntimeGateSummary() << "\n";
    // executable-block-source-closure anchor: emit the truthful
    // parser/AST boundary so lane-A can prove block literals entered the source
    // closure before runtime lowering remains fail closed.
    out << "; executable_block_source_closure = "
        << Objc3ExecutableBlockSourceClosureSummary() << "\n";
    // block-source-model-completion anchor: emit the completed
    // source-model replay boundary so source-only frontend runs and later
    // lowering work can agree on typed parameter signatures, capture storage
    // inventories, and invoke-surface symbols without reconstructing them.
    out << "; executable_block_source_model_completion = "
        << Objc3ExecutableBlockSourceModelCompletionSummary()
        << ";replay_key="
        << frontend_metadata_.lowering_block_source_model_completion_replay_key
        << "\n";
    // block-source-storage-annotation anchor: emit the truthful
    // byref/helper/escape-shape replay boundary so source-only manifests and
    // later runnable block lowering consume the same deterministic source
    // annotations while native block execution remains fail closed.
    out << "; executable_block_source_storage_annotations = "
        << Objc3ExecutableBlockSourceStorageAnnotationSummary()
        << ";replay_key="
        << frontend_metadata_
               .lowering_block_source_storage_annotation_replay_key
        << "\n";
    // block-runtime-semantic-rules freeze anchor: emit the current
    // semantic-rule boundary so block runtime follow-on work can preserve the
    // source-only admission/native fail-closed split without rediscovering it
    // from prose or historical issue packets.
    // capture-legality/escape/invocation implementation anchor:
    // the emitted summary line stays on the frozen runtime boundary while the
    // source-only sema path now enforces live capture legality and local
    // block-call typing ahead of runnable block-object lowering.
    // byref/copy-dispose/object-ownership anchor: helper-eligibility
    // totals in this emitted surface now reflect owned object captures as well as byref cells,
    // while native block execution still remains fail-closed.
    out << "; executable_block_runtime_semantic_rules = "
        << Objc3ExecutableBlockRuntimeSemanticRulesSummary() << "\n";
    // block-lowering-ABI/artifact-boundary freeze anchor: lane-C
    // now publishes the truthful lowering boundary required for runnable block
    // execution, while native emit still fails closed before any emitted block
    // object records, invoke thunks, byref cells, or helper bodies land.
    out << "; executable_block_lowering_abi_artifact_boundary = "
        << Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() << "\n";
    // executable-block-object/invoke-thunk anchor: native lowering
    // now emits stack block objects plus internal invoke thunks for the narrow
    // readonly-scalar capture slice, while byref/helper/ownership-sensitive
    // cases remain explicitly deferred to C003.
    out << "; executable_block_object_invoke_thunk_lowering = "
        << Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() << "\n";
    // byref-cell/copy-helper/dispose-helper anchor: native lowering
    // now widens the runnable block slice to non-escaping byref and owned
    // capture cases through stack helper emission and helper call sites, while
    // escaping heap promotion remains deferred to later work.
    out << "; executable_block_byref_helper_lowering = "
        << Objc3ExecutableBlockByrefHelperLoweringSummary() << "\n";
    // escaping-block runtime-hook anchor: lane-C now publishes the
    // escaping readonly-scalar block slice that lowers through runtime
    // promotion/invoke hooks while pointer-managed escaping captures remain
    // explicitly deferred to later runtime issues.
    out << "; executable_block_escape_runtime_hook_lowering = "
        << Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() << "\n";
    // block-runtime API/object-layout freeze anchor: emitted IR now
    // republishes the current private helper ABI and private runtime layout
    // boundary so later runtime implementation issues preserve this exact
    // contract instead of widening it ad hoc.
    out << "; runtime_block_api_object_layout = "
        << Objc3RuntimeBlockApiObjectLayoutSummary() << "\n";
    // block-runtime allocation/copy-dispose/invoke anchor: emitted
    // IR now republishes the live runtime capability boundary for promoted
    // block records with helper-mediated copy/dispose support while byref and
    // ownership-interoperating escape paths remain deferred.
    out << "; runtime_block_allocation_copy_dispose_invoke_support = "
        << Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() << "\n";
    // byref-forwarding/heap-promotion/ownership-interop anchor:
    // emitted IR now republishes that escaping pointer-capture block handles
    // rewrite capture slots onto runtime-owned forwarding cells before helper
    // execution, keeping byref mutation and owned capture lifetimes live after
    // the source frame returns.
    out << "; runtime_block_byref_forwarding_heap_promotion_ownership_interop = "
        << Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() << "\n";
    // runnable-block-runtime gate anchor: lane-E now freezes the
    // integrated block-runtime gate above the retained A003/B003/C004/D003
    // source, sema, lowering, and runtime proofs so later closeout work
    // cannot substitute metadata-only evidence.
    out << "; runnable_block_runtime_gate = "
        << Objc3RunnableBlockRuntimeGateSummary() << "\n";
    // runnable-block execution-matrix anchor: lane-E now closes the
    // current current slice by requiring integrated executable block probes above
    // the retained gate, without widening the public block ABI or helper
    // surface.
    out << "; runnable_block_execution_matrix = "
        << Objc3RunnableBlockExecutionMatrixSummary() << "\n";
    // ARC source-surface/mode-boundary anchor: emit the truthful
    // ARC-adjacent frontend/mode boundary so later ARC automation work cannot
    // silently claim a runnable `-fobjc-arc` mode before the driver and
    // executable ownership-qualified function/method path are actually live.
    out << "; arc_source_mode_boundary = "
        << Objc3ArcSourceModeBoundarySummary() << "\n";
    // ARC mode-handling core implementation anchor: publish the
    // explicit ARC-mode execution boundary so manifests and emitted IR stay
    // aligned on when ownership-qualified executable signatures are runnable.
    out << "; arc_mode_handling = "
        << Objc3ArcModeHandlingSummary(frontend_metadata_.arc_mode_enabled) << "\n";
    // ARC semantic-rule freeze anchor: publish the semantic
    // fail-closed boundary for property conflicts and deferred inference so IR
    // evidence stays aligned with semantic validation.
    out << "; arc_semantic_rules = "
        << Objc3ArcSemanticRulesSummary() << "\n";
    // ARC inference/lifetime implementation anchor: publish the
    // truthful semantic-upgrade boundary so emitted IR and manifests agree
    // when ARC mode has widened the supported slice from explicit-only
    // ownership spelling into inferred strong-owned retain/release activity.
    out << "; arc_inference_lifetime = "
        << Objc3ArcInferenceLifetimeSummary() << "\n";
    // ARC interaction-semantics expansion anchor: publish the
    // supported weak/autorelease-return/property-synthesis/block-interaction
    // semantic boundary so emitted IR stays aligned with the live ARC slice
    // instead of forcing later issues to reconstruct it out of older packets.
    out << "; arc_interaction_semantics = "
        << Objc3ArcInteractionSemanticsSummary() << "\n";
    // ARC lowering ABI/cleanup freeze anchor: publish the current
    // lowering/helper boundary directly into IR so later retain/release,
    // cleanup-scheduling, weak lowering, and autorelease-return work must
    // preserve one explicit contract instead of inferring it from older ARC
    // semantic packets.
    out << "; arc_lowering_abi_cleanup_model = "
        << Objc3ArcLoweringAbiCleanupModelSummary() << "\n";
    // ARC automatic-insertion implementation anchor: publish the
    // live param/return helper-insertion boundary so later ARC work extends a
    // real lowering surface instead of a summary-only semantic contract.
    out << "; arc_automatic_insertions = "
        << Objc3ArcAutomaticInsertionSummary() << "\n";
    // ARC cleanup/weak/lifetime implementation anchor: publish the
    // supported scope-exit cleanup, weak current-property helper, and block
    // lifetime cleanup boundary so later ARC/block widening extends a real
    // lowering/runtime surface rather than inferred behavior.
    out << "; arc_cleanup_weak_lifetime_hooks = "
        << Objc3ArcCleanupWeakLifetimeHooksSummary() << "\n";
    // ARC/block autorelease-return implementation anchor: publish
    // the supported escaping-block plus autoreleasing-return edge inventory so
    // later runtime ARC work extends a real branch-stable lowering surface
    // rather than re-deriving cleanup ordering from semantic summaries.
    out << "; arc_block_autorelease_return_lowering = "
        << Objc3ArcBlockAutoreleaseReturnLoweringSummary() << "\n";
    // runtime ARC helper API surface anchor: publish the private
    // helper ABI boundary that current ARC lowering already consumes so later
    // runtime implementation work extends a truthful runtime contract instead
    // of inferring helper availability from emitted calls alone.
    out << "; runtime_arc_helper_api_surface = "
        << Objc3RuntimeArcHelperApiSurfaceSummary() << "\n";
    // runtime ARC helper implementation anchor: publish the live
    // executable helper-runtime support boundary so later diagnostics or debug
    // instrumentation work extends a truthful runtime capability rather than
    // another summary-only marker.
    out << "; runtime_arc_helper_runtime_support = "
        << Objc3RuntimeArcHelperRuntimeSupportSummary() << "\n";
    // ownership-debug/runtime-validation anchor: publish the
    // private ARC debug snapshot boundary so lane-D validation can prove live
    // helper traffic without widening the public runtime ABI.
    out << "; runtime_arc_debug_instrumentation = "
        << Objc3RuntimeArcDebugInstrumentationSummary() << "\n";
    // runnable-arc-runtime gate anchor: lane-E freezes the supported
    // ARC slice above the existing mode-handling, interaction, lowering, and
    // runtime proof chain without claiming closeout-matrix coverage yet.
    out << "; runnable_arc_runtime_gate = "
        << Objc3RunnableArcRuntimeGateSummary() << "\n";
    // runnable-arc-closeout anchor: lane-E consumes the already-live
    // ARC proof chain plus integrated execution smoke as the closeout surface
    // without widening the supported ARC semantics or runtime ABI.
    out << "; runnable_arc_closeout = " << Objc3RunnableArcCloseoutSummary()
        << "\n";
    // runtime-backed semantics closure anchor: issue #8017 consumes the
    // existing block, ARC, error, async/task, and actor runtime-backed slices
    // through one emitted contract line plus a durable claimability report.
    out << "; runtime_backed_semantics_closure = "
        << Objc3RuntimeBackedSemanticsClosureSummary() << "\n";
    out << "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites
        << ", invalid_ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites
        << ", object_pointer_type_annotation_sites="
        << frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites
        << ", deterministic_ownership_qualifier_lowering_handoff="
        << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites="
        << frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites
        << ", retain_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites
        << ", release_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_release_insertion_sites
        << ", autorelease_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites
        << ", contract_violation_sites="
        << frontend_metadata_.retain_release_operation_lowering_contract_violation_sites
        << ", deterministic_retain_release_operation_lowering_handoff="
        << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
        << ", scope_symbolized_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites
        << ", max_scope_depth="
        << frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth
        << ", scope_entry_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites
        << ", scope_exit_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites
        << ", deterministic_autoreleasepool_scope_lowering_handoff="
        << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites
        << ", weak_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites
        << ", unowned_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites
        << ", unowned_safe_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites
        << ", weak_unowned_conflict_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites
        << ", contract_violation_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites
        << ", deterministic_weak_unowned_semantics_lowering_handoff="
        << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites
        << ", ownership_arc_fixit_available_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites
        << ", ownership_arc_profiled_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites
        << ", ownership_arc_weak_unowned_conflict_diagnostic_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites
        << ", ownership_arc_empty_fixit_hint_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites
        << ", deterministic_arc_diagnostics_fixit_lowering_handoff="
        << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_literal_sites
        << ", block_parameter_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_parameter_entries
        << ", block_capture_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_capture_entries
        << ", block_body_statement_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries
        << ", block_empty_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites
        << ", block_nondeterministic_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites
        << ", block_non_normalized_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_literal_capture_lowering_contract_violation_sites
        << ", deterministic_block_literal_capture_lowering_handoff="
        << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites
        << ", invoke_argument_slots_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total
        << ", capture_word_count_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total
        << ", descriptor_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites
        << ", invoke_trampoline_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites
        << ", missing_invoke_trampoline_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites
        << ", non_normalized_layout_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites
        << ", deterministic_block_abi_invoke_trampoline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_storage_escape_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total
        << ", requires_byref_cells_sites="
        << frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites
        << ", escape_analysis_enabled_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites
        << ", escape_to_heap_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites
        << ", escape_profile_normalized_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites
        << ", byref_layout_symbolized_sites="
        << frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_storage_escape_lowering_contract_violation_sites
        << ", deterministic_block_storage_escape_lowering_handoff="
        << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_copy_dispose_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total
        << ", copy_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites
        << ", dispose_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites
        << ", profile_normalized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites
        << ", copy_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites
        << ", dispose_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites
        << ", deterministic_block_copy_dispose_lowering_handoff="
        << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites
        << ", baseline_weight_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total
        << ", deterministic_capture_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites
        << ", heavy_tier_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites
        << ", normalized_profile_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites
        << ", deterministic_block_determinism_perf_baseline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_lightweight_generic_constraint_lowering_profile = generic_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_constraint_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_suffix_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_object_pointer_type_sites
        << ", terminated_generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_pointer_declarator_sites
        << ", normalized_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_normalized_constraint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_contract_violation_sites
        << ", deterministic_lightweight_generic_constraint_lowering_handoff="
        << (frontend_metadata_.deterministic_lightweight_generic_constraint_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_nullability_flow_warning_precision_lowering_profile = nullability_flow_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_object_pointer_type_sites
        << ", nullability_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullability_suffix_sites
        << ", nullable_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullable_suffix_sites
        << ", nonnull_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nonnull_suffix_sites
        << ", normalized_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_contract_violation_sites
        << ", deterministic_nullability_flow_warning_precision_lowering_handoff="
        << (frontend_metadata_.deterministic_nullability_flow_warning_precision_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_protocol_qualified_object_type_lowering_profile = protocol_qualified_object_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_protocol_composition_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_object_pointer_type_sites
        << ", terminated_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_pointer_declarator_sites
        << ", normalized_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_contract_violation_sites
        << ", deterministic_protocol_qualified_object_type_lowering_handoff="
        << (frontend_metadata_.deterministic_protocol_qualified_object_type_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_variance_bridge_cast_lowering_profile = variance_bridge_cast_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_contract_violation_sites
        << ", deterministic_variance_bridge_cast_lowering_handoff="
        << (frontend_metadata_.deterministic_variance_bridge_cast_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_generic_metadata_abi_lowering_profile = generic_metadata_abi_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_generic_suffix_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_contract_violation_sites
        << ", deterministic_generic_metadata_abi_lowering_handoff="
        << (frontend_metadata_.deterministic_generic_metadata_abi_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_module_import_graph_lowering_profile = module_import_graph_sites="
        << frontend_metadata_.module_import_graph_lowering_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_.module_import_graph_lowering_import_edge_candidate_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.module_import_graph_lowering_namespace_segment_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.module_import_graph_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.module_import_graph_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.module_import_graph_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.module_import_graph_lowering_contract_violation_sites
        << ", deterministic_module_import_graph_lowering_handoff="
        << (frontend_metadata_.deterministic_module_import_graph_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_namespace_collision_shadowing_lowering_profile = namespace_collision_shadowing_sites="
        << frontend_metadata_.namespace_collision_shadowing_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_contract_violation_sites
        << ", deterministic_namespace_collision_shadowing_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_namespace_collision_shadowing_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_public_private_api_partition_lowering_profile = public_private_api_partition_sites="
        << frontend_metadata_.public_private_api_partition_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_contract_violation_sites
        << ", deterministic_public_private_api_partition_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_public_private_api_partition_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_incremental_module_cache_invalidation_lowering_profile = incremental_module_cache_invalidation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_contract_violation_sites
        << ", deterministic_incremental_module_cache_invalidation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_incremental_module_cache_invalidation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_cross_module_conformance_lowering_profile = cross_module_conformance_sites="
        << frontend_metadata_.cross_module_conformance_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.cross_module_conformance_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_contract_violation_sites
        << ", deterministic_cross_module_conformance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_cross_module_conformance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites="
        << frontend_metadata_.throws_propagation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.throws_propagation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.throws_propagation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.throws_propagation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.throws_propagation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_.throws_propagation_lowering_contract_violation_sites
        << ", deterministic_throws_propagation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_throws_propagation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites="
        << frontend_metadata_.ns_error_bridging_lowering_sites
        << ", ns_error_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_parameter_sites
        << ", ns_error_out_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_out_parameter_sites
        << ", ns_error_bridge_path_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_bridge_path_sites
        << ", failable_call_sites="
        << frontend_metadata_.ns_error_bridging_lowering_failable_call_sites
        << ", normalized_sites="
        << frontend_metadata_.ns_error_bridging_lowering_normalized_sites
        << ", bridge_boundary_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_bridge_boundary_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_contract_violation_sites
        << ", deterministic_ns_error_bridging_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_ns_error_bridging_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites="
        << frontend_metadata_.unwind_cleanup_lowering_sites
        << ", unwind_edge_sites="
        << frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites
        << ", cleanup_scope_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites
        << ", cleanup_emit_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites
        << ", landing_pad_sites="
        << frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites
        << ", cleanup_resume_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites
        << ", normalized_sites="
        << frontend_metadata_.unwind_cleanup_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites
        << ", deterministic_unwind_cleanup_lowering_handoff="
        << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_error_diagnostics_recovery_lowering_profile = error_diagnostic_sites="
        << frontend_metadata_.error_diagnostics_recovery_lowering_sites
        << ", parser_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_parser_diagnostic_sites
        << ", semantic_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_semantic_diagnostic_sites
        << ", fixit_hint_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_fixit_hint_sites
        << ", recovery_candidate_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_candidate_sites
        << ", recovery_applied_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_applied_sites
        << ", normalized_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_contract_violation_sites
        << ", deterministic_error_diagnostics_recovery_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_error_diagnostics_recovery_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_async_continuation_lowering_profile = async_continuation_sites="
        << frontend_metadata_.async_continuation_lowering_sites
        << ", async_keyword_sites="
        << frontend_metadata_.async_continuation_lowering_async_keyword_sites
        << ", async_function_sites="
        << frontend_metadata_.async_continuation_lowering_async_function_sites
        << ", continuation_allocation_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_allocation_sites
        << ", continuation_resume_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_resume_sites
        << ", continuation_suspend_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_suspend_sites
        << ", async_state_machine_sites="
        << frontend_metadata_
               .async_continuation_lowering_async_state_machine_sites
        << ", normalized_sites="
        << frontend_metadata_.async_continuation_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_.async_continuation_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .async_continuation_lowering_contract_violation_sites
        << ", deterministic_async_continuation_lowering_handoff="
        << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_await_lowering_suspension_state_lowering_profile = await_suspension_sites="
        << frontend_metadata_.await_lowering_suspension_state_lowering_sites
        << ", await_keyword_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_keyword_sites
        << ", await_suspension_point_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_suspension_point_sites
        << ", await_resume_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_resume_sites
        << ", await_state_machine_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_state_machine_sites
        << ", await_continuation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_continuation_sites
        << ", normalized_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_contract_violation_sites
        << ", deterministic_await_lowering_suspension_state_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_await_lowering_suspension_state_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites="
        << frontend_metadata_.actor_isolation_sendability_lowering_sites
        << ", sendability_check_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendability_check_sites
        << ", cross_actor_hop_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_cross_actor_hop_sites
        << ", non_sendable_capture_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_non_sendable_capture_sites
        << ", sendable_transfer_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendable_transfer_sites
        << ", isolation_boundary_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_isolation_boundary_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_contract_violation_sites
        << ", deterministic_actor_isolation_sendability_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_actor_isolation_sendability_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_actor_lowering_metadata_profile = actor_interface_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_interface_sites
        << ", actor_method_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_method_sites
        << ", actor_metadata_record_sites="
        << frontend_metadata_
               .actor_lowering_metadata_actor_metadata_record_sites
        << ", nonisolated_entry_sites="
        << frontend_metadata_.actor_lowering_metadata_nonisolated_entry_sites
        << ", executor_affinity_sites="
        << frontend_metadata_.actor_lowering_metadata_executor_affinity_sites
        << ", actor_hop_artifact_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_hop_artifact_sites
        << ", actor_isolation_thunk_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_isolation_thunk_sites
        << ", replay_proof_dependency_sites="
        << frontend_metadata_
               .actor_lowering_metadata_replay_proof_dependency_sites
        << ", race_guard_dependency_sites="
        << frontend_metadata_
               .actor_lowering_metadata_race_guard_dependency_sites
        << ", task_handoff_sites="
        << frontend_metadata_.actor_lowering_metadata_task_handoff_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.actor_lowering_metadata_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.actor_lowering_metadata_contract_violation_sites
        << ", deterministic_actor_lowering_metadata_handoff="
        << (frontend_metadata_.deterministic_actor_lowering_metadata_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_dispatch_control_lowering_profile = direct_call_candidate_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_direct_call_candidate_sites
        << ", direct_members_defaulted_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_direct_members_defaulted_sites
        << ", dynamic_opt_out_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_dynamic_opt_out_sites
        << ", final_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_final_container_sites
        << ", sealed_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_sealed_container_sites
        << ", override_legality_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_override_legality_sites
        << ", metadata_preserved_callable_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites
        << ", metadata_preserved_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_metadata_preserved_container_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.dispatch_dispatch_control_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_contract_violation_sites
        << ", deterministic_dispatch_dispatch_control_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_dispatch_dispatch_control_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_interop_interop_lowering_profile = foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_foreign_callable_sites
        << ", c_foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_c_foreign_callable_sites
        << ", objc_runtime_parity_callable_sites="
        << frontend_metadata_.interop_interop_lowering_objc_runtime_parity_callable_sites
        << ", ownership_bridge_callable_sites="
        << frontend_metadata_.interop_interop_lowering_ownership_bridge_callable_sites
        << ", error_surface_sites="
        << frontend_metadata_.interop_interop_lowering_error_surface_sites
        << ", async_boundary_sites="
        << frontend_metadata_.interop_interop_lowering_async_boundary_sites
        << ", swift_concurrency_metadata_sites="
        << frontend_metadata_.interop_interop_lowering_swift_concurrency_metadata_sites
        << ", interface_preserved_foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_interface_preserved_foreign_callable_sites
        << ", interface_preserved_metadata_annotation_sites="
        << frontend_metadata_.interop_interop_lowering_interface_preserved_metadata_annotation_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.interop_interop_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.interop_interop_lowering_contract_violation_sites
        << ", deterministic_interop_interop_lowering_handoff="
        << (frontend_metadata_.deterministic_interop_interop_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_metaprogramming_expansion_lowering_profile = derive_inventory_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_derive_inventory_sites
        << ", derived_selector_artifact_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_derived_selector_artifact_sites
        << ", macro_replay_visible_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_macro_replay_visible_sites
        << ", property_behavior_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_property_behavior_sites
        << ", synthesized_binding_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_binding_sites
        << ", synthesized_getter_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_getter_sites
        << ", synthesized_setter_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_setter_sites
        << ", replay_visible_metadata_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_replay_visible_metadata_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_contract_violation_sites
        << ", deterministic_metaprogramming_expansion_lowering_handoff="
        << (frontend_metadata_.deterministic_metaprogramming_expansion_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_metaprogramming_synthesized_emission_profile = emitted_derive_method_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_derive_method_sites
        << ", emitted_macro_artifact_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_macro_artifact_sites
        << ", emitted_property_behavior_artifact_sites="
        << frontend_metadata_
               .metaprogramming_synthesized_emitted_property_behavior_artifact_sites
        << ", emitted_global_artifact_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_global_artifact_sites
        << ", emitted_runtime_method_list_sites="
        << frontend_metadata_
               .metaprogramming_synthesized_emitted_runtime_method_list_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.metaprogramming_synthesized_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.metaprogramming_synthesized_contract_violation_sites
        << ", deterministic_metaprogramming_synthesized_emission_handoff="
        << (frontend_metadata_.deterministic_metaprogramming_synthesized_emission_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_direct_callable_record_count
        << ", local_final_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_final_callable_record_count
        << ", local_final_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_final_container_record_count
        << ", local_sealed_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_sealed_container_record_count
        << ", imported_module_count="
        << frontend_metadata_.dispatch_dispatch_metadata_imported_module_count
        << ", imported_direct_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_direct_callable_record_count
        << ", imported_final_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_final_callable_record_count
        << ", imported_final_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_final_container_record_count
        << ", imported_sealed_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_sealed_container_record_count
        << ", runtime_import_artifact_ready="
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_runtime_import_artifact_ready
                ? "true"
                : "false")
        << ", separate_compilation_preservation_ready="
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_separate_compilation_preservation_ready
                ? "true"
                : "false")
        << ", deterministic_dispatch_dispatch_metadata_interface_handoff="
        << (frontend_metadata_
                    .deterministic_dispatch_dispatch_metadata_interface_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites="
        << frontend_metadata_.ownership_system_extension_lowering_cleanup_hook_sites
        << ", resource_local_sites="
        << frontend_metadata_.ownership_system_extension_lowering_resource_local_sites
        << ", cleanup_owned_local_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_cleanup_owned_local_sites
        << ", resource_move_capture_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_resource_move_capture_sites
        << ", borrowed_parameter_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_parameter_sites
        << ", borrowed_return_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_return_callable_sites
        << ", borrowed_escape_candidate_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_escape_candidate_sites
        << ", explicit_capture_item_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_explicit_capture_item_sites
        << ", retainable_family_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_callable_sites
        << ", retainable_family_operation_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_operation_callable_sites
        << ", retainable_family_alias_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_alias_callable_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.ownership_system_extension_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_contract_violation_sites
        << ", deterministic_ownership_system_extension_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_ownership_system_extension_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_ownership_borrowed_retainable_abi_profile = returns_borrowed_attribute_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_returns_borrowed_attribute_sites
        << ", family_retain_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_retain_sites
        << ", family_release_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_release_sites
        << ", family_autorelease_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_autorelease_sites
        << ", compatibility_returns_retained_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_returns_retained_sites
        << ", compatibility_returns_not_retained_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
        << ", compatibility_consumed_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_consumed_sites
        << ", deterministic_ownership_borrowed_retainable_abi_completion_handoff="
        << (frontend_metadata_
                    .deterministic_ownership_borrowed_retainable_abi_completion_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites="
        << frontend_metadata_.task_runtime_interop_cancellation_lowering_sites
        << ", task_runtime_interop_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_interop_sites
        << ", cancellation_probe_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_probe_sites
        << ", cancellation_handler_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_handler_sites
        << ", runtime_resume_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_resume_sites
        << ", runtime_cancel_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_cancel_sites
        << ", normalized_sites="
        << frontend_metadata_
