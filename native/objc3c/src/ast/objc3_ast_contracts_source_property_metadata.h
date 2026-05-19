#pragma once

inline constexpr const char *kObjc3RuntimeMetadataSourceOwnershipContractId =
    "objc3c.runtime.metadata.source.ownership.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataCanonicalSourceSchema =
    "objc3-runtime-metadata-source-boundary-v1";
inline constexpr const char *kObjc3RuntimeMetadataClassAstAnchor =
    "Objc3InterfaceDecl/Objc3ImplementationDecl";
inline constexpr const char *kObjc3RuntimeMetadataProtocolAstAnchor =
    "Objc3ProtocolDecl";
inline constexpr const char *kObjc3RuntimeMetadataCategoryAstAnchor =
    "Objc3InterfaceDecl.has_category/Objc3ImplementationDecl.has_category";
inline constexpr const char *kObjc3RuntimeMetadataPropertyAstAnchor =
    "Objc3PropertyDecl";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceClosureContractId =
    "objc3c.executable.property.ivar.source.closure.v1";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceSurfaceModel =
    "property-ivar-executable-source-closure-freezes-decls-synthesis-bindings-and-accessor-selectors-before-storage-realization";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceModelCompletionContractId =
    "objc3c.executable.property.ivar.source.model.completion.v1";
inline constexpr const char *kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId =
    "objc3c.runtime.property.ivar.storage.accessor.source.surface.v1";
inline constexpr const char *kObjc3RuntimeStorageAccessorAbiSurfaceContractId =
    "objc3c.runtime.storage.accessor.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationSurfaceContractId =
        "objc3c.runtime.property.ivar.accessor.reflection.implementation.surface.v1";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationModel =
        "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-through-runtime-storage-owners";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationReflectionModel =
        "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationFailClosedModel =
        "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-synthetic-storage-path";
inline constexpr const char *kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceModel =
    "property-ivar-storage-accessor-runtime-source-surface-freezes-ast-sema-ir-pipeline-and-runtime-codepaths-before-lowering-or-runtime-semantic-expansion";
inline constexpr const char *kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceContractId =
    "objc3c.runtime.property.atomicity.synthesis.reflection.source.surface.v1";
inline constexpr const char *kObjc3RuntimePropertyAtomicitySynthesisReflectionSourceSurfaceModel =
    "property-atomicity-synthesis-reflection-source-surface-freezes-atomicity-flags-conflict-state-attribute-profiles-and-private-reflection-codepaths-before-runtime-managed-atomic-storage-semantics-land";
inline constexpr const char *kObjc3RuntimePropertyAtomicityReflectionBoundaryModel =
    "property-attribute-profiles-remain-the-authoritative-reflection-carrier-for-atomicity-and-synthesis-state-on-the-private-property-query-boundary";
inline constexpr const char *kObjc3ExecutablePropertyIvarLayoutModel =
    "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization";
inline constexpr const char *kObjc3ExecutablePropertyAttributeModel =
    "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles";
// accessor/layout lowering freeze anchor: AST remains the canonical
// source of property attribute/accessor profiles, synthesized-binding
// identities, and ivar layout identities. Lowering may only serialize
// this handoff into emitted metadata/object artifacts; it must not synthesize
// accessor bodies or invent runtime storage/layout beyond the sema-approved
// source model.
// ivar offset/layout emission anchor: AST-owned
// `Objc3PropertyDecl.executable_ivar_layout_symbol`,
// `.executable_ivar_layout_slot_index`, `.executable_ivar_layout_size_bytes`,
// and `.executable_ivar_layout_alignment_bytes` remain the only authoritative
// layout-shape inputs that lane-C may use when it materializes emitted offset
// globals or per-owner layout tables.
inline constexpr const char *kObjc3ExecutablePropertyIvarSemanticsContractId =
    "objc3c.executable.property.ivar.semantics.v1";
inline constexpr const char *kObjc3ExecutablePropertySynthesisSemanticsModel =
    "non-category-class-interface-properties-own-authoritative-default-ivar-and-synthesized-binding-identities-across-implementation-redeclaration-boundaries";
inline constexpr const char *kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel =
    "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration";
inline constexpr const char *kObjc3ExecutablePropertyAccessorSemanticsModel =
    "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission";
inline constexpr const char *kObjc3ExecutablePropertyAccessorSelectorUniquenessModel =
    "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding";
inline constexpr const char *kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel =
    "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land";
inline constexpr const char *kObjc3ExecutablePropertyStorageSemanticsModel =
    "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation";
inline constexpr const char *kObjc3ExecutablePropertyCompatibilitySemanticsModel =
    "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols";
inline constexpr const char *kObjc3RuntimeMetadataMethodAstAnchor =
    "Objc3MethodDecl";
inline constexpr const char *kObjc3RuntimeMetadataIvarAstAnchor =
    "Objc3PropertyDecl.ivar_binding_symbol";
inline constexpr const char *kObjc3RuntimeMetadataIvarSourceModel =
    "property-synthesis-ivar-binding-symbols";
inline constexpr const char *kObjc3RuntimeMetadataSectionAbiContractId =
    "objc3c.runtime.metadata.section.abi.symbol.policy.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataSectionPublicationContractId =
    "objc3c.runtime.metadata.section.publication.v1";
inline constexpr const char *kObjc3RuntimeStatePublicationSurfaceContractId =
    "objc3c.runtime.state.publication.surface.v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionContractId =
    "objc3c.runtime.metadata.object.inspection.harness.v1";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionContractId =
    "objc3c.executable.metadata.debug.projection.v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingContractId =
    "objc3c.executable.metadata.runtime.ingest.packaging.boundary.v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId =
    "objc3c.executable.metadata.runtime.ingest.binary.boundary.v1";
inline constexpr const char *kObjc3RuntimeSupportLibraryContractId =
    "objc3c.runtime.support.library.surface.build.contract.v1";
inline constexpr const char *kObjc3RuntimeSupportLibraryCoreFeatureContractId =
    "objc3c.runtime.support.library.core.feature.v1";
