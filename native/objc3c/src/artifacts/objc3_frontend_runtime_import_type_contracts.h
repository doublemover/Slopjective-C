#pragma once

#include <cstddef>

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kObjc3ArtifactDispatchDispatchControlLoweringContractId =
        "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationContractId =
        "objc3c.dispatch.dispatch.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationImportArtifactMemberName =
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationSourceModel =
        "runtime-metadata-source-records-and-runtime-import-surface-artifacts-preserve-direct-final-sealed-intent-for-separate-compilation-and-interface-replay";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationModel =
        "provider-and-consumer-runtime-import-surface-artifacts-preserve-direct-final-sealed-dispatch-intent-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3ArtifactDispatchDispatchMetadataInterfacePreservationFailClosedModel =
        "missing-or-drifted-dispatch-intent-preservation-packets-disable-cross-module-dispatch-preservation-claims";

inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringContractId =
        "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringOptionalModel =
        "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringTypedKeypathModel =
        "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringAuthorityModel =
        "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathLoweringFailClosedModel =
        "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathRuntimeHelperContractId =
        "objc3c.type_system.optional.keypath.runtime.helper.contract.v1";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_runtime_helper_contract";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        "optional-send-and-optional-member-access-sites-use-lowering-owned-nil-short-circuit-plus-public-runtime-selector-lookup-dispatch";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        "validated-single-component-typed-keypath-sites-publish-stable-descriptor-handles-and-retained-descriptor-sections-while-runtime-evaluation-helpers-remain-a-follow-on-private-runtime-step";
inline constexpr const char
    *kObjc3ArtifactTypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        "unsupported-typed-keypath-shapes-and-non-objc-optional-member-access-fail-closed-before-runtime";
inline constexpr const char
    *kObjc3ArtifactRuntimeKeypathDescriptorLogicalSection =
        "objc3.runtime.keypath_descriptors";

inline constexpr const char *kObjc3ArtifactTypeSystemTypeSemanticModelContractId =
    "objc3c.type_system.type.semantic.model.v1";
inline constexpr const char
    *kObjc3ArtifactTypeSystemGenericContractPreservationContractId =
        "objc3c.type_system.generic.contract.preservation.v1";
inline constexpr const char
    *kObjc3ArtifactTypeSystemNullabilityContractPreservationContractId =
        "objc3c.type_system.nullability.contract.preservation.v1";
inline constexpr const char
    *kObjc3ArtifactTypeSystemProtocolContractPreservationContractId =
        "objc3c.type_system.protocol.contract.preservation.v1";

struct Objc3ArtifactTypeSystemGenericContractInventory {
  std::size_t interface_count = 0;
  std::size_t generic_interface_count = 0;
  std::size_t generic_parameter_count = 0;
  std::size_t generic_variance_annotation_count = 0;
  std::size_t generic_argument_reference_count = 0;
  std::size_t protocol_qualified_generic_argument_count = 0;
};

struct Objc3ArtifactTypeSystemProtocolContractInventory {
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
