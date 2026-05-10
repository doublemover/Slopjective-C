#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

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
