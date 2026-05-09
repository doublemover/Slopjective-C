#pragma once

#include <cstdint>
#include <string>

// Archive/static-link discovery is the bootstrap-retention handoff: it gives
// runtime bootstrap lowering a translation-unit-stable retained-object corpus
// rather than forcing the driver or IR emitter to rediscover metadata roots.
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkDiscoveryContractId =
    "objc3c.runtime.metadata.archive.and.static.link.discovery.v1";
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkAnchorSeedModel =
    "module-and-metadata-replay-plus-translation-unit-identity";
inline constexpr const char
    *kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel =
        "input-path-plus-parse-and-lowering-replay";
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkMergeModel =
    "deduplicated-driver-flag-fan-in";
inline constexpr const char *kObjc3RuntimeMergedLinkerResponseArtifactSuffix =
    ".merged.runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3RuntimeMergedDiscoveryArtifactSuffix =
    ".merged.runtime-metadata-discovery.json";

// Constructor-root/init-array lowering is the lowering-owned bootstrap
// publication boundary consumed by IR/object emission and runtime replay.
inline constexpr const char *kObjc3RuntimeBootstrapLoweringContractId =
    "objc3c.runtime.constructor.root.init.array.lowering.v1";
inline constexpr const char *kObjc3RuntimeBootstrapLoweringBoundaryModel =
    "registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorArtifact =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel =
        "registration-descriptor-artifact-and-registration-manifest-are-authoritative-lowering-inputs";
inline constexpr const char *kObjc3RuntimeBootstrapConstructorRootEmissionState =
    "materialized-before-user-main-via-llvm-global-ctors-single-root";
inline constexpr const char *kObjc3RuntimeBootstrapInitStubEmissionState =
    "materialized-before-user-main-via-derived-init-stub";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationTableEmissionState =
        "materialized-in-native-object-artifact";
inline constexpr const char *kObjc3RuntimeBootstrapGlobalCtorListModel =
    "llvm.global_ctors-single-root-priority-65535";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix =
    "__objc3_runtime_registration_table_";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix =
        "__objc3_runtime_image_local_init_state_";

inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId =
        "objc3c.runtime.registration.descriptor.and.image.root.lowering.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringModel =
        "frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection =
        "objc3.runtime.registration_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapImageRootLogicalSection =
    "objc3.runtime.image_root";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix =
        "__objc3_runtime_registration_descriptor_";
inline constexpr const char *kObjc3RuntimeBootstrapImageRootSymbolPrefix =
    "__objc3_runtime_image_root_";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorPayloadModel =
        "registration-descriptor-record-points-at-image-root-image-descriptor-registration-table-linker-anchor-and-init-state";
inline constexpr const char *kObjc3RuntimeBootstrapImageRootPayloadModel =
    "image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root";

inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId =
        "objc3c.runtime.bootstrap.archive.static.link.replay.corpus.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel =
        "merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel =
        "plain-link-omits-bootstrap-images-retained-link-replays-them";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableLayoutModel =
    "abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-keypath-descriptors-image-local-init-state";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitializationModel =
    "guarded-once-per-image-local-state-cell";
inline constexpr std::uint64_t kObjc3RuntimeBootstrapRegistrationTableAbiVersion =
    2u;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount = 12u;

std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
std::string Objc3RuntimeBootstrapLoweringBoundarySummary();
std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary();
std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary();
