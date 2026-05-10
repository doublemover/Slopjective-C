#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "parse/objc3_parser_contract.h"
#include "token/objc3_token_contract.h"

inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMajor = 1;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMinor = 0;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionPatch = 0;
inline constexpr const char *kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr std::array<ValueType, 6> kObjc3CanonicalReferenceTypeForms = {
    ValueType::ObjCId,
    ValueType::ObjCClass,
    ValueType::ObjCSel,
    ValueType::ObjCProtocol,
    ValueType::ObjCInstancetype,
    ValueType::ObjCObjectPtr,
};
inline constexpr std::array<ValueType, 2> kObjc3CanonicalScalarMessageSendTypeForms = {
    ValueType::I32,
    ValueType::Bool,
};
inline constexpr std::array<ValueType, 5> kObjc3CanonicalBridgeTopReferenceTypeForms = {
    ValueType::ObjCId,
    ValueType::ObjCClass,
    ValueType::ObjCProtocol,
    ValueType::ObjCInstancetype,
    ValueType::ObjCObjectPtr,
};
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticsContractId =
    "objc3c.compatibility.strictness.claim.semantics.v1";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics";
// runnable-core compatibility guard anchor: sema owns the current
// truthful split between live selections, source-only downgraded claims, and
// fail-closed unsupported advanced surfaces around the runnable core.
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticModel =
    "canonical-language-profile-source-only-downgrade-unsupported-fail-closed";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimDowngradeModel =
    "source-only-claims-remain-recognized-but-never-promote-to-runnable";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimRejectionModel =
    "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel =
    "no-standalone-interface-payload-yet-and-any-future-canonical-interface-must-stay-bounded-to-runnable-and-source-downgraded-claims";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel =
    "suppressed-feature-macro-claims-remain-unpublished-across-manifest-interface-and-conformance-surfaces-until-executable";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode =
    "no-standalone-interface-payload-yet";
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount = 1u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount = 2u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount = 1u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRunnableFeatureCount = 7u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount = 6u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedFeatureCount = 7u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount = 3u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount = 3u;

inline bool IsObjc3CanonicalReferenceTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalReferenceTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return false;
}

inline bool IsObjc3CanonicalMessageSendTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalScalarMessageSendTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return IsObjc3CanonicalReferenceTypeForm(type);
}

inline bool IsObjc3CanonicalBridgeTopReferenceTypeForm(ValueType type) {
  for (const ValueType candidate : kObjc3CanonicalBridgeTopReferenceTypeForms) {
    if (candidate == type) {
      return true;
    }
  }
  return false;
}

enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
  Unsupported = 5,
};

struct Objc3AtomicMemoryOrderMappingSummary {
  std::size_t relaxed = 0;
  std::size_t acquire = 0;
  std::size_t release = 0;
  std::size_t acq_rel = 0;
  std::size_t seq_cst = 0;
  std::size_t unsupported = 0;
  bool deterministic = true;

  std::size_t total() const { return relaxed + acquire + release + acq_rel + seq_cst + unsupported; }
};

struct Objc3VectorTypeLoweringSummary {
  std::size_t return_annotations = 0;
  std::size_t param_annotations = 0;
  std::size_t i32_annotations = 0;
  std::size_t bool_annotations = 0;
  std::size_t lane2_annotations = 0;
  std::size_t lane4_annotations = 0;
  std::size_t lane8_annotations = 0;
  std::size_t lane16_annotations = 0;
  std::size_t unsupported_annotations = 0;
  bool deterministic = true;

  std::size_t total() const { return return_annotations + param_annotations; }
};

struct Objc3ProtocolCategoryCompositionSummary {
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const { return protocol_composition_sites + category_composition_sites; }
};

struct Objc3ClassProtocolCategoryLinkingSummary {
  // completeness anchor: these deterministic counts validate the
  // first-class executable metadata graph packet.
  // completion anchor: protocol/category composition counts stay
  // stable so protocol/category/property/ivar export graph closure can fail
  // closed on deterministic sema inputs.
  // freeze anchor: these counts remain the canonical semantic
  // consistency inputs for executable metadata graph admission.
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const { return protocol_composition_sites + category_composition_sites; }
};

