#pragma once

#include <cstdint>

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
        "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationReflectionModel =
        "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts";
inline constexpr const char
    *kObjc3RuntimePropertyIvarAccessorReflectionImplementationFailClosedModel =
        "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-retired-route-synthesis";
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
// emitted metadata inventory freeze anchor: these constants define
// the canonical logical sections, symbol families, linkage, visibility,
// retention root, and inspection commands for the currently supported runtime
// metadata inventory.
inline constexpr const char *kObjc3RuntimeMetadataLogicalImageInfoSection =
    "objc3.runtime.image_info";
inline constexpr const char *kObjc3RuntimeMetadataLogicalClassDescriptorSection =
    "objc3.runtime.class_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalProtocolDescriptorSection =
    "objc3.runtime.protocol_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalCategoryDescriptorSection =
    "objc3.runtime.category_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalPropertyDescriptorSection =
    "objc3.runtime.property_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalIvarDescriptorSection =
    "objc3.runtime.ivar_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorSymbolPrefix =
    "__objc3_meta_";
inline constexpr const char *kObjc3RuntimeMetadataAggregateSymbolPrefix =
    "__objc3_sec_";
inline constexpr const char *kObjc3RuntimeMetadataImageInfoSymbol =
    "__objc3_image_info";
inline constexpr const char *kObjc3RuntimeMetadataClassDescriptorAggregateSymbol =
    "__objc3_sec_class_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol =
    "__objc3_sec_protocol_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol =
    "__objc3_sec_category_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol =
    "__objc3_sec_property_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol =
    "__objc3_sec_ivar_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorLinkagePolicy =
    "private";
inline constexpr const char *kObjc3RuntimeMetadataAggregateLinkagePolicy =
    "internal";
inline constexpr const char *kObjc3RuntimeMetadataVisibilityPolicy =
    "hidden";
inline constexpr const char *kObjc3RuntimeMetadataRetentionPolicyRoot =
    "llvm.used";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_object_inspection_zero_descriptor.objc3";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionEmitPrefix =
    "module";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionObjectRelativePath =
    "module.obj";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey =
    "zero-descriptor-section-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey =
    "zero-descriptor-symbol-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionCommand =
    "llvm-readobj --sections module.obj";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolCommand =
    "llvm-objdump --syms module.obj";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixContractId =
    "objc3c.runtime.metadata.source.to.section.matrix.v1";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixOrderingModel =
    "source-graph-node-kind-order-v1";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode =
    "standalone-descriptor-section";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode =
    "no-standalone-emission-yet";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode =
    "fixture-plus-object-inspection";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode =
    "source-graph-fixture";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue =
    "(none)";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior =
    "zero-sentinel-or-count-plus-pointer-vector";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior =
    "none";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey =
    "interface-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionImplementationRowKey =
    "implementation-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionClassRowKey =
    "class-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey =
    "metaclass-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionProtocolRowKey =
    "protocol-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionCategoryRowKey =
    "category-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionPropertyRowKey =
    "property-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMethodRowKey =
    "method-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionIvarRowKey =
    "ivar-node-to-emission";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionNamedMetadataName =
    "!objc3.objc_executable_metadata_debug_projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_source_records_category_protocol_property.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrFixturePath =
    "tests/tooling/fixtures/native/hello.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionEmitPrefix =
    "module";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestRelativePath =
    "module.manifest.json";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrRelativePath =
    "module.ll";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey =
    "class-protocol-property-ivar-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey =
    "category-protocol-property-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey =
    "hello-ir-named-metadata-anchor";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe "
    "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3 "
    "--out-dir <probe-root>/class_protocol_property_ivar --emit-prefix module "
    "--no-emit-ir --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe "
    "tests/tooling/fixtures/native/runtime_metadata_source_records_category_protocol_property.objc3 "
    "--out-dir <probe-root>/category_protocol_property --emit-prefix module "
    "--no-emit-ir --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3 "
    "--out-dir <probe-root>/hello_ir_anchor --emit-prefix module --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand =
    "python -c \"import json,pathlib; "
    "payload=json.loads(pathlib.Path('module.manifest.json').read_text()); "
    "print(payload['frontend']['pipeline']['semantic_surface']['objc_executable_metadata_debug_projection'])\"";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand =
    "Select-String -Path module.ll -Pattern '!objc3.objc_executable_metadata_debug_projection'";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel =
    "typed-handoff-plus-debug-projection-manifest-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact =
    "module.manifest.json";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat =
    "objc3-runtime-metadata-envelope-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix =
    ".runtime-metadata.bin";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath =
    "module.runtime-metadata.bin";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryMagic =
    "OBJC3RM1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName =
    "runtime_ingest_packaging_contract";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName =
    "typed_lowering_handoff";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName =
    "debug_projection";
