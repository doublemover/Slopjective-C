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
    "objc3c-compatibility-strictness-claim-semantics/m264-b001-v1";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics";
// M259-B001 runnable-core compatibility guard anchor: sema owns the current
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
  // M252-A002 completeness anchor: these deterministic counts validate the
  // first-class executable metadata graph packet.
  // M252-A003 completion anchor: protocol/category composition counts stay
  // stable so protocol/category/property/ivar export graph closure can fail
  // closed on deterministic sema inputs.
  // M252-B001 freeze anchor: these counts remain the canonical semantic
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

struct Objc3Part3TypeSemanticModelSummary {
  std::string contract_id = "objc3c-part3-type-semantic-model/m265-b001-v1";
  std::string surface_path =
      "frontend.pipeline.semantic_surface.objc_part3_type_semantic_model";
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
    *kObjc3Part5ControlFlowSemanticModelFrontendDependencyContractId =
        "objc3c-part5-control-flow-source-closure/m266-a002-v1";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelContractId =
    "objc3c-part5-control-flow-semantic-model/m266-b001-v1";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelRule =
    "guard-refinement-plus-statement-match-exhaustiveness-and-defer-legality-semantics-are-live-while-defer-cleanup-lowering-remains-a-later-lane-c-runtime-step";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelDeferRule =
    "defer-statement-lifo-cleanup-order-and-defer-mediated-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred-to-later-m266-lowering-and-runtime-work";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelMatchRule =
    "statement-match-enforces-catch-all-bool-and-result-case-exhaustiveness-with-case-local-binding-scopes-while-result-payload-typing-remains-deferred";
inline constexpr const char *kObjc3Part5ControlFlowSemanticModelExitRule =
    "break-and-continue-restrictions-plus-defer-body-nonlocal-exit-legality-are-live-in-sema-while-runtime-cleanup-execution-remains-deferred";

struct Objc3Part5ControlFlowSemanticModelSummary {
  std::string contract_id = kObjc3Part5ControlFlowSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3Part5ControlFlowSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3Part5ControlFlowSemanticModelSurfacePath;
  std::string semantic_model = kObjc3Part5ControlFlowSemanticModelRule;
  std::string defer_model = kObjc3Part5ControlFlowSemanticModelDeferRule;
  std::string match_model = kObjc3Part5ControlFlowSemanticModelMatchRule;
  std::string non_local_exit_model = kObjc3Part5ControlFlowSemanticModelExitRule;
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

inline bool IsReadyObjc3Part5ControlFlowSemanticModelSummary(
    const Objc3Part5ControlFlowSemanticModelSummary &summary) {
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

inline constexpr const char *kObjc3Part6ErrorSemanticModelFrontendDependencyContractId =
    "objc3c-part6-error-source-closure/m267-a001-v1";
inline constexpr const char *kObjc3Part6ErrorSemanticModelContractId =
    "objc3c-part6-error-semantic-model/m267-b001-v1";
inline constexpr const char *kObjc3Part6ErrorSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part6_error_semantic_model";
inline constexpr const char *kObjc3Part6ErrorSemanticModelRule =
    "throws-declaration-semantics-plus-deterministic-result-and-nserror-profile-carriage-are-live-while-try-throw-do-catch-propagation-and-native-error-runtime-behavior-remain-deferred";
inline constexpr const char *kObjc3Part6ErrorSemanticModelDeferredRule =
    "try-throw-do-catch-postfix-propagation-status-to-error-execution-bridge-temporaries-and-native-thrown-error-abi-remain-fail-closed-or-later-lane-work";

struct Objc3Part6ErrorSemanticModelSummary {
  std::string contract_id = kObjc3Part6ErrorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3Part6ErrorSemanticModelFrontendDependencyContractId;
  std::string surface_path = kObjc3Part6ErrorSemanticModelSurfacePath;
  std::string semantic_model = kObjc3Part6ErrorSemanticModelRule;
  std::string deferred_model = kObjc3Part6ErrorSemanticModelDeferredRule;
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

inline constexpr const char *kObjc3Part7AsyncEffectSuspensionSemanticModelFrontendDependencyContractId =
    "objc3c-part7-async-source-closure/m268-a002-v1";
inline constexpr const char *kObjc3Part7AsyncEffectSuspensionSemanticModelContractId =
    "objc3c-part7-async-effect-suspension-semantic-model/m268-b001-v1";
inline constexpr const char *kObjc3Part7AsyncEffectSuspensionSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model";
inline constexpr const char *kObjc3Part7AsyncEffectSuspensionSemanticModelRule =
    "async-effect-and-await-legality-semantics-plus-deterministic-continuation-suspension-and-concurrency-profile-carriage-are-live-while-runnable-frame-lowering-cleanup-and-executor-runtime-integration-remain-later-m268-work";
inline constexpr const char *kObjc3Part7AsyncEffectSuspensionSemanticModelDeferredRule =
    "async-frame-abi-resume-lowering-suspension-cleanup-task-runtime-execution-and-executor-dispatch-remain-deferred-to-later-m268-lanes";

struct Objc3Part7AsyncEffectSuspensionSemanticModelSummary {
  std::string contract_id =
      kObjc3Part7AsyncEffectSuspensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3Part7AsyncEffectSuspensionSemanticModelFrontendDependencyContractId;
  std::string surface_path =
      kObjc3Part7AsyncEffectSuspensionSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3Part7AsyncEffectSuspensionSemanticModelRule;
  std::string deferred_model =
      kObjc3Part7AsyncEffectSuspensionSemanticModelDeferredRule;
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

inline bool IsReadyObjc3Part7AsyncEffectSuspensionSemanticModelSummary(
    const Objc3Part7AsyncEffectSuspensionSemanticModelSummary &summary) {
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

inline constexpr const char *kObjc3Part7AwaitSuspensionResumeSemanticSummaryDependencyContractId =
    "objc3c-part7-async-effect-suspension-semantic-model/m268-b001-v1";
inline constexpr const char *kObjc3Part7AwaitSuspensionResumeSemanticSummaryContractId =
    "objc3c-part7-await-suspension-resume-semantics/m268-b002-v1";
inline constexpr const char *kObjc3Part7AwaitSuspensionResumeSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics";
inline constexpr const char *kObjc3Part7AwaitSuspensionResumeSemanticSummaryRule =
    "await-placement-suspension-and-resume-semantics-are-live-in-sema-while-runnable-async-frame-lowering-and-executor-runtime-execution-remain-later-m268-work";
inline constexpr const char *kObjc3Part7AwaitSuspensionResumeSemanticSummaryDeferredRule =
    "async-frame-layout-resume-lowering-suspension-cleanup-and-runtime-executor-scheduling-remain-deferred-to-later-m268-lanes";

struct Objc3Part7AwaitSuspensionResumeSemanticSummary {
  std::string contract_id =
      kObjc3Part7AwaitSuspensionResumeSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3Part7AwaitSuspensionResumeSemanticSummaryDependencyContractId;
  std::string surface_path =
      kObjc3Part7AwaitSuspensionResumeSemanticSummarySurfacePath;
  std::string semantic_model =
      kObjc3Part7AwaitSuspensionResumeSemanticSummaryRule;
  std::string deferred_model =
      kObjc3Part7AwaitSuspensionResumeSemanticSummaryDeferredRule;
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

inline bool IsReadyObjc3Part7AwaitSuspensionResumeSemanticSummary(
    const Objc3Part7AwaitSuspensionResumeSemanticSummary &summary) {
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

inline constexpr const char *kObjc3Part6TryDoCatchSemanticSummaryDependencyContractId =
    "objc3c-part6-error-semantic-model/m267-b001-v1";
inline constexpr const char *kObjc3Part6TryDoCatchSemanticSummaryContractId =
    "objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1";
inline constexpr const char *kObjc3Part6TryDoCatchSemanticSummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics";
inline constexpr const char *kObjc3Part6TryDoCatchSemanticSummaryRule =
    "try-throw-and-do-catch-parse-and-undergo-deterministic-legality-checking-in-source-only-native-validation-while-lowering-and-runtime-integration-remain-later-lane-work";
inline constexpr const char *kObjc3Part6TryDoCatchSemanticSummaryDeferredRule =
    "native-ir-object-execution-lowering-catch-transfer-and-thrown-error-abi-remain-deferred-to-lanes-c-and-d";

struct Objc3Part6TryDoCatchSemanticSummary {
  std::string contract_id = kObjc3Part6TryDoCatchSemanticSummaryContractId;
  std::string dependency_contract_id =
      kObjc3Part6TryDoCatchSemanticSummaryDependencyContractId;
  std::string surface_path = kObjc3Part6TryDoCatchSemanticSummarySurfacePath;
  std::string semantic_model = kObjc3Part6TryDoCatchSemanticSummaryRule;
  std::string deferred_model = kObjc3Part6TryDoCatchSemanticSummaryDeferredRule;
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

inline constexpr const char *kObjc3Part6ErrorBridgeLegalitySummaryDependencyContractId =
    "objc3c-part6-try-throw-do-catch-semantics/m267-b002-v1";
inline constexpr const char *kObjc3Part6ErrorBridgeLegalitySummaryContractId =
    "objc3c-part6-error-bridge-legality/m267-b003-v1";
inline constexpr const char *kObjc3Part6ErrorBridgeLegalitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality";
inline constexpr const char *kObjc3Part6ErrorBridgeLegalitySummaryRule =
    "nserror-and-status-bridge-markers-undergo-deterministic-semantic-legality-checking-before-lowering-and-only-semantically-valid-bridge-surfaces-qualify-for-try";
inline constexpr const char *kObjc3Part6ErrorBridgeLegalitySummaryDeferredRule =
    "status-to-error-execution-bridge-temporaries-native-error-abi-and-runnable-bridge-lowering-remain-deferred-to-lanes-c-and-d";

struct Objc3Part6ErrorBridgeLegalitySummary {
  std::string contract_id = kObjc3Part6ErrorBridgeLegalitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3Part6ErrorBridgeLegalitySummaryDependencyContractId;
  std::string surface_path = kObjc3Part6ErrorBridgeLegalitySummarySurfacePath;
  std::string semantic_model = kObjc3Part6ErrorBridgeLegalitySummaryRule;
  std::string deferred_model = kObjc3Part6ErrorBridgeLegalitySummaryDeferredRule;
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
  // M252-B002 anchor: lane-B executable metadata semantic validation consumes
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
  // M252-B004 export-legality anchor: these counts are the canonical sema
  // preconditions for property/ivar runtime export and must not degrade into
  // generic property-declaration totals.
  // M252-E001 semantic-closure gate anchor: lane-E consumes this property/ivar
  // legality summary together with the A003 graph, C003 projection, and D002
  // packaging proofs before M253-A001 section emission begins.
  // M252-E002 corpus-sync anchor: representative legality corpus cases keep
  // these counts deterministic so docs and integrated gate coverage stay
  // aligned on the real runner path.
  // M257-B002 default-binding semantics anchor: matched class implementations
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
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

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
    "objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1";
inline constexpr const char *kObjc3BootstrapLegalityFailureSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract";
inline constexpr const char *kObjc3BootstrapLegalityImageOrderInvariantModel =
    kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel;
inline constexpr const char *kObjc3BootstrapLegalitySemanticsContractId =
    "objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1";
inline constexpr const char *kObjc3BootstrapLegalitySemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics";
inline constexpr const char *kObjc3BootstrapLegalityCrossImageLegalityModel =
    "translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality";
inline constexpr const char *kObjc3BootstrapLegalitySemanticDiagnosticModel =
    "fail-closed-bootstrap-legality-before-runtime-handoff";
inline constexpr const char *kObjc3BootstrapFailureRestartSemanticsContractId =
    "objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1";
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

inline constexpr const char *kObjc3Part7AsyncDiagnosticsCompatibilitySummaryDependencyContractId =
    "objc3c-part7-await-suspension-resume-semantics/m268-b002-v1";
inline constexpr const char *kObjc3Part7AsyncDiagnosticsCompatibilitySummaryContractId =
    "objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1";
inline constexpr const char *kObjc3Part7AsyncDiagnosticsCompatibilitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion";
inline constexpr const char *kObjc3Part7AsyncDiagnosticsCompatibilitySummaryRule =
    "async-topology-diagnostics-now-fail-closed-for-non-async-executor-affinity-async-function-prototypes-and-async-throws-while-runnable-frame-and-runtime-integration-remain-later-m268-work";
inline constexpr const char *kObjc3Part7AsyncDiagnosticsCompatibilitySummaryDeferredRule =
    "async-prototype-import-surfaces-async-error-propagation-abi-and-runnable-executor-runtime-behavior-remain-deferred-to-later-m268-lanes";

struct Objc3Part7AsyncDiagnosticsCompatibilitySummary {
  std::string contract_id =
      kObjc3Part7AsyncDiagnosticsCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3Part7AsyncDiagnosticsCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3Part7AsyncDiagnosticsCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3Part7AsyncDiagnosticsCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3Part7AsyncDiagnosticsCompatibilitySummaryDeferredRule;
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

inline bool IsReadyObjc3Part7AsyncDiagnosticsCompatibilitySummary(
    const Objc3Part7AsyncDiagnosticsCompatibilitySummary &summary) {
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
  // M259-B002/M264-B002 unsupported-feature enforcement anchor: accepted
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
  // M252-B003 diagnostic precision anchor: these maps model class containers
  // only. Category containers are validated separately so valid
  // class-plus-category programs do not collapse into duplicate class-owner
  // diagnostics before runtime metadata conflict analysis runs.
  // M252-D002 binary-boundary anchor: the executable metadata binary envelope
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

// M252-C001 lowering-handoff anchor: this typed metadata handoff is the
// canonical sema-to-lowering schema input for executable metadata graph
// lowering freeze packets and must remain deterministic and replayable.
// M252-C002 typed-lowering anchor: the concrete lowering-ready metadata graph
// packet consumes this same deterministic sema metadata surface so the typed
// handoff stays schema-stable between manifest publication and later lowering.
// M252-C003 debug-projection anchor: the manifest/IR inspection matrix replays
// this same typed sema surface so operators inspect one deterministic schema
// before runtime section emission lands.
// M252-D001 runtime-ingest packaging anchor: the manifest packaging boundary
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
