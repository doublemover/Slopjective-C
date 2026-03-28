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
inline constexpr const char *kObjc3RuntimeShimHostLinkDefaultDispatchSymbol =
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
    "live-compatibility-and-migration-selection-source-only-downgrade-unsupported-fail-closed";
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
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidCompatibilityModeCount = 2u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount = 3u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount = 4u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRunnableFeatureCount = 7u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount = 6u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedFeatureCount = 7u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount = 2u;
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
  std::size_t generic_erasure_semantic_sites = 0;
  std::size_t nullability_semantic_sites = 0;
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
    "ffi-abi-lowering-runtime-bridge-shims-cross-language-object-ownership-and-runnable-call-behavior-remain-deferred-to-later-runtime-lanes";

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
  std::size_t swift_name_annotation_sites = 0;
  std::size_t swift_private_annotation_sites = 0;
  std::size_t cpp_name_annotation_sites = 0;
  std::size_t header_name_annotation_sites = 0;
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
        "ffi-abi-lowering-runtime-bridge-shims-cross-language-ownership-and-runnable-foreign-call-behavior-remain-deferred-to-later-runtime-lanes";

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
        "cxx-facing-interop-callables-now-fail-closed-on-ownership-managed-throws-and-async-combinations-while-ffi-abi-lowering-runtime-bridge-shims-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-shims-cross-language-ownership-async-propagation-and-runnable-cxx-interop-behavior-remain-deferred-to-later-runtime-lanes";

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
        "swift-facing-metadata-now-fails-closed-on-missing-name-pairing-actor-owned-surfaces-and-objc-nonisolated-isolation-mapping-gaps-while-ffi-abi-lowering-runtime-bridge-shims-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-shims-swift-facing-isolation-export-and-runnable-cross-language-behavior-remain-deferred-to-later-runtime-lanes";

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

inline constexpr const char
    *kObjc3DispatchDispatchIntentSemanticModelDependencyContractId =
        "objc3c.dispatch.dispatch.intent.source.completion.v1";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelContractId =
    "objc3c.dispatch.dynamism.dispatch.control.semantic.model.v1";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_dispatch_dynamism_and_dispatch_control_semantic_model";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelRule =
    "direct-members-defaulting-final-sealed-and-override-accounting-now-share-one-truthful-sema-packet-while-direct-dispatch-legality-final-sealed-enforcement-and-runnable-dispatch-boundary-realization-remain-later-runtime-work";