inline constexpr std::uint32_t kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion = 1u;
inline constexpr std::uint32_t kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount = 3u;
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationContractId =
    "objc3c.translation.unit.registration.surface.freeze.v1";
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract";
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationPayloadModel =
    "runtime-metadata-binary-plus-linker-retention-sidecars-v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath =
        kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath;
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath =
        "module.runtime-metadata-linker-options.rsp";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath =
        "module.runtime-metadata-discovery.json";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol =
        "__objc3_runtime_register_image_ctor";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel =
        "compiler-emits-constructor-root-runtime-owns-registration-state";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorEmissionMode =
        "reserved-not-emitted-yet";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorPriorityPolicy =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol =
        "objc3_runtime_register_image";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel =
        "input-path-plus-parse-and-lowering-replay";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestContractId =
        "objc3c.translation.unit.registration.manifest.v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_translation_unit_registration_manifest";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestPayloadModel =
        "translation-unit-registration-manifest-json-v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix =
        ".runtime-registration-manifest.json";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath =
        "module.runtime-registration-manifest.json";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanContractId =
    "objc3c.cross.module.runtime.packaging.link.plan.v1";
inline constexpr const char
    *kObjc3RuntimeCrossModuleRealizedMetadataReplayPreservationSurfaceContractId =
        "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelAbiQuerySurfaceContractId =
        "objc3c.runtime.object.model.abi.query.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRealizationLookupReflectionImplementationSurfaceContractId =
        "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanPayloadModel =
    "cross-module-runtime-link-plan-json-v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanArtifactSuffix =
    ".cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath =
        "module.cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactSuffix =
        ".cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath =
        "module.cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheArtifactSuffix =
        ".metaprogramming-macro-host-cache.json";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheArtifactRelativePath =
        "module.metaprogramming-macro-host-cache.json";
inline constexpr const char
    *kObjc3InteropBridgeHeaderArtifactSuffix = ".interop-bridge.h";
inline constexpr const char
    *kObjc3InteropBridgeHeaderArtifactRelativePath =
        "module.interop-bridge.h";
inline constexpr const char
    *kObjc3InteropBridgeModuleArtifactSuffix = ".interop-bridge.modulemap";
inline constexpr const char
    *kObjc3InteropBridgeModuleArtifactRelativePath =
        "module.interop-bridge.modulemap";
inline constexpr const char
    *kObjc3InteropBridgeArtifactSuffix = ".interop-bridge.json";
inline constexpr const char
    *kObjc3InteropBridgeArtifactRelativePath = "module.interop-bridge.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanAuthorityModel =
        "runtime-import-surface-plus-imported-registration-manifest-peer-artifacts-drive-cross-module-link-plan";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanPackagingModel =
        "compiler-emits-cross-module-link-plan-and-merged-linker-response";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel =
        "registration-ordinal-sorted-link-plan-drives-multi-image-startup-registration";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkObjectOrderModel =
        "ascending-registration-ordinal-then-translation-unit-identity-key";
