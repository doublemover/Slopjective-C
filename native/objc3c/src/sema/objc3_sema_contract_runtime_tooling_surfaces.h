#pragma once

#include "sema/objc3_sema_contract_metaprogramming_surfaces.h"

struct Objc3TaskRuntimeCancellationSummary {
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardSummary {
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnsafePointerExtensionSummary {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InlineAsmIntrinsicGovernanceSummary {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingSummary {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ResultLikeLoweringSummary {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ErrorDiagnosticsRecoverySummary {
  std::size_t error_diagnostics_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsContractId =
        "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_cross_module_semantic_contracts_and_diagnostics";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsRule =
        "module-import-namespace-api-partition-cache-invalidation-cross-module-conformance-and-diagnostic-recovery-packets-share-one-deterministic-semantic-contract-rooted-in-live-sema-and-lowering-surfaces";

struct Objc3CrossModuleSemanticContractsDiagnosticsSummary {
  std::string contract_id =
      kObjc3CrossModuleSemanticContractsDiagnosticsContractId;
  std::string surface_path =
      kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3CrossModuleSemanticContractsDiagnosticsRule;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t public_private_api_partition_sites = 0;
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t cross_module_conformance_sites = 0;
  std::size_t normalized_cross_module_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t diagnostic_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t diagnostic_normalized_sites = 0;
  std::size_t diagnostic_gate_blocked_sites = 0;
  std::size_t interop_import_module_annotation_sites = 0;
  std::size_t interop_imported_module_name_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool module_import_graph_semantics_landed = false;
  bool namespace_collision_semantics_landed = false;
  bool public_private_partition_semantics_landed = false;
  bool incremental_cache_semantics_landed = false;
  bool cross_module_conformance_semantics_landed = false;
  bool diagnostic_recovery_semantics_landed = false;
  bool interop_import_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3SymbolGraphScopeResolutionSummary {
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic = true;

  std::size_t symbol_nodes_total() const {
    return global_symbol_nodes + function_symbol_nodes + interface_symbol_nodes + implementation_symbol_nodes +
           interface_property_symbol_nodes + implementation_property_symbol_nodes + interface_method_symbol_nodes +
           implementation_method_symbol_nodes;
  }

  std::size_t resolution_sites_total() const {
    return implementation_interface_resolution_sites + method_resolution_sites;
  }

  std::size_t resolution_hits_total() const {
    return implementation_interface_resolution_hits + method_resolution_hits;
  }

  std::size_t resolution_misses_total() const {
    return implementation_interface_resolution_misses + method_resolution_misses;
  }
};

struct Objc3MethodLookupOverrideConflictSummary {
  // anchor: lane-B executable metadata semantic validation consumes
  // this deterministic override summary as the canonical legality handoff for
  // superclass override checks before lowering admission exists.
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;

  std::size_t total_lookup_sites() const { return method_lookup_sites + override_lookup_sites; }
  std::size_t total_lookup_hits() const { return method_lookup_hits + override_lookup_hits; }
  std::size_t total_lookup_misses() const { return method_lookup_misses + override_lookup_misses; }
};

struct Objc3PropertySynthesisIvarBindingSummary {
  // export-legality anchor: these counts are the canonical sema
  // preconditions for property/ivar runtime export and must not degrade into
  // generic property-declaration totals.
  // semantic-closure gate anchor: lane-E consumes this property/ivar
  // legality summary together with the A003 graph, C003 projection, and D002
  // packaging proofs before the next runtime step section emission begins.
  // corpus-sync anchor: representative legality corpus cases keep
  // these counts deterministic so docs and integrated gate coverage stay
  // aligned on the real runner path.
  // default-binding semantics anchor: matched class implementations
  // resolve synthesis from interface-declared properties first, then fold in
  // optional implementation redeclarations without making redeclaration a
  // prerequisite for default ivar binding.
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypeCheckingSummary {
  std::size_t canonical_reference_form_count = 0;
  std::size_t canonical_message_scalar_form_count = 0;
  std::size_t canonical_bridge_top_form_count = 0;
  std::size_t param_type_sites = 0;
  std::size_t param_id_spelling_sites = 0;
  std::size_t param_class_spelling_sites = 0;
  std::size_t param_sel_spelling_sites = 0;
  std::size_t param_instancetype_spelling_sites = 0;
  std::size_t param_object_pointer_type_sites = 0;
  std::size_t return_type_sites = 0;
  std::size_t return_id_spelling_sites = 0;
  std::size_t return_class_spelling_sites = 0;
  std::size_t return_sel_spelling_sites = 0;
  std::size_t return_instancetype_spelling_sites = 0;
  std::size_t return_object_pointer_type_sites = 0;
  std::size_t property_type_sites = 0;
  std::size_t property_id_spelling_sites = 0;
  std::size_t property_class_spelling_sites = 0;
  std::size_t property_sel_spelling_sites = 0;
  std::size_t property_instancetype_spelling_sites = 0;
  std::size_t property_object_pointer_type_sites = 0;
  bool canonical_reference_forms_unique = false;
  bool canonical_message_scalar_forms_unique = false;
  bool canonical_bridge_top_forms_unique = false;
  bool canonical_bridge_top_subset_of_reference = false;
  bool canonical_type_form_scaffold_ready = false;
  bool canonical_type_form_diagnostics_hardening_consistent = false;
  bool canonical_type_form_diagnostics_hardening_ready = false;
  std::string canonical_type_form_diagnostics_hardening_key;
  bool canonical_type_form_recovery_determinism_consistent = false;
  bool canonical_type_form_recovery_determinism_ready = false;
  std::string canonical_type_form_recovery_determinism_key;
  bool canonical_type_form_conformance_matrix_consistent = false;
  bool canonical_type_form_conformance_matrix_ready = false;
  std::string canonical_type_form_conformance_matrix_key;
  std::size_t canonical_type_form_conformance_corpus_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_passed_case_count = 0;
  std::size_t canonical_type_form_conformance_corpus_failed_case_count = 0;
  bool canonical_type_form_conformance_corpus_consistent = false;
  bool canonical_type_form_conformance_corpus_ready = false;
  std::string canonical_type_form_conformance_corpus_key;
  std::size_t canonical_type_form_performance_quality_required_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_passed_guardrail_count = 0;
  std::size_t canonical_type_form_performance_quality_failed_guardrail_count = 0;
  bool canonical_type_form_performance_quality_guardrails_consistent = false;
  bool canonical_type_form_performance_quality_guardrails_ready = false;
  std::string canonical_type_form_performance_quality_guardrails_key;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringSiteMetadata {
  std::string selector;
  std::string selector_lowering_symbol;
  std::size_t argument_count = 0;
  std::size_t selector_piece_count = 0;
  std::size_t selector_argument_piece_count = 0;
  bool unary_form = false;
  bool keyword_form = false;
  bool selector_lowering_is_normalized = false;
  bool receiver_is_nil_literal = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  bool nil_receiver_semantics_is_normalized = false;
  bool runtime_link_host_link_required = true;
  bool runtime_link_host_link_elided = false;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_link_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_link_host_link_symbol;
  bool runtime_link_host_link_is_normalized = false;
  bool receiver_is_super_identifier = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  bool method_family_semantics_is_normalized = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3MessageSendSelectorLoweringSummary {
  std::size_t message_send_sites = 0;
  std::size_t unary_form_sites = 0;
  std::size_t keyword_form_sites = 0;
  std::size_t selector_lowering_symbol_sites = 0;
  std::size_t selector_lowering_piece_entries = 0;
  std::size_t selector_lowering_argument_piece_entries = 0;
  std::size_t selector_lowering_normalized_sites = 0;
  std::size_t selector_lowering_form_mismatch_sites = 0;
  std::size_t selector_lowering_arity_mismatch_sites = 0;
  std::size_t selector_lowering_symbol_mismatch_sites = 0;
  std::size_t selector_lowering_missing_symbol_sites = 0;
  std::size_t selector_lowering_contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingSummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots = 0;
  std::size_t selector_symbol_slots = 0;
  std::size_t argument_slots = 0;
  std::size_t keyword_argument_slots = 0;
  std::size_t unary_argument_slots = 0;
  std::size_t arity_mismatch_sites = 0;
  std::size_t missing_selector_symbol_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilitySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkSummary {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationSummary {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsSummary {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitSummary {
  // freezes the advanced diagnostics taxonomy/portability contract on
  // top of this deterministic ARC/fix-it baseline rather than inventing a
  // parallel semantic diagnostics summary.
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityContractId =
    "objc3c.tooling.diagnostic.taxonomy.portability.contract.v1";
inline constexpr const char *kObjc3ToolingDiagnosticTaxonomyPortabilityDiagnosticNamespace =
    "O3S";
inline constexpr const char *kObjc3ToolingFeatureSpecificFixitSynthesisContractId =
    "objc3c.tooling.feature.specific.fixit.synthesis.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode = "O3S216";