inline constexpr const char
    *kObjc3DispatchDispatchIntentSemanticModelDeferredRule =
        "direct-call-lowering-final-sealed-diagnostics-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentSemanticModelSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3DispatchDispatchIntentSemanticModelDependencyContractId;
  std::string surface_path = kObjc3DispatchDispatchIntentSemanticModelSurfacePath;
  std::string semantic_model = kObjc3DispatchDispatchIntentSemanticModelRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentSemanticModelDeferredRule;
  std::size_t prefixed_container_attribute_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t effective_direct_member_sites = 0;
  std::size_t direct_members_defaulted_method_sites = 0;
  std::size_t direct_members_dynamic_opt_out_sites = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool source_dependency_required = false;
  bool dispatch_intent_source_supported = false;
  bool override_semantic_surface_reused = false;
  bool direct_dispatch_reserved_non_goal = false;
  bool final_sealed_enforcement_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_core_implementation = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentSemanticModelSummary(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.dispatch_intent_source_supported &&
         summary.override_semantic_surface_reused &&
         summary.direct_dispatch_reserved_non_goal &&
         summary.final_sealed_enforcement_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_core_implementation && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3DispatchDispatchIntentLegalitySummaryDependencyContractId =
        "objc3c.dispatch.dynamism.dispatch.control.semantic.model.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryContractId =
        "objc3c.dispatch.override.finality.sealing.legality.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_dispatch_override_finality_and_sealing_legality";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryRule =
        "superclass-finality-superclass-sealing-and-direct-final-override-restrictions-are-now-live-fail-closed-sema-rules-while-direct-call-lowering-and-runnable-dispatch-boundary-realization-remain-later-runtime-work";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryDeferredRule =
        "direct-call-lowering-selector-dispatch-bypass-metadata-realization-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentLegalitySummary {
  std::string contract_id = kObjc3DispatchDispatchIntentLegalitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3DispatchDispatchIntentLegalitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3DispatchDispatchIntentLegalitySummarySurfacePath;
  std::string semantic_model = kObjc3DispatchDispatchIntentLegalitySummaryRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentLegalitySummaryDeferredRule;
  std::size_t subclass_sites = 0;
  std::size_t override_sites = 0;
  std::size_t illegal_final_superclass_sites = 0;
  std::size_t illegal_sealed_superclass_sites = 0;
  std::size_t illegal_final_override_sites = 0;
  std::size_t illegal_direct_override_sites = 0;
  bool dependency_required = false;
  bool final_superclass_fail_closed = false;
  bool sealed_superclass_fail_closed = false;
  bool final_override_fail_closed = false;
  bool direct_override_fail_closed = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentLegalitySummary(
    const Objc3DispatchDispatchIntentLegalitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.final_superclass_fail_closed &&
         summary.sealed_superclass_fail_closed &&
         summary.final_override_fail_closed &&
         summary.direct_override_fail_closed &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3DispatchDispatchIntentCompatibilitySummaryDependencyContractId =
        "objc3c.dispatch.override.finality.sealing.legality.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryContractId =
        "objc3c.dispatch.dynamism.control.compatibility.diagnostics.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_dispatch_dynamism_control_compatibility_diagnostics";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryRule =
        "dispatch-dispatch-intent-now-fails-closed-on-conflicting-direct-dynamic-final-dynamic-callable-markers-plus-unsupported-function-protocol-and-category-topologies-before-lowering-and-runtime-dispatch-boundary-realization";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryDeferredRule =
        "direct-call-lowering-metadata-realization-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentCompatibilitySummary {
  std::string contract_id =
      kObjc3DispatchDispatchIntentCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3DispatchDispatchIntentCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3DispatchDispatchIntentCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3DispatchDispatchIntentCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentCompatibilitySummaryDeferredRule;
  std::size_t callable_dispatch_intent_sites = 0;
  std::size_t container_dispatch_intent_sites = 0;
  std::size_t illegal_direct_dynamic_conflict_sites = 0;
  std::size_t illegal_final_dynamic_conflict_sites = 0;
  std::size_t illegal_non_method_callable_sites = 0;
  std::size_t illegal_protocol_method_sites = 0;
  std::size_t illegal_category_method_sites = 0;
  std::size_t illegal_category_container_sites = 0;
  bool dependency_required = false;
  bool callable_conflict_fail_closed = false;
  bool unsupported_callable_topology_fail_closed = false;
  bool unsupported_container_topology_fail_closed = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentCompatibilitySummary(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.callable_conflict_fail_closed &&
         summary.unsupported_callable_topology_fail_closed &&
         summary.unsupported_container_topology_fail_closed &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelDependencyContractId =
        "objc3c.metaprogramming.property.behavior.source.completion.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelContractId =
        "objc3c.metaprogramming.expansion.behavior.semantic.model.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_expansion_and_behavior_semantic_model";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule =
        "derive-macro-package-provenance-and-property-behavior-source-surfaces-now-share-one-truthful-sema-packet-while-real-derive-expansion-macro-execution-and-property-behavior-runtime-materialization-remain-later-runtime-work";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule =
        "derive-body-expansion-macro-sandbox-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary {
  std::string contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule;
  std::string deferred_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule;
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  std::size_t property_behavior_contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool derive_macro_source_supported = false;
  bool macro_package_provenance_surface_reused = false;
  bool property_behavior_source_supported = false;
  bool synthesized_visibility_surface_reused = false;
  bool derive_synthesis_deferred = false;
  bool macro_execution_deferred = false;
  bool property_behavior_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_core_implementation = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.derive_macro_source_supported &&
         summary.macro_package_provenance_surface_reused &&
         summary.property_behavior_source_supported &&
         summary.synthesized_visibility_surface_reused &&
         summary.derive_synthesis_deferred &&
         summary.macro_execution_deferred &&
         summary.property_behavior_runtime_deferred && summary.deterministic &&
         summary.ready_for_core_implementation && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId =
        "objc3c.metaprogramming.expansion.behavior.semantic.model.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_expansion_inventory";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryRule =
        "supported-derive-requests-now-expand-into-a-deterministic-selector-inventory-while-unsupported-derive-names-category-topologies-and-selector-collisions-fail-closed-in-sema";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule =
        "runtime-backed-derived-method-body-materialization-macro-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingDeriveExpansionInventorySummary {
  std::string contract_id = kObjc3MetaprogrammingDeriveExpansionInventoryContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId;
  std::string surface_path = kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath;
  std::string semantic_model = kObjc3MetaprogrammingDeriveExpansionInventoryRule;
  std::string deferred_model = kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule;
  std::size_t derive_request_sites = 0;
  std::size_t supported_derive_request_sites = 0;
  std::size_t unsupported_derive_request_sites = 0;
  std::size_t unsupported_topology_sites = 0;
  std::size_t equatable_alias_sites = 0;
  std::size_t equality_derive_sites = 0;
  std::size_t hash_derive_sites = 0;
  std::size_t debug_description_derive_sites = 0;
  std::size_t selector_conflict_sites = 0;
  std::size_t generated_method_entry_count = 0;
  std::vector<std::string> expansion_inventory_rows_lexicographic;
  bool semantic_dependency_required = false;
  bool supported_derive_inventory_landed = false;
  bool unsupported_derive_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool selector_conflicts_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingDeriveExpansionInventorySummary(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_derive_inventory_landed &&
         summary.unsupported_derive_fail_closed &&
         summary.unsupported_topology_fail_closed &&
         summary.selector_conflicts_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule =
        "macro-callable-safety-sandbox-namespace-and-deterministic-provenance-semantics-are-live-in-sema-while-runnable-macro-execution-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule =
        "runnable-macro-execution-runtime-package-loading-and-expanded-body-materialization-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule;
  std::string deferred_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t safe_macro_callable_sites = 0;
  std::size_t incomplete_macro_metadata_sites = 0;
  std::size_t orphan_macro_metadata_sites = 0;
  std::size_t invalid_package_sites = 0;
  std::size_t invalid_provenance_sites = 0;
  std::size_t nondeterministic_callable_sites = 0;
  std::size_t unsupported_callable_topology_sites = 0;
  bool semantic_dependency_required = false;
  bool metadata_completeness_enforced = false;
  bool sandbox_namespace_enforced = false;
  bool provenance_determinism_enforced = false;
  bool callable_determinism_enforced = false;
  bool macro_execution_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.metadata_completeness_enforced &&
         summary.sandbox_namespace_enforced &&
         summary.provenance_determinism_enforced &&
         summary.callable_determinism_enforced &&
         summary.macro_execution_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId =
        "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_legality_and_interaction_completion";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule =
        "supported-property-behavior-names-and-owner-interaction-legality-now-fail-closed-in-sema-while-runtime-backed-behavior-materialization-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule =
        "runnable-property-behavior-hooks-observation-materialization-and-projection-runtime-support-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary {
  std::string contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule;
  std::string deferred_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule;
  std::size_t property_behavior_sites = 0;
  std::size_t supported_behavior_sites = 0;
  std::size_t unsupported_behavior_sites = 0;
  std::size_t observed_behavior_sites = 0;
  std::size_t projected_behavior_sites = 0;
  std::size_t observed_on_protocol_sites = 0;
  std::size_t observed_readonly_conflict_sites = 0;
  std::size_t projected_writable_conflict_sites = 0;
  std::size_t non_object_behavior_sites = 0;
  bool semantic_dependency_required = false;
  bool supported_behavior_inventory_landed = false;
  bool unsupported_behavior_fail_closed = false;
  bool owner_topology_fail_closed = false;
  bool interaction_legality_fail_closed = false;
  bool storage_legality_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_behavior_inventory_landed &&
         summary.unsupported_behavior_fail_closed &&
         summary.owner_topology_fail_closed &&
         summary.interaction_legality_fail_closed &&
         summary.storage_legality_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDependencyContractId =
        "objc3c.concurrency.task.executor.cancellation.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryContractId =
        "objc3c.concurrency.structured.task.cancellation.semantics.v1";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_structured_task_and_cancellation_semantics";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryRule =
        "structured-task-scope-task-hierarchy-and-cancellation-usage-semantics-are-live-in-sema-while-runnable-task-lowering-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDeferredRule =
        "task-allocation-executor-hop-task-group-runtime-and-scheduler-backed-cancellation-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyStructuredTaskCancellationSemanticSummary {
  std::string contract_id =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyStructuredTaskCancellationSemanticSummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t illegal_non_async_task_sites = 0;
  std::size_t illegal_task_group_scope_sites = 0;
  std::size_t illegal_task_hierarchy_sites = 0;
  std::size_t illegal_cancellation_usage_sites = 0;
  bool source_dependency_required = false;
  bool async_task_boundary_enforced = false;
  bool structured_task_scope_enforced = false;
  bool task_hierarchy_enforced = false;
  bool cancellation_usage_enforced = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyStructuredTaskCancellationSemanticSummary(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.async_task_boundary_enforced &&
         summary.structured_task_scope_enforced &&
         summary.task_hierarchy_enforced &&
         summary.cancellation_usage_enforced &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDependencyContractId =
        "objc3c.concurrency.structured.task.cancellation.semantics.v1";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryContractId =
        "objc3c.concurrency.executor.hop.affinity.compatibility.v1";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_executor_hop_and_affinity_compatibility_completion";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryRule =
        "executor-affinity-and-detached-task-hop-boundaries-are-live-in-sema-while-runnable-hop-lowering-and-scheduler-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDeferredRule =
        "executor-hop-lowering-task-spawn-runtime-and-scheduler-visible-execution-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary {
  std::string contract_id =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyExecutorHopAffinityCompatibilitySummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t detached_task_creation_sites = 0;
  std::size_t illegal_missing_executor_affinity_sites = 0;
  std::size_t illegal_main_executor_detached_sites = 0;
  bool dependency_required = false;
  bool executor_affinity_required_for_task_callables_enforced = false;
  bool detached_task_hop_boundary_enforced = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyExecutorHopAffinityCompatibilitySummary(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.executor_affinity_required_for_task_callables_enforced &&
         summary.detached_task_hop_boundary_enforced &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDependencyContractId =
    "objc3c.concurrency.async.effect.suspension.semantic.model.v1";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryContractId =
    "objc3c.concurrency.await.suspension.resume.semantics.v1";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_await_suspension_and_resume_semantics";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryRule =
    "await-placement-suspension-and-resume-semantics-are-live-in-sema-while-runnable-async-frame-lowering-and-executor-runtime-execution-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDeferredRule =
    "async-frame-layout-resume-lowering-suspension-cleanup-and-runtime-executor-scheduling-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary {
  std::string contract_id =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyAwaitSuspensionResumeSemanticSummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t await_in_async_callable_sites = 0;
  std::size_t illegal_await_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  bool source_dependency_required = false;
  bool await_placement_enforced = false;
  bool suspension_profile_enforced = false;
  bool resume_profile_enforced = false;
  bool non_async_await_fail_closed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAwaitSuspensionResumeSemanticSummary(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  return !summary.contract_id.empty() && !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.await_placement_enforced &&
         summary.suspension_profile_enforced &&
         summary.resume_profile_enforced &&
         summary.non_async_await_fail_closed &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryDependencyContractId =
    "objc3c.error_handling.error.semantic.model.v1";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryContractId =
    "objc3c.error_handling.try.throw.do.catch.semantics.v1";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_try_do_catch_semantics";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryRule =
    "try-throw-and-do-catch-parse-and-undergo-deterministic-legality-checking-in-source-only-native-validation-while-lowering-and-runtime-integration-remain-later-lane-work";
inline constexpr const char *kObjc3ErrorHandlingTryDoCatchSemanticSummaryDeferredRule =
    "native-ir-object-execution-lowering-catch-transfer-and-thrown-error-abi-remain-deferred-to-lanes-c-and-d";

struct Objc3ErrorHandlingTryDoCatchSemanticSummary {
  std::string contract_id = kObjc3ErrorHandlingTryDoCatchSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3ErrorHandlingTryDoCatchSemanticSummaryDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingTryDoCatchSemanticSummarySurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingTryDoCatchSemanticSummaryRule;
  std::string deferred_model = kObjc3ErrorHandlingTryDoCatchSemanticSummaryDeferredRule;
  std::size_t try_expression_sites = 0;
  std::size_t try_propagating_sites = 0;
  std::size_t try_optional_sites = 0;
  std::size_t try_forced_sites = 0;
  std::size_t throw_statement_sites = 0;
  std::size_t do_catch_sites = 0;
  std::size_t catch_clause_sites = 0;
  std::size_t catch_binding_sites = 0;
  std::size_t catch_all_sites = 0;
  std::size_t throwing_callable_try_sites = 0;
  std::size_t bridged_callable_try_sites = 0;
  std::size_t caller_propagation_sites = 0;
  std::size_t local_handler_sites = 0;
  std::size_t rethrow_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = true;
  bool try_surface_landed = false;
  bool throw_surface_landed = false;
  bool do_catch_surface_landed = false;
  bool throwing_context_legality_enforced = false;
  bool native_emit_remains_fail_closed = true;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = true;
  std::string replay_key;
  std::string failure_reason;
};

inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryDependencyContractId =
    "objc3c.error_handling.try.throw.do.catch.semantics.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryContractId =
    "objc3c.error_handling.error.bridge.legality.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_error_bridge_legality";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryRule =
    "nserror-and-status-bridge-markers-undergo-deterministic-semantic-legality-checking-before-lowering-and-only-semantically-valid-bridge-surfaces-qualify-for-try";
inline constexpr const char *kObjc3ErrorHandlingErrorBridgeLegalitySummaryDeferredRule =
    "status-to-error-execution-bridge-temporaries-native-error-abi-and-runnable-bridge-lowering-remain-deferred-to-lanes-c-and-d";

struct Objc3ErrorHandlingErrorBridgeLegalitySummary {
  std::string contract_id = kObjc3ErrorHandlingErrorBridgeLegalitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ErrorHandlingErrorBridgeLegalitySummaryDependencyContractId;
  std::string surface_path = kObjc3ErrorHandlingErrorBridgeLegalitySummarySurfacePath;
  std::string semantic_model = kObjc3ErrorHandlingErrorBridgeLegalitySummaryRule;
  std::string deferred_model = kObjc3ErrorHandlingErrorBridgeLegalitySummaryDeferredRule;
  std::size_t bridge_callable_sites = 0;
  std::size_t objc_nserror_callable_sites = 0;
  std::size_t objc_status_code_callable_sites = 0;
  std::size_t semantically_valid_bridge_callable_sites = 0;
  std::size_t try_eligible_bridge_callable_sites = 0;
  std::size_t missing_error_out_parameter_sites = 0;
  std::size_t invalid_nserror_return_sites = 0;
  std::size_t invalid_status_return_sites = 0;
  std::size_t invalid_error_type_sites = 0;
  std::size_t missing_mapping_symbol_sites = 0;
  std::size_t invalid_mapping_signature_sites = 0;
  std::size_t throws_bridge_conflict_sites = 0;
  std::size_t marker_conflict_sites = 0;
  std::size_t unsupported_combination_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = true;
  bool bridge_legality_landed = false;
  bool try_bridge_filter_landed = false;
  bool unsupported_combinations_fail_closed = false;
  bool native_emit_remains_fail_closed = true;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ModuleImportGraphSummary {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingSummary {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionSummary {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationSummary {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceSummary {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ThrowsPropagationSummary {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupSummary {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AsyncContinuationSummary {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateSummary {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorIsolationSendabilitySummary {
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId =
        kObjc3ActorMemberIsolationSourceClosureContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId =
        "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelRule =
        "actor-member-source-closure-and-parser-owned-actor-sendability-profiles-now-publish-one-deterministic-sema-packet-while-cross-actor-legality-sendable-enforcement-and-runnable-actor-runtime-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule =
        "dedicated-actor-isolation-diagnostics-cross-actor-sendable-enforcement-executor-scheduling-and-runnable-actor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendableSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool actor_member_source_supported = false;
  bool actor_isolation_sendability_profile_normalized = false;
  bool strict_concurrency_selection_fail_closed = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool cross_actor_enforcement_deferred = false;
  bool deterministic = false;
  bool ready_for_semantic_expansion = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary) {
  return summary.source_dependency_required &&
         summary.actor_member_source_supported &&
         summary.actor_isolation_sendability_profile_normalized &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred &&
         summary.cross_actor_enforcement_deferred &&
         summary.deterministic && summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId =
        "objc3c.concurrency.actor.isolation.sendability.enforcement.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule =
        "actor-method-semantics-now-fail-closed-for-non-actor-objc-nonisolated-usage-invalid-nonisolated-combinations-non-async-actor-hops-and-non-sendable-crossings-while-runnable-actor-mailbox-runtime-remains-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule =
        "full-cross-module-actor-runtime-mailboxes-race-hazard-closure-and-runnable-strict-concurrency-scheduling-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t total_nonisolated_method_sites = 0;
  std::size_t illegal_non_actor_nonisolated_sites = 0;
  std::size_t illegal_nonisolated_async_sites = 0;
  std::size_t illegal_nonisolated_executor_sites = 0;
  std::size_t illegal_actor_hop_without_async_sites = 0;
  std::size_t illegal_non_sendable_crossing_sites = 0;
  bool dependency_required = false;
  bool non_actor_nonisolated_fail_closed = false;
  bool nonisolated_combination_fail_closed = false;
  bool actor_hop_async_boundary_enforced = false;
  bool non_sendable_crossing_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendabilityEnforcementSummary(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary &summary) {
  return summary.dependency_required &&
         summary.non_actor_nonisolated_fail_closed &&
         summary.nonisolated_combination_fail_closed &&
         summary.actor_hop_async_boundary_enforced &&
         summary.non_sendable_crossing_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId =
        "objc3c.concurrency.actor.race.hazard.escape.diagnostics.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_race_hazard_and_escape_diagnostics";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule =
        "actor-method-task-handoff-now-fails-closed-without-race-guard-replay-proof-and-actor-isolation-coverage-while-escaping-block-literals-in-that-hazard-slice-remain-unsupported";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule =
        "runnable-actor-mailboxes-cross-module-isolation-runtime-and-full-strict-concurrency-escape-analysis-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule;
  std::size_t actor_method_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t escaping_block_literal_sites = 0;
  std::size_t illegal_missing_race_guard_sites = 0;
  std::size_t illegal_missing_replay_proof_sites = 0;
  std::size_t illegal_missing_actor_isolation_sites = 0;
  std::size_t illegal_escaping_block_literal_sites = 0;
  bool dependency_required = false;
  bool race_guard_fail_closed = false;
  bool replay_proof_fail_closed = false;
  bool actor_isolation_boundary_fail_closed = false;
  bool escaping_block_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary) {
  return summary.dependency_required && summary.race_guard_fail_closed &&
         summary.replay_proof_fail_closed &&
         summary.actor_isolation_boundary_fail_closed &&
         summary.escaping_block_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

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
  bool runtime_shim_host_link_required = true;
  bool runtime_shim_host_link_elided = false;
  std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_shim_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_shim_host_link_symbol;
  bool runtime_shim_host_link_is_normalized = false;
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

struct Objc3RuntimeShimHostLinkSummary {
  std::size_t message_send_sites = 0;
  std::size_t runtime_shim_required_sites = 0;
  std::size_t runtime_shim_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;
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

struct Objc3BlockLiteralCaptureSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool capture_set_deterministic = false;
  bool literal_is_normalized = false;
  bool has_count_mismatch = false;
  std::string capture_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockLiteralCaptureSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineSiteMetadata {
  std::size_t invoke_argument_slots = 0;
  std::size_t capture_word_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool has_invoke_trampoline = false;
  bool layout_is_normalized = false;
  bool has_count_mismatch = false;
  std::string layout_profile;
  std::string descriptor_symbol;
  std::string invoke_trampoline_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockAbiInvokeTrampolineSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool requires_byref_cells = false;
  bool escape_analysis_enabled = false;
  bool escape_to_heap = false;
  bool escape_profile_is_normalized = false;
  bool has_count_mismatch = false;
  std::string escape_profile;
  std::string byref_layout_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockStorageEscapeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  std::size_t owned_object_capture_count = 0;
  std::size_t weak_object_capture_count = 0;
  std::size_t unowned_object_capture_count = 0;
  std::size_t explicit_move_capture_count = 0;
  bool copy_helper_required = false;
  bool dispose_helper_required = false;
  bool copy_dispose_profile_is_normalized = false;
  bool ownership_helper_eligibility_is_normalized = false;
  bool has_count_mismatch = false;
  std::string copy_dispose_profile;
  std::string copy_helper_symbol;
  std::string dispose_helper_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockCopyDisposeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  std::size_t baseline_weight = 0;
  bool capture_set_deterministic = false;
  bool baseline_profile_is_normalized = false;
  std::string baseline_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockDeterminismPerfBaselineSummary {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeSiteMetadata {
  std::string scope_symbol;
  unsigned scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3AutoreleasePoolScopeSummary {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  unsigned max_scope_depth = 0;
  bool deterministic = true;
};

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3MethodInfo {
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool objc_direct_declared = false;
  bool objc_final_declared = false;
  bool objc_dynamic_declared = false;
  bool effective_direct_dispatch = false;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3PropertyInfo {
  ValueType type = ValueType::Unknown;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  unsigned line = 1;
  unsigned column = 1;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3InterfaceInfo {
  std::string super_name;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3ImplementationInfo {
  bool has_matching_interface = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3CategoryMergeInfo {
  std::vector<std::string> category_owner_identities_in_merge_order;
  std::unordered_map<std::string, Objc3PropertyInfo> merged_properties;
  std::unordered_map<std::string, std::string>
      merged_property_owner_identities;
  std::unordered_map<std::string, Objc3MethodInfo> merged_methods;
  std::unordered_map<std::string, std::string> merged_method_owner_identities;
  bool deterministic = true;
};

struct Objc3InterfaceImplementationSummary {
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3BootstrapLegalityFailureContractId =
    "objc3c.runtime.bootstrap.legality.duplicate.order.failure.contract.v1";
inline constexpr const char *kObjc3BootstrapLegalityFailureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract";
inline constexpr const char *kObjc3BootstrapLegalityImageOrderInvariantModel =
    kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
inline constexpr const char *kObjc3BootstrapLegalitySemanticsContractId =
    "objc3c.runtime.bootstrap.legality.duplicate.order.semantics.v1";
inline constexpr const char *kObjc3BootstrapLegalitySemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics";
inline constexpr const char *kObjc3BootstrapLegalityCrossImageLegalityModel =
    "translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality";
inline constexpr const char *kObjc3BootstrapLegalitySemanticDiagnosticModel =
    "fail-closed-bootstrap-legality-before-runtime-handoff";
inline constexpr const char *kObjc3BootstrapFailureRestartSemanticsContractId =
    "objc3c.runtime.bootstrap.failure.restart.semantics.v1";
inline constexpr const char *kObjc3BootstrapFailureRestartSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics";
inline constexpr const char *kObjc3BootstrapFailureRestartUnsupportedTopologyModel =
    "replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog";

struct Objc3BootstrapLegalityFailureContractSummary {
  std::string contract_id = kObjc3BootstrapLegalityFailureContractId;
  std::string surface_path = kObjc3BootstrapLegalityFailureSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string runtime_state_snapshot_symbol =
      kObjc3RuntimeBootstrapStateSnapshotSymbol;
  bool fail_closed = false;
  bool duplicate_registration_policy_frozen = false;
  bool image_order_invariant_frozen = false;
  bool bootstrap_rejection_frozen = false;
  bool restart_boundary_frozen = false;
  bool semantic_diagnostics_required = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapLegalityFailureContractSummary(
    const Objc3BootstrapLegalityFailureContractSummary &summary) {
  return !summary.contract_id.empty() && !summary.surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.runtime_state_snapshot_symbol.empty() &&
         summary.fail_closed &&
         summary.duplicate_registration_policy_frozen &&
         summary.image_order_invariant_frozen &&
         summary.bootstrap_rejection_frozen &&
         summary.restart_boundary_frozen &&
         summary.semantic_diagnostics_required &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId =
    "objc3c.concurrency.await.suspension.resume.semantics.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId =
    "objc3c.concurrency.async.diagnostics.compatibility.completion.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_diagnostics_and_compatibility_completion";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule =
    "async-topology-diagnostics-now-fail-closed-for-non-async-executor-affinity-async-function-prototypes-and-async-throws-while-runnable-frame-and-runtime-integration-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule =
    "async-prototype-import-surfaces-async-error-propagation-abi-and-runnable-executor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t illegal_non_async_executor_sites = 0;
  std::size_t illegal_async_function_prototype_sites = 0;
  std::size_t illegal_async_throws_sites = 0;
  std::size_t compatibility_diagnostic_sites = 0;
  std::size_t supported_async_callable_sites = 0;
  bool dependency_required = false;
  bool executor_affinity_requires_async_enforced = false;
  bool async_function_prototypes_fail_closed = false;
  bool async_throws_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.executor_affinity_requires_async_enforced &&
         summary.async_function_prototypes_fail_closed &&
         summary.async_throws_fail_closed &&
         summary.unsupported_topology_fail_closed && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3BootstrapLegalitySemanticsSummary {
  std::string contract_id = kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_legality_failure_contract_id =
      kObjc3BootstrapLegalityFailureContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string surface_path = kObjc3BootstrapLegalitySemanticsSurfacePath;
  std::string duplicate_registration_policy =
      kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy;
  std::string image_registration_order_invariant =
      kObjc3BootstrapLegalityImageOrderInvariantModel;
  std::string cross_image_legality_model =
      kObjc3BootstrapLegalityCrossImageLegalityModel;
  std::string semantic_diagnostic_model =
      kObjc3BootstrapLegalitySemanticDiagnosticModel;
  bool fail_closed = false;
  bool bootstrap_legality_failure_contract_ready = false;
  bool duplicate_registration_semantics_landed = false;
  bool image_order_semantics_landed = false;
  bool cross_image_legality_semantics_landed = false;
  bool semantic_diagnostics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string bootstrap_legality_failure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapLegalitySemanticsSummary(
    const Objc3BootstrapLegalitySemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_failure_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.surface_path.empty() &&
         !summary.duplicate_registration_policy.empty() &&
         !summary.image_registration_order_invariant.empty() &&
         !summary.cross_image_legality_model.empty() &&
         !summary.semantic_diagnostic_model.empty() && summary.fail_closed &&
         summary.bootstrap_legality_failure_contract_ready &&
         summary.duplicate_registration_semantics_landed &&
         summary.image_order_semantics_landed &&
         summary.cross_image_legality_semantics_landed &&
         summary.semantic_diagnostics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.bootstrap_legality_failure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3BootstrapFailureRestartSemanticsSummary {
  std::string contract_id = kObjc3BootstrapFailureRestartSemanticsContractId;
  std::string bootstrap_legality_semantics_contract_id =
      kObjc3BootstrapLegalitySemanticsContractId;
  std::string bootstrap_reset_contract_id =
      kObjc3RuntimeBootstrapResetContractId;
  std::string bootstrap_semantics_contract_id =
      kObjc3RuntimeBootstrapSemanticsContractId;
  std::string surface_path = kObjc3BootstrapFailureRestartSemanticsSurfacePath;
  std::string failure_mode = kObjc3RuntimeStartupBootstrapFailureMode;
  std::string restart_lifecycle_model =
      kObjc3RuntimeBootstrapResetLifecycleModel;
  std::string replay_order_model = kObjc3RuntimeBootstrapReplayOrderModel;
  std::string image_local_init_reset_model =
      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
  std::string catalog_retention_model =
      kObjc3RuntimeBootstrapCatalogRetentionModel;
  std::string unsupported_topology_model =
      kObjc3BootstrapFailureRestartUnsupportedTopologyModel;
  bool fail_closed = false;
  bool bootstrap_legality_semantics_ready = false;
  bool failure_mode_semantics_landed = false;
  bool restart_semantics_landed = false;
  bool replay_semantics_landed = false;
  bool unsupported_topology_semantics_landed = false;
  bool deterministic_recovery_semantics_landed = false;
  bool ready_for_lowering_and_runtime = false;
  std::string bootstrap_legality_semantics_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3BootstrapFailureRestartSemanticsSummary(
    const Objc3BootstrapFailureRestartSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.bootstrap_legality_semantics_contract_id.empty() &&
         !summary.bootstrap_reset_contract_id.empty() &&
         !summary.bootstrap_semantics_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.failure_mode.empty() &&
         !summary.restart_lifecycle_model.empty() &&
         !summary.replay_order_model.empty() &&
         !summary.image_local_init_reset_model.empty() &&
         !summary.catalog_retention_model.empty() &&
         !summary.unsupported_topology_model.empty() &&
         summary.fail_closed &&
         summary.bootstrap_legality_semantics_ready &&
         summary.failure_mode_semantics_landed &&
         summary.restart_semantics_landed &&
         summary.replay_semantics_landed &&
         summary.unsupported_topology_semantics_landed &&
         summary.deterministic_recovery_semantics_landed &&
         summary.ready_for_lowering_and_runtime &&
         !summary.bootstrap_legality_semantics_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3CompatibilityStrictnessClaimSemanticsSummary {
  std::string contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string surface_path =
      kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3CompatibilityStrictnessClaimSemanticModel;
  std::string downgrade_model =
      kObjc3CompatibilityStrictnessClaimDowngradeModel;
  std::string rejection_model =
      kObjc3CompatibilityStrictnessClaimRejectionModel;
  std::string canonical_interface_truth_model =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel;
  std::string separate_compilation_macro_truth_model =
      kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel;
  std::string canonical_interface_payload_mode =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode;
  std::string effective_compatibility_mode = "canonical";
  std::vector<std::string> suppressed_macro_claim_ids;
  bool migration_assist_enabled = false;
  std::size_t valid_compatibility_mode_count = 0;
  std::size_t live_selection_surface_count = 0;
  std::size_t valid_selection_combination_count = 0;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t downgraded_source_only_claim_count = 0;
  std::size_t rejected_unsupported_feature_claim_count = 0;
  std::size_t rejected_selection_surface_count = 0;
  std::size_t suppressed_macro_claim_count = 0;
  // unsupported-feature enforcement anchor: accepted
  // advanced source surfaces must either keep these counters at zero on the
  // runnable path or fail closed before lowering/runtime handoff.
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
  bool fail_closed = false;
  bool compatibility_mode_semantics_landed = false;
  bool migration_assist_semantics_landed = false;
  bool source_only_claim_downgrade_semantics_landed = false;
  bool unsupported_feature_claim_rejection_semantics_landed = false;
  bool live_unsupported_feature_source_rejection_landed = false;
  bool strictness_selection_rejection_semantics_landed = false;
  bool feature_macro_claim_suppression_semantics_landed = false;
  bool canonical_interface_truth_semantics_landed = false;
  bool separate_compilation_macro_truth_semantics_landed = false;
  bool selected_configuration_valid = false;
  bool selected_configuration_downgraded = false;
  bool selected_configuration_rejected = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &summary) {
  const bool compatibility_mode_valid =
      summary.effective_compatibility_mode == "canonical" ||
      summary.effective_compatibility_mode == "legacy";
  return !summary.contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.downgrade_model.empty() && !summary.rejection_model.empty() &&
         !summary.canonical_interface_truth_model.empty() &&
         !summary.separate_compilation_macro_truth_model.empty() &&
         summary.canonical_interface_payload_mode ==
             kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode &&
         compatibility_mode_valid && summary.fail_closed &&
         summary.suppressed_macro_claim_ids.size() ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.suppressed_macro_claim_ids[0] ==
             kObjc3SuppressedMacroClaimStrictnessLevel &&
         summary.suppressed_macro_claim_ids[1] ==
             kObjc3SuppressedMacroClaimConcurrencyMode &&
         summary.suppressed_macro_claim_ids[2] ==
             kObjc3SuppressedMacroClaimConcurrencyStrict &&
         summary.valid_compatibility_mode_count ==
             kObjc3CompatibilityStrictnessClaimValidCompatibilityModeCount &&
         summary.live_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount &&
         summary.valid_selection_combination_count ==
             kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount &&
         summary.runnable_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRunnableFeatureCount &&
         summary.downgraded_source_only_claim_count ==
             kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount &&
         summary.rejected_unsupported_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRejectedFeatureCount &&
         summary.live_unsupported_feature_family_count <=
             summary.rejected_unsupported_feature_claim_count &&
         summary.throws_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.blocks_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.arc_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.throws_source_rejection_site_count +
                 summary.blocks_source_rejection_site_count +
                 summary.arc_source_rejection_site_count ==
             summary.live_unsupported_feature_site_count &&
         summary.rejected_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount &&
         summary.suppressed_macro_claim_count ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.compatibility_mode_semantics_landed &&
         summary.migration_assist_semantics_landed &&
         summary.source_only_claim_downgrade_semantics_landed &&
         summary.unsupported_feature_claim_rejection_semantics_landed &&
         summary.live_unsupported_feature_source_rejection_landed &&
         summary.strictness_selection_rejection_semantics_landed &&
         summary.feature_macro_claim_suppression_semantics_landed &&
         summary.canonical_interface_truth_semantics_landed &&
         summary.separate_compilation_macro_truth_semantics_landed &&
         summary.selected_configuration_valid &&
         !summary.selected_configuration_downgraded &&
         !summary.selected_configuration_rejected &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  // diagnostic precision anchor: these maps model class containers
  // only. Category containers are validated separately so valid
  // class-plus-category programs do not collapse into duplicate class-owner
  // diagnostics before runtime metadata conflict analysis runs.
  // binary-boundary anchor: the executable metadata binary envelope
  // must consume the canonical sema-owned class/category split without
  // rebuilding container ownership from manifest-only heuristics.
  std::unordered_map<std::string, Objc3InterfaceInfo> interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo> implementations;
  std::unordered_map<std::string, Objc3InterfaceInfo> category_interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo>
      category_implementations;
  std::unordered_map<std::string, Objc3CategoryMergeInfo>
      category_merge_surfaces;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3BootstrapLegalityFailureContractSummary
      bootstrap_legality_failure_contract_summary;
  Objc3BootstrapLegalitySemanticsSummary
      bootstrap_legality_semantics_summary;
  Objc3BootstrapFailureRestartSemanticsSummary
      bootstrap_failure_restart_semantics_summary;
  Objc3CompatibilityStrictnessClaimSemanticsSummary
      compatibility_strictness_claim_semantics_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  bool built = false;
};

struct Objc3SemanticFunctionTypeMetadata {
  std::string name;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3SemanticMethodTypeMetadata {
  std::string selector;
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3SemanticPropertyTypeMetadata {
  std::string name;
  ValueType type = ValueType::Unknown;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3SemanticInterfaceTypeMetadata {
  std::string name;
  std::string super_name;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

struct Objc3SemanticImplementationTypeMetadata {
  std::string name;
  bool has_matching_interface = false;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

// lowering-handoff anchor: this typed metadata handoff is the
// canonical sema-to-lowering schema input for executable metadata graph
// lowering freeze packets and must remain deterministic and replayable.
// typed-lowering anchor: the concrete lowering-ready metadata graph
// packet consumes this same deterministic sema metadata surface so the typed
// handoff stays schema-stable between manifest publication and later lowering.
// debug-projection anchor: the manifest/IR inspection matrix replays
// this same typed sema surface so operators inspect one deterministic schema
// before runtime section emission lands.
// runtime-ingest packaging anchor: the manifest packaging boundary
// must carry this same typed sema surface forward verbatim, so runtime ingest
// packaging never invents a second schema between lane-C publication and
// later section emission/startup registration.
struct Objc3SemanticTypeMetadataHandoff {
  std::vector<std::string> global_names_lexicographic;
  std::vector<Objc3SemanticFunctionTypeMetadata> functions_lexicographic;
  std::vector<Objc3SemanticInterfaceTypeMetadata> interfaces_lexicographic;
  std::vector<Objc3SemanticImplementationTypeMetadata> implementations_lexicographic;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
};

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
  bool allow_source_only_block_literals = false;
  bool allow_source_only_defer_statements = false;
  bool allow_source_only_error_runtime_surface = false;
  bool arc_mode_enabled = false;
};

bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);
Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