struct Objc3SelectorNormalizationSummary {
  std::size_t methods_total = 0;
  std::size_t normalized_methods = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_parameter_piece_entries = 0;
  std::size_t selector_pieceless_methods = 0;
  std::size_t selector_spelling_mismatches = 0;
  std::size_t selector_arity_mismatches = 0;
  std::size_t selector_parameter_linkage_mismatches = 0;
  std::size_t selector_normalization_flag_mismatches = 0;
  std::size_t selector_missing_keyword_pieces = 0;
  bool deterministic = true;

  std::size_t contract_violations() const {
    return selector_pieceless_methods + selector_spelling_mismatches + selector_arity_mismatches +
           selector_parameter_linkage_mismatches + selector_normalization_flag_mismatches +
           selector_missing_keyword_pieces;
  }
};

struct Objc3PropertyAttributeSummary {
  std::size_t properties_total = 0;
  std::size_t attribute_entries = 0;
  std::size_t readonly_modifiers = 0;
  std::size_t readwrite_modifiers = 0;
  std::size_t atomic_modifiers = 0;
  std::size_t nonatomic_modifiers = 0;
  std::size_t copy_modifiers = 0;
  std::size_t strong_modifiers = 0;
  std::size_t weak_modifiers = 0;
  std::size_t assign_modifiers = 0;
  std::size_t getter_modifiers = 0;
  std::size_t setter_modifiers = 0;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool deterministic = true;

  std::size_t ownership_modifiers() const { return copy_modifiers + strong_modifiers + weak_modifiers + assign_modifiers; }
  std::size_t contract_violations() const { return invalid_attribute_entries + property_contract_violations; }
};

struct Objc3TypeAnnotationSurfaceSummary {
  std::size_t generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t invalid_generic_suffix_sites = 0;
  std::size_t invalid_pointer_declarator_sites = 0;
  std::size_t invalid_nullability_suffix_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  bool deterministic = true;

  std::size_t total_type_annotation_sites() const {
    return generic_suffix_sites + pointer_declarator_sites + nullability_suffix_sites + ownership_qualifier_sites;
  }

  std::size_t invalid_type_annotation_sites() const {
    return invalid_generic_suffix_sites + invalid_pointer_declarator_sites + invalid_nullability_suffix_sites +
           invalid_ownership_qualifier_sites;
  }
};

