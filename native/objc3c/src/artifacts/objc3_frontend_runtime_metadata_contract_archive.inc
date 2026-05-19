inline constexpr const char *kArtifactRuntimeArchiveStaticLinkDiscoveryContractId =
    "objc3c.runtime.metadata.archive.and.static.link.discovery.v1";
inline constexpr const char *kArtifactRuntimeArchiveStaticLinkAnchorSeedModel =
    "module-and-metadata-replay-plus-translation-unit-identity";
inline constexpr const char
    *kArtifactRuntimeArchiveStaticLinkTranslationUnitIdentityModel =
        "input-path-plus-parse-and-lowering-replay";
inline constexpr const char *kArtifactRuntimeArchiveStaticLinkMergeModel =
    "deduplicated-driver-flag-fan-in";
inline constexpr const char *kArtifactRuntimeMergedLinkerResponseArtifactSuffix =
    ".merged.runtime-metadata-linker-options.rsp";
inline constexpr const char *kArtifactRuntimeMergedDiscoveryArtifactSuffix =
    ".merged.runtime-metadata-discovery.json";

void ApplyRuntimeMetadataArchiveStaticLinkDiscoveryContract(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::filesystem::path &input_path,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key) {
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_contract_id =
      kArtifactRuntimeArchiveStaticLinkDiscoveryContractId;
  ir_frontend_metadata.runtime_metadata_archive_static_link_anchor_seed_model =
      kArtifactRuntimeArchiveStaticLinkAnchorSeedModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_model =
      kArtifactRuntimeArchiveStaticLinkTranslationUnitIdentityModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_merge_model =
      kArtifactRuntimeArchiveStaticLinkMergeModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_response_artifact_suffix =
      kArtifactRuntimeMergedLinkerResponseArtifactSuffix;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_discovery_artifact_suffix =
      kArtifactRuntimeMergedDiscoveryArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_ready =
      true;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_key =
      objc3::artifacts::identity::BuildObjc3TranslationUnitIdentityKey(
          objc3::artifacts::identity::Objc3TranslationUnitIdentityEvidence{
              input_path,
              parse_artifact_replay_key,
              lowering_boundary_replay_key,
          });
}