inline constexpr const char
    *kObjc3CrossModuleRealizedMetadataReplayPreservationModel =
        "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix =
        "__objc3_runtime_register_image_init_stub_";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubOwnershipModel =
        "lowering-emits-init-stub-from-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel =
        "registration-manifest-authoritative-for-constructor-root-shape";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantContractId =
        "objc3c.runtime.startup.bootstrap.invariants.v1";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_startup_bootstrap_invariants";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy =
        "fail-closed-by-translation-unit-identity-key";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapRealizationOrderPolicy =
        "constructor-root-then-registration-manifest-order";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapFailureMode =
        "abort-before-user-main-no-partial-registration-commit";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapImageLocalInitializationScope =
        "runtime-owned-image-local-registration-state";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy =
        "one-startup-root-per-translation-unit-identity";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConsumptionModel =
        "startup-root-consumes-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapExecutionMode =
        "deferred-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsContractId =
    "objc3c.runtime.startup.bootstrap.semantics.v1";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics";
inline constexpr const char *kObjc3RuntimeBootstrapResultModel =
    "zero-success-negative-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel =
        "strictly-monotonic-positive-registration-order-ordinal";
inline constexpr const char *kObjc3RuntimeBootstrapStateSnapshotSymbol =
    "objc3_runtime_copy_registration_state_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel =
        "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel =
        "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedModuleNameField =
        "last_rejected_module_name";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField =
        "last_rejected_translation_unit_identity_key";
inline constexpr const char
    *kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField =
        "next_expected_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField =
        "last_successful_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField =
        "last_rejected_registration_order_ordinal";
inline constexpr int kObjc3RuntimeBootstrapSuccessStatusCode = 0;
inline constexpr int kObjc3RuntimeBootstrapInvalidDescriptorStatusCode = -1;
inline constexpr int
    kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode = -2;
inline constexpr int kObjc3RuntimeBootstrapOutOfOrderStatusCode = -3;
inline constexpr int
    kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode = -4;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal = 1u;
inline constexpr const char *kObjc3RuntimeBootstrapApiContractId =
    "objc3c.runtime.bootstrap.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapApiSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract";
inline constexpr const char *kObjc3RuntimeBootstrapApiStatusEnumType =
    "objc3_runtime_registration_status_code";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageDescriptorType =
    "objc3_runtime_image_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapApiSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeBootstrapApiRegistrationSnapshotType =
    "objc3_runtime_registration_state_snapshot";
inline constexpr const char *kObjc3RuntimeBootstrapApiStateLockingModel =
    "process-global-mutex-serialized-runtime-state";
inline constexpr const char *kObjc3RuntimeBootstrapApiStartupInvocationModel =
    "generated-init-stub-calls-runtime-register-image";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageWalkLifecycleModel =
    "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId =
        "objc3c.runtime.bootstrap.registration.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId =
        "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId =
        "objc3c.runtime.multi.image.startup.ordering.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId =
        "objc3c.runtime.object.model.realization.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRealizationLoweringReflectionArtifactSurfaceContractId =
        "objc3c.runtime.realization.lowering.reflection.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchTableReflectionRecordLoweringSurfaceContractId =
        "objc3c.runtime.dispatch.table.reflection.record.lowering.surface.v1";
inline constexpr const char *kObjc3RuntimeObjectModelQueryBoundaryModel =
    "public-runtime-header-plus-private-testing-snapshots-freeze-the-object-model-lookup-and-reflection-query-surface-without-widening-the-public-abi";
inline constexpr const char
    *kObjc3RuntimeRealizationLookupReflectionImplementationModel =
        "runtime-owned-class-property-protocol-and-method-cache-query-snapshots-publish-coherent-last-query-state-and-live-object-model-counts";
inline constexpr const char *kObjc3RuntimeReflectionQuerySurfaceContractId =
    "objc3c.runtime.reflection.query.surface.v1";
inline constexpr const char *kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId =
    "objc3c.runtime.realization.lookup.semantics.v1";
inline constexpr const char *kObjc3RuntimeClassMetaclassProtocolRealizationSurfaceContractId =
    "objc3c.runtime.class.metaclass.protocol.realization.v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentMergedDispatchSurfaceContractId =
    "objc3c.runtime.category.attachment.merged.dispatch.surface.v1";
