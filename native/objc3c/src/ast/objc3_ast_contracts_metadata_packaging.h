#pragma once

#include <cstdint>
#include <string>

#include "artifacts/identity/artifact_identity.h"

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
    objc3::artifacts::identity::kObjc3NativeDefaultArtifactStem;
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionObjectRelativePath =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectArtifactName;
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey =
    "zero-descriptor-section-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey =
    "zero-descriptor-symbol-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionCommand =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectSectionInventoryCommand;
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolCommand =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectSymbolInventoryCommand;
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
    objc3::artifacts::identity::kObjc3NativeDefaultArtifactStem;
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestRelativePath =
    objc3::artifacts::identity::kObjc3NativeDefaultManifestArtifactName;
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrRelativePath =
    objc3::artifacts::identity::kObjc3NativeDefaultIrArtifactName;
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey =
    "class-protocol-property-ivar-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey =
    "category-protocol-property-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey =
    "hello-ir-named-metadata-anchor";
inline const std::string kObjc3ExecutableMetadataDebugProjectionClassProbeCommand =
    objc3::artifacts::identity::BuildObjc3NativeFrontendRunnerProbeCommand(
        kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
        "<probe-root>/class_protocol_property_ivar",
        kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
        true,
        true);
inline const std::string
    kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand =
        objc3::artifacts::identity::BuildObjc3NativeFrontendRunnerProbeCommand(
            kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
            "<probe-root>/category_protocol_property",
            kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
            true,
            true);
inline const std::string kObjc3ExecutableMetadataDebugProjectionIrProbeCommand =
    objc3::artifacts::identity::BuildObjc3NativeFrontendRunnerProbeCommand(
        kObjc3ExecutableMetadataDebugProjectionIrFixturePath,
        "<probe-root>/hello_ir_anchor",
        kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
        false,
        true);
inline const std::string
    kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand =
        std::string("python -c \"import json,pathlib; "
                    "payload=json.loads(pathlib.Path('") +
        objc3::artifacts::identity::kObjc3NativeDefaultManifestArtifactName +
        "').read_text()); "
        "print(payload['frontend']['pipeline']['semantic_surface']['objc_executable_metadata_debug_projection'])\"";
inline const std::string kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand =
    std::string("Select-String -Path ") +
    objc3::artifacts::identity::kObjc3NativeDefaultIrArtifactName +
    " -Pattern '!objc3.objc_executable_metadata_debug_projection'";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel =
    "typed-handoff-plus-debug-projection-manifest-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact =
    objc3::artifacts::identity::kObjc3NativeDefaultManifestArtifactName;
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat =
    "objc3-runtime-metadata-envelope-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix =
    objc3::artifacts::identity::kObjc3NativeRuntimeMetadataBinaryArtifactSuffix;
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath =
    objc3::artifacts::identity::kObjc3NativeDefaultRuntimeMetadataBinaryArtifactName;
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
        objc3::artifacts::identity::
            kObjc3NativeDefaultRuntimeMetadataLinkerOptionsArtifactName;
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath =
        objc3::artifacts::identity::
            kObjc3NativeDefaultRuntimeMetadataDiscoveryArtifactName;
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
        objc3::artifacts::identity::
            kObjc3NativeRuntimeRegistrationManifestArtifactSuffix;
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath =
        objc3::artifacts::identity::
            kObjc3NativeDefaultRuntimeRegistrationManifestArtifactName;
