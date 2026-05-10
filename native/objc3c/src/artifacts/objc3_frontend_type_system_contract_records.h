#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3Program;
struct Objc3RuntimeMetadataSourceRecordSet;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringContractId =
        "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringOptionalModel =
        "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringTypedKeypathModel =
        "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringAuthorityModel =
        "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringFailClosedModel =
        "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperContractId =
        "objc3c.type_system.optional.keypath.runtime.helper.contract.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_runtime_helper_contract";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        "optional-send-and-optional-member-access-sites-use-lowering-owned-nil-short-circuit-plus-public-runtime-selector-lookup-dispatch";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        "validated-single-component-typed-keypath-sites-publish-stable-descriptor-handles-and-retained-descriptor-sections-while-runtime-evaluation-helpers-remain-a-follow-on-private-runtime-step";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        "unsupported-typed-keypath-shapes-and-non-objc-optional-member-access-fail-closed-before-runtime";
inline constexpr const char
    *kObjc3FrontendTypeSystemRuntimeKeypathDescriptorLogicalSection =
        "objc3.runtime.keypath_descriptors";

inline constexpr const char
    *kObjc3FrontendTypeSystemTypeSemanticModelContractId =
        "objc3c.type_system.type.semantic.model.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemGenericContractPreservationContractId =
        "objc3c.type_system.generic.contract.preservation.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemNullabilityContractPreservationContractId =
        "objc3c.type_system.nullability.contract.preservation.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemProtocolContractPreservationContractId =
        "objc3c.type_system.protocol.contract.preservation.v1";
inline constexpr const char
    *kObjc3FrontendTypeSystemOptionalKeypathLoweringLaneContract =
        "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char
    *kObjc3FrontendLightweightGenericsConstraintLoweringLaneContract =
        "objc3c.lightweight.generics.constraint.lowering.v1";
inline constexpr const char
    *kObjc3FrontendNullabilityFlowWarningPrecisionLoweringLaneContract =
        "objc3c.nullability.flow.warning.precision.lowering.v1";
inline constexpr const char
    *kObjc3FrontendProtocolQualifiedObjectTypeLoweringLaneContract =
        "objc3c.protocol.qualified.object.type.lowering.v1";
inline constexpr const char
    *kObjc3FrontendVarianceBridgeCastLoweringLaneContract =
        "objc3c.variance.bridge.cast.lowering.v1";
inline constexpr const char
    *kObjc3FrontendGenericMetadataAbiLoweringLaneContract =
        "objc3c.generic.metadata.abi.lowering.v1";

struct Objc3FrontendTypeSystemSemanticModelRecord {
  std::string contract_id = kObjc3FrontendTypeSystemTypeSemanticModelContractId;
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t canonical_type_entries = 0;
  std::size_t canonical_object_type_entries = 0;
  std::size_t canonical_nullable_entries = 0;
  std::size_t canonical_nonnull_entries = 0;
  std::size_t canonical_implicitly_unwrapped_entries = 0;
  std::size_t canonical_null_resettable_entries = 0;
  std::size_t canonical_unspecified_nullability_entries = 0;
  std::size_t canonical_invalid_type_entries = 0;
  std::size_t optional_binding_contract_violation_sites = 0;
  std::size_t optional_send_contract_violation_sites = 0;
  std::size_t optional_flow_contract_violation_sites = 0;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
};

