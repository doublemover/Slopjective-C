#pragma once

#include <cstddef>

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

}  // namespace objc3::artifacts::frontend