inline constexpr const char
    *kObjc3RuntimeReflectionVisibilityCoherenceDiagnosticsSurfaceContractId =
        "objc3c.runtime.reflection.visibility.coherence.diagnostics.surface.v1";
inline constexpr const char *kObjc3RuntimeUnifiedConcurrencySourceSurfaceContractId =
    "objc3c.runtime.unified.concurrency.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeAsyncTaskActorNormalizationCompletionSurfaceContractId =
        "objc3c.runtime.async.task.actor.normalization.completion.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyLoweringMetadataSurfaceContractId =
        "objc3c.runtime.unified.concurrency.lowering.metadata.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiSurfaceContractId =
        "objc3c.runtime.unified.concurrency.runtime.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiBoundaryModel =
        "private-async-task-and-actor-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyContinuationRuntimeModel =
        "continuation-allocation-handoff-resume-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyTaskRuntimeModel =
        "task-spawn-group-cancellation-executor-hop-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyActorRuntimeModel =
        "actor-isolation-nonisolated-hop-replay-race-guard-mailbox-and-testing-snapshots-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeUnifiedConcurrencyRuntimeAbiFailClosedModel =
        "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-concurrency-runtime-abi-widening";
inline constexpr const char *kObjc3RuntimeInstallationAbiSurfaceContractId =
    "objc3c.runtime.installation.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeLoaderLifecycleSurfaceContractId =
    "objc3c.runtime.loader.lifecycle.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimAbiSurfaceContractId =
    "objc3c.runtime.release.candidate.claim.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_claim_snapshot_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimSnapshotType =
    "objc3_runtime_release_candidate_claim_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimProbePath =
    "tests/tooling/runtime/release_candidate_claim_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeReleaseCandidateClaimBoundaryModel =
    "private-release-candidate-claim-snapshot-freezes-the-final-claim-publication-contract-set-and-deprecated-path-shutdown-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeFinalReleaseEvidenceDescaffoldingImplementationSurfaceContractId =
        "objc3c.runtime.final.release.evidence.descaffolding.implementation.surface.v1";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotSymbol =
    "objc3_runtime_copy_release_candidate_evidence_state_for_testing";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceSnapshotType =
    "objc3_runtime_release_candidate_evidence_state_snapshot";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceProbePath =
    "tests/tooling/runtime/release_candidate_evidence_runtime_probe.cpp";
inline constexpr const char *kObjc3RuntimeReleaseCandidateEvidenceImplementationModel =
    "private-release-candidate-evidence-snapshot-freezes-the-live-validation-release-evidence-dashboard-gate-matrix-and-deprecated-path-shutdown-implementation-boundary";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarContractId =
    "objc3c.runtime.bootstrap.registrar.image.walk.v1";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract";
inline constexpr const char *kObjc3RuntimeBootstrapInternalHeaderPath =
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
inline constexpr const char
    *kObjc3RuntimeBootstrapStageRegistrationTableSymbol =
        "objc3_runtime_stage_registration_table_for_bootstrap";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageWalkSnapshotSymbol =
        "objc3_runtime_copy_image_walk_state_for_testing";
inline constexpr const char *kObjc3RuntimeBootstrapImageWalkModel =
    "registration-table-roots-validated-and-staged-before-realization";
inline constexpr const char
    *kObjc3RuntimeBootstrapDiscoveryRootValidationModel =
        "linker-anchor-must-point-at-discovery-root-and-discovery-root-must-close-over-registration-roots";
inline constexpr const char
    *kObjc3RuntimeBootstrapSelectorPoolInterningModel =
        "canonical-selector-pool-preinterned-during-startup-image-walk";
inline constexpr const char *kObjc3RuntimeBootstrapRealizationStagingModel =
    "registration-table-roots-retained-for-later-realization";
inline constexpr const char *kObjc3RuntimeBootstrapResetContractId =
    "objc3c.runtime.bootstrap.reset.replay.v1";