struct Objc3LightweightGenericConstraintSummary {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NullabilityFlowWarningPrecisionSummary {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ProtocolQualifiedObjectTypeSummary {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3VarianceBridgeCastSummary {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3GenericMetadataAbiSummary {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3TypeSystemTypeSemanticModelSummary {
  std::string contract_id = "objc3c.type_system.type.semantic.model.v1";
  std::string surface_path =
      "frontend.pipeline.semantic_surface.objc_type_system_type_semantic_model";
  std::string semantic_model =
      "optional-bindings-optional-sends-erased-generic-metadata-and-typed-keypath-shape-obey-one-fail-closed-sema-model-before-lowering";
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t guard_binding_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t optional_propagation_sites = 0;
  std::size_t optional_flow_refinement_sites = 0;
  std::size_t guard_binding_exit_enforcement_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t object_pointer_semantic_sites = 0;
  std::size_t protocol_composition_semantic_sites = 0;
  std::size_t generic_suffix_semantic_sites = 0;
  std::size_t generic_erasure_semantic_sites = 0;
  std::size_t nullability_suffix_semantic_sites = 0;
  std::size_t nullability_semantic_sites = 0;
  std::size_t canonical_type_entries = 0;
  std::size_t canonical_object_type_entries = 0;
  std::size_t canonical_protocol_qualified_entries = 0;
  std::size_t canonical_generic_argument_entries = 0;
  std::size_t canonical_nullable_entries = 0;
  std::size_t canonical_nonnull_entries = 0;
  std::size_t canonical_implicitly_unwrapped_entries = 0;
  std::size_t canonical_null_resettable_entries = 0;
  std::size_t canonical_unspecified_nullability_entries = 0;
  std::size_t canonical_invalid_type_entries = 0;
  std::size_t invalid_generic_suffix_semantic_sites = 0;
  std::size_t invalid_nullability_suffix_semantic_sites = 0;
  std::size_t invalid_protocol_composition_semantic_sites = 0;
  std::size_t optional_binding_contract_violation_sites = 0;
  std::size_t optional_send_contract_violation_sites = 0;
  std::size_t optional_flow_contract_violation_sites = 0;
  std::size_t typed_keypath_root_legality_violation_sites = 0;
  std::size_t typed_keypath_member_path_contract_violation_sites = 0;
  std::size_t typed_keypath_contract_violation_sites = 0;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
};

inline constexpr const char *kObjc3EffectsOwnershipSemanticModelContractId =
    "objc3c.effects.ownership.semantic.model.closure.v1";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_effects_ownership_semantic_model";
inline constexpr const char *kObjc3EffectsOwnershipSemanticModelRule =
    "arc-block-throws-async-actor-and-foreign-boundary-semantics-share-one-deterministic-effects-and-ownership-summary-rooted-in-live-sema-lowering-surfaces";

struct Objc3EffectsOwnershipSemanticModelSummary {
  std::string contract_id = kObjc3EffectsOwnershipSemanticModelContractId;
  std::string surface_path = kObjc3EffectsOwnershipSemanticModelSurfacePath;
  std::string semantic_model = kObjc3EffectsOwnershipSemanticModelRule;
  std::size_t arc_ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t weak_zeroing_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t autoreleasepool_scope_sites = 0;
  std::size_t cleanup_order_exit_sites = 0;
  std::size_t block_literal_sites = 0;
  std::size_t stack_to_heap_promotion_sites = 0;
  std::size_t byref_forwarding_cell_sites = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t captured_object_lifetime_sites = 0;
  std::size_t throws_propagation_sites = 0;
  std::size_t unwind_cleanup_sites = 0;
  std::size_t bridged_error_sites = 0;
  std::size_t nested_cleanup_sites = 0;
  std::size_t foreign_boundary_sites = 0;
  std::size_t async_continuation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t reentrancy_policy_sites = 0;
  std::size_t imported_actor_api_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool arc_semantics_landed = false;
  bool block_escape_semantics_landed = false;
  bool throws_cleanup_semantics_landed = false;
  bool async_task_semantics_landed = false;
  bool actor_semantics_landed = false;
  bool foreign_boundary_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char
    *kObjc3ControlFlowControlFlowSemanticModelFrontendDependencyContractId =
        "objc3c.control_flow.control.flow.source.closure.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelContractId =
    "objc3c.control_flow.control.flow.semantic.model.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_control_flow_control_flow_semantic_model";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelRule =
    "guard-refinement-plus-statement-match-exhaustiveness-and-defer-legality-semantics-are-live-while-defer-cleanup-lowering-remains-a-later-lane-c-runtime-step";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelDeferRule =
    "defer-statement-lifo-cleanup-order-and-defer-mediated-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred-to-later-lowering-and-runtime-work";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelMatchRule =
    "statement-match-enforces-catch-all-bool-and-result-case-exhaustiveness-with-case-local-binding-scopes-while-result-payload-typing-remains-deferred";
inline constexpr const char *kObjc3ControlFlowControlFlowSemanticModelExitRule =
    "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred";

struct Objc3ControlFlowControlFlowSemanticModelSummary {
  std::string contract_id = kObjc3ControlFlowControlFlowSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ControlFlowControlFlowSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3ControlFlowControlFlowSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ControlFlowControlFlowSemanticModelRule;
  std::string defer_model = kObjc3ControlFlowControlFlowSemanticModelDeferRule;
  std::string match_model = kObjc3ControlFlowControlFlowSemanticModelMatchRule;
  std::string non_local_exit_model = kObjc3ControlFlowControlFlowSemanticModelExitRule;
  std::size_t guard_binding_semantic_sites = 0;
  std::size_t guard_binding_clause_semantic_sites = 0;
  std::size_t guard_condition_statement_sites = 0;
  std::size_t guard_condition_clause_semantic_sites = 0;
  std::size_t guard_exit_enforcement_sites = 0;
  std::size_t guard_refinement_sites = 0;
  std::size_t match_statement_semantic_sites = 0;
  std::size_t match_default_pattern_sites = 0;
  std::size_t match_wildcard_pattern_sites = 0;
  std::size_t match_literal_pattern_sites = 0;
  std::size_t match_binding_scope_sites = 0;
  std::size_t match_result_case_scope_sites = 0;
  std::size_t match_exhaustive_statement_sites = 0;
  std::size_t match_bool_exhaustive_sites = 0;
  std::size_t match_result_case_exhaustive_sites = 0;
  std::size_t match_non_exhaustive_diagnostic_sites = 0;
  std::size_t match_exhaustiveness_deferred_sites = 0;
  std::size_t defer_statement_semantic_sites = 0;
  std::size_t defer_scope_cleanup_order_sites = 0;
  std::size_t defer_nonlocal_exit_diagnostic_sites = 0;
  std::size_t break_statement_sites = 0;
  std::size_t continue_statement_sites = 0;
  std::size_t break_restriction_diagnostic_sites = 0;
  std::size_t continue_restriction_diagnostic_sites = 0;
  bool source_dependency_required = false;
  bool guard_refinement_semantics_landed = false;
  bool guard_exit_enforcement_landed = false;
  bool match_binding_scope_semantics_landed = false;
  bool match_result_case_scope_semantics_landed = false;
  bool match_exhaustiveness_semantics_landed = false;
  bool match_exhaustiveness_deferred = false;
  bool defer_cleanup_order_semantics_landed = false;
  bool defer_nonlocal_exit_semantics_landed = false;
  bool defer_cleanup_order_deferred = false;
  bool defer_nonlocal_exit_deferred = false;
  bool non_local_exit_restrictions_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ControlFlowControlFlowSemanticModelSummary(
    const Objc3ControlFlowControlFlowSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.defer_model.empty() && !summary.match_model.empty() &&
         !summary.non_local_exit_model.empty() &&
         summary.source_dependency_required &&
         summary.guard_refinement_semantics_landed &&
         summary.guard_exit_enforcement_landed &&
         summary.match_binding_scope_semantics_landed &&
         summary.match_result_case_scope_semantics_landed &&
         summary.match_exhaustiveness_semantics_landed &&
         !summary.match_exhaustiveness_deferred &&
         summary.defer_cleanup_order_semantics_landed &&
         summary.defer_nonlocal_exit_semantics_landed &&
         !summary.defer_cleanup_order_deferred &&
         !summary.defer_nonlocal_exit_deferred &&
         summary.non_local_exit_restrictions_landed &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId =
    "objc3c.error_handling.error.source.closure.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelContractId =
    "objc3c.error_handling.error.semantic.model.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_semantic_model";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelRule =
    "throws-declaration-semantics-plus-deterministic-result-and-nserror-profile-carriage-are-live-while-try-throw-do-catch-propagation-and-native-error-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3ErrorHandlingErrorSemanticModelDeferredRule =
    "try-throw-do-catch-postfix-propagation-status-to-error-execution-bridge-temporaries-and-native-thrown-error-abi-remain-fail-closed-or-later-lane-work";

struct Objc3ErrorHandlingErrorSemanticModelSummary {
  std::string contract_id = kObjc3ErrorHandlingErrorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ErrorHandlingErrorSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingErrorSemanticModelSurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingErrorSemanticModelRule;
  std::string deferred_model = kObjc3ErrorHandlingErrorSemanticModelDeferredRule;
  std::size_t throws_declaration_sites = 0;
  std::size_t function_throws_declaration_sites = 0;
  std::size_t method_throws_declaration_sites = 0;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t placeholder_throws_propagation_sites = 0;
  std::size_t placeholder_unwind_cleanup_sites = 0;
  bool source_dependency_required = false;
  bool throws_declaration_semantics_landed = false;
  bool result_carrier_profile_semantics_landed = false;
  bool ns_error_bridging_profile_semantics_landed = false;
  bool bridge_marker_semantics_landed = false;
  bool parser_fail_closed_boundary_required = false;
  bool parser_fail_closed_boundary_preserved = false;
  bool propagation_runtime_deferred = false;
  bool status_to_error_runtime_deferred = false;
  bool native_error_abi_deferred = false;
  bool placeholder_throws_summary_carried = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId =
    "objc3c.concurrency.async.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId =
    "objc3c.concurrency.async.effect.suspension.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule =
    "async-effect-and-await-legality-semantics-plus-deterministic-continuation-suspension-and-concurrency-profile-carriage-are-live-while-runnable-frame-lowering-cleanup-and-executor-runtime-integration-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule =
    "async-frame-abi-resume-lowering-suspension-cleanup-task-runtime-execution-and-executor-dispatch-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  bool source_dependency_required = false;
  bool async_declaration_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool await_legality_semantics_landed = false;
  bool continuation_profile_semantics_landed = false;
  bool await_suspension_profile_semantics_landed = false;
  bool actor_isolation_sendability_semantics_landed = false;
  bool task_runtime_cancellation_semantics_landed = false;
  bool concurrency_replay_race_guard_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.async_declaration_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.await_legality_semantics_landed &&
         summary.continuation_profile_semantics_landed &&
         summary.await_suspension_profile_semantics_landed &&
         summary.actor_isolation_sendability_semantics_landed &&
         summary.task_runtime_cancellation_semantics_landed &&
         summary.concurrency_replay_race_guard_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId =
    "objc3c.concurrency.task.group.cancellation.source.closure.v1";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId =
    "objc3c.concurrency.task.executor.cancellation.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule =
    "task-lifetime-executor-affinity-cancellation-observation-and-structured-task-legality-are-live-in-sema-while-runnable-task-allocation-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule =
    "task-allocation-executor-hop-runtime-task-group-execution-and-scheduler-backed-cancellation-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  bool source_dependency_required = false;
  bool task_lifetime_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool cancellation_observation_semantics_landed = false;
  bool structured_task_legality_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyTaskExecutorCancellationSemanticModelSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.task_lifetime_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.cancellation_observation_semantics_landed &&
         summary.structured_task_legality_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelDependencyContractId =
    "objc3c.ownership.retainable.c.family.source.completion.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelContractId =
    "objc3c.ownership.system.extension.semantic.model.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_ownership_system_extension_semantic_model";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelRule =
    "cleanup-resource-borrowed-capture-and-retainable-family-source-surfaces-now-share-one-truthful-sema-model-while-resource-move-borrowed-escape-and-runtime-interop-work-remain-later-runtime-lanes";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelDeferredRule =
    "resource-move-use-after-move-borrowed-escape-retainable-family-legality-and-runtime-interop-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipSystemExtensionSemanticModelSummary {
  std::string contract_id = kObjc3OwnershipSystemExtensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3OwnershipSystemExtensionSemanticModelDependencyContractId;
  std::string surface_path = kObjc3OwnershipSystemExtensionSemanticModelSurfacePath;
  std::string semantic_model = kObjc3OwnershipSystemExtensionSemanticModelRule;
  std::string deferred_model = kObjc3OwnershipSystemExtensionSemanticModelDeferredRule;
  std::size_t cleanup_attribute_sites = 0;
  std::size_t cleanup_sugar_sites = 0;
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_sugar_sites = 0;
  std::size_t borrowed_pointer_sites = 0;
  std::size_t returns_borrowed_attribute_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_annotation_sites = 0;
  std::size_t retainable_family_compatibility_alias_sites = 0;
  bool source_dependency_required = false;
  bool cleanup_resource_semantic_model_frozen = false;
  bool borrowed_pointer_semantic_model_frozen = false;
  bool capture_legality_semantic_model_frozen = false;
  bool retainable_family_semantic_model_frozen = false;
  bool resource_move_semantics_deferred = false;
  bool borrowed_escape_semantics_deferred = false;
  bool retainable_family_legality_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipSystemExtensionSemanticModelSummary(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.cleanup_resource_semantic_model_frozen &&
         summary.borrowed_pointer_semantic_model_frozen &&
         summary.capture_legality_semantic_model_frozen &&
         summary.retainable_family_semantic_model_frozen &&
         summary.resource_move_semantics_deferred &&
         summary.borrowed_escape_semantics_deferred &&
         summary.retainable_family_legality_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDependencyContractId =
        "objc3c.ownership.system.extension.semantic.model.v1";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsContractId =
        "objc3c.ownership.resource.move.use.after.move.semantics.v1";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsRule =
        "cleanup-owned-resource-locals-now-transfer-cleanup-ownership-through-explicit-move-captures-and-fail-closed-on-use-after-move-while-borrowed-escape-retainable-family-legality-lowering-and-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDeferredRule =
        "borrowed-escape-retainable-family-legality-lowering-and-runtime-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary {
  std::string contract_id =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsRule;
  std::string deferred_model =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDeferredRule;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t illegal_non_resource_move_sites = 0;
  std::size_t illegal_use_after_move_sites = 0;
  std::size_t illegal_duplicate_move_sites = 0;
  bool dependency_required = false;
  bool cleanup_ownership_transfer_enforced = false;
  bool use_after_move_fail_closed = false;
  bool duplicate_move_fail_closed = false;
  bool borrowed_escape_semantics_deferred = false;
  bool retainable_family_legality_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipResourceMoveUseAfterMoveSemanticsSummary(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.cleanup_ownership_transfer_enforced &&
         summary.use_after_move_fail_closed &&
         summary.duplicate_move_fail_closed &&
         summary.borrowed_escape_semantics_deferred &&
         summary.retainable_family_legality_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId =
        "objc3c.ownership.resource.move.use.after.move.semantics.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_borrowed_pointer_escape_analysis";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisRule =
        "borrowed-pointer-bindings-now-fail-closed-on-unproven-call-boundaries-escaping-block-capture-and-invalid-borrowed-return-contracts-while-retainable-family-legality-lowering-and-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule =
        "retainable-family-legality-lowering-and-runtime-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary {
  std::string contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisRule;
  std::string deferred_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t illegal_unproven_call_escape_sites = 0;
  std::size_t illegal_escaping_block_capture_sites = 0;
  std::size_t illegal_borrowed_return_sites = 0;
  bool dependency_required = false;
  bool borrowed_call_boundary_enforced = false;
  bool escaping_block_capture_fail_closed = false;
  bool borrowed_return_contract_enforced = false;
  bool retainable_family_legality_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.borrowed_call_boundary_enforced &&
         summary.escaping_block_capture_fail_closed &&
         summary.borrowed_return_contract_enforced &&
         summary.retainable_family_legality_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId =
        "objc3c.ownership.capture.list.retainable.family.legality.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule =
        "explicit-capture-lists-now-fail-closed-on-duplicate-unused-and-non-object-ownership-modes-while-retainable-family-callables-fail-closed-on-conflicting-and-shape-invalid-annotations-before-lowering-and-runtime-integration";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule =
        "lowering-runtime-interop-and-runnable-retainable-family-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule;
  std::string deferred_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_ownership_mode_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t illegal_duplicate_explicit_capture_sites = 0;
  std::size_t illegal_non_object_capture_mode_sites = 0;
  std::size_t illegal_unused_explicit_capture_sites = 0;
  std::size_t illegal_conflicting_retainable_family_sites = 0;
  std::size_t illegal_invalid_family_operation_shape_sites = 0;
  std::size_t illegal_invalid_family_alias_shape_sites = 0;
  bool dependency_required = false;
  bool explicit_capture_duplicate_fail_closed = false;
  bool explicit_capture_ownership_mode_enforced = false;
  bool explicit_capture_inventory_enforced = false;
  bool retainable_family_conflict_enforced = false;
  bool retainable_family_operation_shape_enforced = false;
  bool retainable_family_alias_shape_enforced = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool
IsReadyObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.explicit_capture_duplicate_fail_closed &&
         summary.explicit_capture_ownership_mode_enforced &&
         summary.explicit_capture_inventory_enforced &&
         summary.retainable_family_conflict_enforced &&
         summary.retainable_family_operation_shape_enforced &&
         summary.retainable_family_alias_shape_enforced &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropInteropSemanticModelDependencyContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char *kObjc3InteropInteropSemanticModelContractId =
    "objc3c.interop.interop.semantic.model.v1";
inline constexpr const char *kObjc3InteropInteropSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_interop_interop_semantic_model";
inline constexpr const char *kObjc3InteropInteropSemanticModelRule =
    "foreign-import-swift-cpp-facing-annotation-surfaces-now-freeze-one-deterministic-sema-model-over-existing-ownership-error-async-and-actor-interaction-profiles-while-ffi-lowering-and-runnable-bridge-generation-remain-later-runtime-work";
inline constexpr const char *kObjc3InteropInteropSemanticModelDeferredRule =
    "ffi-abi-lowering-runtime-bridge-gates-cross-language-object-ownership-and-runnable-call-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropInteropSemanticModelSummary {
  std::string contract_id = kObjc3InteropInteropSemanticModelContractId;
  std::string dependency_contract_id =
      kObjc3InteropInteropSemanticModelDependencyContractId;
  std::string surface_path = kObjc3InteropInteropSemanticModelSurfacePath;
  std::string semantic_model = kObjc3InteropInteropSemanticModelRule;
  std::string deferred_model = kObjc3InteropInteropSemanticModelDeferredRule;
  std::size_t foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t imported_module_name_sites = 0;
  std::size_t export_header_annotation_sites = 0;
  std::size_t export_header_name_sites = 0;
  std::size_t mixed_image_annotation_sites = 0;
  std::size_t mixed_image_name_sites = 0;
  std::size_t package_entry_annotation_sites = 0;
  std::size_t package_entry_name_sites = 0;
  std::size_t swift_name_annotation_sites = 0;
  std::size_t swift_private_annotation_sites = 0;
  std::size_t cpp_name_annotation_sites = 0;
  std::size_t header_name_annotation_sites = 0;
  std::size_t abi_alignment_annotation_sites = 0;
  std::size_t foreign_type_annotation_sites = 0;
  std::size_t named_annotation_payload_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t bridge_callable_sites = 0;
  std::size_t async_executor_affinity_sites = 0;
  std::size_t actor_hazard_sites = 0;
  std::size_t interop_metadata_annotation_sites = 0;
  bool source_dependency_required = false;
  bool foreign_annotation_source_supported = false;
  bool ownership_interaction_profile_frozen = false;
  bool error_bridge_profile_reused = false;
  bool async_affinity_profile_reused = false;
  bool actor_hazard_profile_reused = false;
  bool metadata_payload_profile_frozen = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropInteropSemanticModelSummary(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.source_dependency_required &&
         summary.foreign_annotation_source_supported &&
         summary.ownership_interaction_profile_frozen &&
         summary.error_bridge_profile_reused &&
         summary.async_affinity_profile_reused &&
         summary.actor_hazard_profile_reused &&
         summary.metadata_payload_profile_frozen &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred && summary.deterministic &&
         summary.ready_for_semantic_expansion && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryDependencyContractId =
        kObjc3InteropInteropSemanticModelContractId;
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryContractId =
        "objc3c.interop.c.and.objc.runtime.parity.semantics.v1";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_c_and_objc_runtime_parity_semantics";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryRule =
        "declaration-only-foreign-c-surfaces-import-module-requires-foreign-and-implementation-annotation-rejections-are-now-live-fail-closed-sema-rules-while-ffi-abi-lowering-and-runnable-bridge-generation-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-cross-language-ownership-and-runnable-foreign-call-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropInteropRuntimeParitySummary {
  std::string contract_id = kObjc3InteropInteropRuntimeParitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropInteropRuntimeParitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropInteropRuntimeParitySummarySurfacePath;
  std::string semantic_model = kObjc3InteropInteropRuntimeParitySummaryRule;
  std::string deferred_model =
      kObjc3InteropInteropRuntimeParitySummaryDeferredRule;
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_method_foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t import_module_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t foreign_definition_rejection_sites = 0;
  std::size_t import_without_foreign_rejection_sites = 0;
  std::size_t implementation_annotation_rejection_sites = 0;
  bool dependency_required = false;
  bool declaration_only_foreign_c_enforced = false;
  bool import_module_requires_foreign_enforced = false;
  bool implementation_annotations_fail_closed = false;
  bool objc_runtime_parity_classified = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropInteropRuntimeParitySummary(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.declaration_only_foreign_c_enforced &&
         summary.import_module_requires_foreign_enforced &&
         summary.implementation_annotations_fail_closed &&
         summary.objc_runtime_parity_classified &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryDependencyContractId =
        kObjc3InteropInteropRuntimeParitySummaryContractId;
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryContractId =
        "objc3c.interop.cpp.ownership.throws.and.async.interaction.completion.v1";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_cpp_ownership_throws_and_async_interactions";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryRule =
        "cxx-facing-interop-callables-now-fail-closed-on-ownership-managed-throws-and-async-combinations-while-ffi-abi-lowering-runtime-bridge-gates-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-cross-language-ownership-async-propagation-and-runnable-cxx-interop-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropCppInteropInteractionSummary {
  std::string contract_id = kObjc3InteropCppInteropInteractionSummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropCppInteropInteractionSummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropCppInteropInteractionSummarySurfacePath;
  std::string semantic_model = kObjc3InteropCppInteropInteractionSummaryRule;
  std::string deferred_model =
      kObjc3InteropCppInteropInteractionSummaryDeferredRule;
  std::size_t cpp_interop_callable_sites = 0;
  std::size_t cpp_named_callable_sites = 0;
  std::size_t header_named_callable_sites = 0;
  std::size_t ownership_interaction_sites = 0;
  std::size_t throws_interaction_sites = 0;
  std::size_t async_interaction_sites = 0;
  std::size_t ownership_rejection_sites = 0;
  std::size_t throws_rejection_sites = 0;
  std::size_t async_rejection_sites = 0;
  bool dependency_required = false;
  bool cpp_annotation_profile_reused = false;
  bool ownership_interactions_fail_closed = false;
  bool throws_interactions_fail_closed = false;
  bool async_interactions_fail_closed = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropCppInteropInteractionSummary(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.cpp_annotation_profile_reused &&
         summary.ownership_interactions_fail_closed &&
         summary.throws_interactions_fail_closed &&
         summary.async_interactions_fail_closed &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryDependencyContractId =
        kObjc3InteropCppInteropInteractionSummaryContractId;
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryContractId =
        "objc3c.interop.swift.metadata.and.isolation.mapping.completion.v1";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_swift_metadata_and_isolation_mapping";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryRule =
        "swift-facing-metadata-now-fails-closed-on-missing-name-pairing-actor-owned-surfaces-and-objc-nonisolated-isolation-mapping-gaps-while-ffi-abi-lowering-runtime-bridge-gates-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-swift-facing-isolation-export-and-runnable-cross-language-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropSwiftInteropIsolationSummary {
  std::string contract_id = kObjc3InteropSwiftInteropIsolationSummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropSwiftInteropIsolationSummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropSwiftInteropIsolationSummarySurfacePath;
  std::string semantic_model = kObjc3InteropSwiftInteropIsolationSummaryRule;
  std::string deferred_model =
      kObjc3InteropSwiftInteropIsolationSummaryDeferredRule;
  std::size_t swift_interop_callable_sites = 0;
  std::size_t swift_named_callable_sites = 0;
  std::size_t swift_private_callable_sites = 0;
  std::size_t swift_private_without_name_sites = 0;
  std::size_t actor_owned_swift_callable_sites = 0;
  std::size_t nonisolated_swift_callable_sites = 0;
  std::size_t implementation_swift_callable_sites = 0;
  std::size_t swift_private_without_name_rejection_sites = 0;
  std::size_t actor_isolation_mapping_rejection_sites = 0;
  std::size_t nonisolated_mapping_rejection_sites = 0;
  std::size_t implementation_surface_rejection_sites = 0;
  bool dependency_required = false;
  bool swift_metadata_profile_reused = false;
  bool swift_private_requires_name_enforced = false;
  bool actor_isolation_mapping_fail_closed = false;
  bool nonisolated_mapping_fail_closed = false;
  bool implementation_surface_fail_closed = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropSwiftInteropIsolationSummary(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.swift_metadata_profile_reused &&
         summary.swift_private_requires_name_enforced &&
         summary.actor_isolation_mapping_fail_closed &&
         summary.nonisolated_mapping_fail_closed &&
         summary.implementation_surface_fail_closed &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

#include "sema/objc3_sema_contract_dispatch_intent.h"

inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelDependencyContractId =
        "objc3c.metaprogramming.property.behavior.source.completion.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelContractId =
        "objc3c.metaprogramming.expansion.behavior.semantic.model.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_expansion_and_behavior_semantic_model";