struct Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendLightweightGenericsConstraintLoweringContractRecord {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendVarianceBridgeCastLoweringContractRecord {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendGenericMetadataAbiLoweringContractRecord {
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

struct Objc3FrontendTypeSystemParitySurfaceRecord {
  std::size_t lightweight_generic_constraint_sites_total = 0;
  std::size_t lightweight_generic_constraint_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_object_pointer_type_sites_total = 0;
  std::size_t lightweight_generic_constraint_terminated_generic_suffix_sites_total = 0;
  std::size_t lightweight_generic_constraint_pointer_declarator_sites_total = 0;
  std::size_t lightweight_generic_constraint_normalized_sites_total = 0;
  std::size_t lightweight_generic_constraint_contract_violation_sites_total = 0;
  bool lightweight_generic_constraint_deterministic = true;
  bool deterministic_lightweight_generic_constraint_handoff = true;
  std::size_t nullability_flow_sites_total = 0;
  std::size_t nullability_flow_object_pointer_type_sites_total = 0;
  std::size_t nullability_flow_nullability_suffix_sites_total = 0;
  std::size_t nullability_flow_nullable_suffix_sites_total = 0;
  std::size_t nullability_flow_nonnull_suffix_sites_total = 0;
  std::size_t nullability_flow_normalized_sites_total = 0;
  std::size_t nullability_flow_contract_violation_sites_total = 0;
  bool nullability_flow_warning_precision_deterministic = true;
  bool deterministic_nullability_flow_warning_precision_handoff = true;
  std::size_t protocol_qualified_object_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_object_pointer_type_sites_total = 0;
  std::size_t protocol_qualified_object_type_terminated_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_pointer_declarator_sites_total = 0;
  std::size_t protocol_qualified_object_type_normalized_protocol_composition_sites_total = 0;
  std::size_t protocol_qualified_object_type_contract_violation_sites_total = 0;
  bool protocol_qualified_object_type_deterministic = true;
  bool deterministic_protocol_qualified_object_type_handoff = true;
  std::size_t variance_bridge_cast_sites_total = 0;
  std::size_t variance_bridge_cast_protocol_composition_sites_total = 0;
  std::size_t variance_bridge_cast_ownership_qualifier_sites_total = 0;
  std::size_t variance_bridge_cast_object_pointer_type_sites_total = 0;
  std::size_t variance_bridge_cast_pointer_declarator_sites_total = 0;
  std::size_t variance_bridge_cast_normalized_sites_total = 0;
  std::size_t variance_bridge_cast_contract_violation_sites_total = 0;
  bool variance_bridge_cast_deterministic = true;
  bool deterministic_variance_bridge_cast_handoff = true;
  std::size_t generic_metadata_abi_sites_total = 0;
  std::size_t generic_metadata_abi_generic_suffix_sites_total = 0;
  std::size_t generic_metadata_abi_protocol_composition_sites_total = 0;
  std::size_t generic_metadata_abi_ownership_qualifier_sites_total = 0;
  std::size_t generic_metadata_abi_object_pointer_type_sites_total = 0;
  std::size_t generic_metadata_abi_pointer_declarator_sites_total = 0;
  std::size_t generic_metadata_abi_normalized_sites_total = 0;
  std::size_t generic_metadata_abi_contract_violation_sites_total = 0;
  bool generic_metadata_abi_deterministic = true;
  bool deterministic_generic_metadata_abi_handoff = true;
};

struct Objc3FrontendTypeSystemCanonicalTypeRecord {
  std::vector<std::string> generic_arguments_source_order;
};

struct Objc3FrontendTypeSystemPropertyRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord canonical_type;
};

struct Objc3FrontendTypeSystemMethodRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord return_canonical_type;
  std::vector<Objc3FrontendTypeSystemCanonicalTypeRecord>
      param_canonical_types;
};

struct Objc3FrontendTypeSystemFunctionRecord {
  Objc3FrontendTypeSystemCanonicalTypeRecord return_canonical_type;
  std::vector<Objc3FrontendTypeSystemCanonicalTypeRecord>
      param_canonical_types;
};

struct Objc3FrontendTypeSystemInterfaceRecord {
  std::string name;
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::vector<Objc3FrontendTypeSystemPropertyRecord> properties_lexicographic;
  std::vector<Objc3FrontendTypeSystemMethodRecord> methods_lexicographic;
};

struct Objc3FrontendTypeSystemImplementationRecord {
  std::vector<Objc3FrontendTypeSystemPropertyRecord> properties_lexicographic;
  std::vector<Objc3FrontendTypeSystemMethodRecord> methods_lexicographic;
};

struct Objc3FrontendTypeSystemSemanticMetadataRecord {
  std::vector<Objc3FrontendTypeSystemFunctionRecord> functions_lexicographic;
  std::vector<Objc3FrontendTypeSystemInterfaceRecord> interfaces_lexicographic;
  std::vector<Objc3FrontendTypeSystemImplementationRecord>
      implementations_lexicographic;
  bool deterministic = true;
};

struct Objc3FrontendTypeSystemGenericContractInventory {
  std::size_t interface_count = 0;
  std::size_t generic_interface_count = 0;
  std::size_t generic_parameter_count = 0;
  std::size_t generic_variance_annotation_count = 0;
  std::size_t generic_argument_reference_count = 0;
  std::size_t protocol_qualified_generic_argument_count = 0;
};

struct Objc3FrontendTypeSystemProtocolContractInventory {
  std::size_t protocol_decl_count = 0;
  std::size_t protocol_forward_declaration_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t class_protocol_adoption_count = 0;
  std::size_t category_protocol_adoption_count = 0;
};

[[nodiscard]] bool IsValidFrontendTypeSystemOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract);
[[nodiscard]] std::string FrontendTypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendLightweightGenericsConstraintLoweringContract(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendLightweightGenericsConstraintLoweringReplayKey(
    const Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendNullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
        &contract);
[[nodiscard]] bool
IsValidFrontendProtocolQualifiedObjectTypeLoweringContract(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract);
[[nodiscard]] std::string
FrontendProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
        &contract);
[[nodiscard]] bool IsValidFrontendVarianceBridgeCastLoweringContract(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract);
[[nodiscard]] std::string FrontendVarianceBridgeCastLoweringReplayKey(
    const Objc3FrontendVarianceBridgeCastLoweringContractRecord &contract);
[[nodiscard]] bool IsValidFrontendGenericMetadataAbiLoweringContract(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract);
[[nodiscard]] std::string FrontendGenericMetadataAbiLoweringReplayKey(
    const Objc3FrontendGenericMetadataAbiLoweringContractRecord &contract);

}  // namespace objc3::artifacts::frontend
