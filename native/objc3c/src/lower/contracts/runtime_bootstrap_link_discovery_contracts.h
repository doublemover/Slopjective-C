#pragma once

#include <string>

// Archive/static-link discovery is the bootstrap-retention handoff: it gives
// runtime bootstrap lowering a translation-unit-stable retained-object corpus
// and replay proof instead of forcing the driver or IR emitter to rediscover
// metadata roots.
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

inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId =
        "objc3c.runtime.bootstrap.archive.static.link.replay.corpus.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel =
        "merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel =
        "plain-link-omits-bootstrap-images-retained-link-replays-them";

std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary();