inline constexpr const char *kObjc3RuntimeBootstrapResetSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract";
inline constexpr const char
    *kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol =
        "objc3_runtime_replay_registered_images_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol =
        "objc3_runtime_copy_reset_replay_state_for_testing";
inline constexpr const char *kObjc3RuntimeInstallationLifecycleProbePath =
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp";
inline constexpr const char *kObjc3RuntimeBootstrapResetLifecycleModel =
    "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells";
inline constexpr const char *kObjc3RuntimeBootstrapReplayOrderModel =
    "replay-re-registers-retained-images-in-original-registration-order";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageLocalInitStateResetModel =
        "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay";
inline constexpr const char *kObjc3RuntimeBootstrapCatalogRetentionModel =
    "bootstrap-catalog-retained-across-reset-for-deterministic-replay";
inline constexpr const char *kObjc3RuntimeSupportLibraryTargetName =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibrarySourceRoot =
    "native/objc3c/src/runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryPublicHeaderPath =
    "native/objc3c/src/runtime/public/objc3_runtime_api.h";
inline constexpr const char *kObjc3RuntimeSupportLibraryKind = "static";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveBasename =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveRelativePath =
    "artifacts/lib/objc3_runtime.lib";
inline constexpr const char *kObjc3RuntimeSupportLibraryImplementationSourcePath =
    "native/objc3c/src/runtime/objc3_runtime.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryProbeSourcePath =
    "tests/tooling/runtime/runtime_library_probe.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryRegisterImageSymbol =
    "objc3_runtime_register_image";
inline constexpr const char *kObjc3RuntimeSupportLibraryLookupSelectorSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeSupportLibraryDispatchI32Symbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeSupportLibraryResetForTestingSymbol =
    "objc3_runtime_reset_for_testing";
inline constexpr const char *kObjc3RuntimeSupportLibraryDriverLinkMode =
    "not-linked-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringContractId =
    "objc3c.runtime.support.library.link.wiring.v1";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath =
        "scripts/check_objc3c_native_execution_smoke.ps1";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringMode =
    "emitted-object-links-against-objc3_runtime-lib";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary =
        "compiler-emits-metadata-runtime-does-not-own-source-records";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary =
        "runtime-owns-registration-lookup-and-dispatch-state";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId =
        "objc3c.bootstrap.registration.descriptor.image.root.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_image_root_source_surface";
inline constexpr const char
    *kObjc3RuntimeBootstrapModuleIdentitySourceModel =
        "module-declaration-or-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourcePragma =
        "source-pragma";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault =
        "module-derived-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel =
        "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix =
        "_registration_descriptor";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageRootDefaultSuffix = "_image_root";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel =
        "runtime-registration-descriptor-json-v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix =
        ".runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel =
        "registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel =
        "compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity";
inline constexpr const char
    *kObjc3RuntimeBlockArcUnifiedSourceSurfaceContractId =
        "objc3c.runtime.block.arc.unified.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcUnifiedSourceSurfaceModel =
        "block-arc-unified-source-surface-freezes-live-frontend-sema-ir-and-runtime-entrypoints-before-generalized-ownership-automation-or-public-abi-widening";
inline constexpr const char
    *kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceContractId =
        "objc3c.runtime.ownership.transfer.capture.family.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeOwnershipTransferCaptureFamilySourceSurfaceModel =
        "ownership-transfer-and-capture-family-source-surface-freezes-sema-level-move-capture-explicit-capture-mode-and-retainable-family-truth-before-lowering-or-runtime-lifetime-expansion";
inline constexpr const char
    *kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId =
        "objc3c.runtime.block.arc.lowering.helper.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcLoweringHelperSurfaceModel =
        "block-arc-lowering-helper-surface-freezes-live-semantic-lowering-packets-manifest-replay-keys-llvm-helper-summaries-and-private-runtime-hooks-before-cross-module-or-public-abi-expansion";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiSurfaceContractId =
        "objc3c.runtime.block.arc.runtime.abi.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel =
        "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiBlockModel =
        "promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiArcModel =
        "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints";
inline constexpr const char
    *kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel =
        "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening";
