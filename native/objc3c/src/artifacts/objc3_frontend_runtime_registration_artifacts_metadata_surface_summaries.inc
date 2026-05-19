namespace {

inline constexpr const char
    *kObjc3ArtifactRuntimeArchiveStaticLinkDiscoveryContractId =
        "objc3c.runtime.metadata.archive.and.static.link.discovery.v1";
inline constexpr const char *kObjc3ArtifactRuntimeArchiveStaticLinkAnchorSeedModel =
    "module-and-metadata-replay-plus-translation-unit-identity";
inline constexpr const char
    *kObjc3ArtifactRuntimeArchiveStaticLinkTranslationUnitIdentityModel =
        "input-path-plus-parse-and-lowering-replay";
inline constexpr const char *kObjc3ArtifactRuntimeArchiveStaticLinkMergeModel =
    "deduplicated-driver-flag-fan-in";
inline constexpr const char
    *kObjc3ArtifactRuntimeMergedLinkerResponseArtifactSuffix =
        ".merged.runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3ArtifactRuntimeMergedDiscoveryArtifactSuffix =
    ".merged.runtime-metadata-discovery.json";
inline constexpr const char
    *kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutContractId =
        "objc3c.runtime.cross.lane.object.emission.closeout.v1";
inline constexpr const char
    *kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutEvidenceModel =
        "integrated-summary-plus-native-object-emission-probes";
inline constexpr const char
    *kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutFailureModel =
        "fail-closed-on-summary-or-integrated-probe-drift";

std::string BuildArtifactRuntimeMetadataArchiveStaticLinkDiscoverySummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3ArtifactRuntimeArchiveStaticLinkDiscoveryContractId
      << ";anchor_seed_model="
      << kObjc3ArtifactRuntimeArchiveStaticLinkAnchorSeedModel
      << ";translation_unit_identity_model="
      << kObjc3ArtifactRuntimeArchiveStaticLinkTranslationUnitIdentityModel
      << ";merge_model=" << kObjc3ArtifactRuntimeArchiveStaticLinkMergeModel
      << ";merged_linker_response_artifact_suffix="
      << kObjc3ArtifactRuntimeMergedLinkerResponseArtifactSuffix
      << ";merged_discovery_artifact_suffix="
      << kObjc3ArtifactRuntimeMergedDiscoveryArtifactSuffix
      << ";non_goals=no-runtime-registration-or-startup-bootstrap";
  return out.str();
}

std::string BuildArtifactRuntimeMetadataObjectEmissionCloseoutSummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutContractId
      << ";evidence_model="
      << kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutEvidenceModel
      << ";failure_model="
      << kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutFailureModel
      << ";non_goals=no-startup-registration-or-runtime-bootstrap";
  return out.str();
}

}  // namespace
