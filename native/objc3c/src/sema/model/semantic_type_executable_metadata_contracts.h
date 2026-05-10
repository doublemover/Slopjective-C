#pragma once

inline constexpr const char *kObjc3ExecutableMetadataSourceGraphContractId =
    "objc3c.executable.metadata.source.graph.completeness.v1";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel =
    "semantic-link-symbol-and-runtime-owner-identity";
inline constexpr const char *kObjc3ExecutableMetadataMetaclassNodePolicy =
    "first-class-metaclass-nodes-derived-from-interface-runtime-owner-identities";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel =
    "lexicographic-kind-source-target";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId =
        "objc3c.executable.class.metaclass.source.closure.v1";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassParentIdentityModel =
        "declaration-owned-class-parent-plus-metaclass-parent-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel =
        "declaration-owned-instance-class-method-owner-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel =
        "declaration-owned-class-and-metaclass-object-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId =
        "objc3c.executable.protocol.category.source.closure.v1";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolInheritanceIdentityModel =
        "protocol-declaration-owned-inherited-protocol-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataCategoryAttachmentIdentityModel =
        "category-declaration-owned-class-interface-implementation-attachment-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel =
        "category-declaration-owned-adopted-protocol-conformance-identities";
inline constexpr const char *kObjc3ExecutableMetadataSemanticConsistencyContractId =
    "objc3c.executable.metadata.semantic.consistency.freeze.v1";
inline constexpr const char *kObjc3ExecutableMetadataSemanticValidationContractId =
    "objc3c.executable.metadata.semantic.validation.v1";
inline constexpr const char *kObjc3ExecutableMetadataLoweringHandoffContractId =
    "objc3c.executable.metadata.lowering.handoff.freeze.v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringHandoffContractId =
    "objc3c.executable.metadata.typed.lowering.handoff.v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel =
    "contract-header-then-source-graph-payload-v1";
