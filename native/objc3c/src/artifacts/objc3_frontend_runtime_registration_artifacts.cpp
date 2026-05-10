#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {
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

Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary() {
  Objc3RuntimeSupportLibraryContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.target_name_frozen = true;
  summary.exported_entrypoints_frozen = true;
  summary.ownership_boundaries_frozen = true;
  summary.build_constraints_frozen = true;
  summary.strict_dispatch_errors_required = true;
  summary.native_runtime_library_present = false;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_runtime_library_skeleton = true;
  return summary;
}

Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  Objc3RuntimeSupportLibraryCoreFeatureSummary summary;
  summary.support_library_contract_id = runtime_support_library.contract_id;
  summary.metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  summary.fail_closed = true;
  summary.native_runtime_library_sources_present = true;
  summary.native_runtime_library_header_present = true;
  summary.native_runtime_library_archive_build_enabled = true;
  summary.native_runtime_library_entrypoints_implemented = true;
  summary.selector_lookup_stateful = true;
  summary.deterministic_dispatch_formula_matches_runtime_test_helper = true;
  summary.reset_for_testing_supported = true;
  summary.strict_dispatch_errors_required = true;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_driver_link_wiring =
      IsReadyObjc3RuntimeSupportLibraryContractSummary(runtime_support_library);
  return summary;
}

Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  Objc3RuntimeSupportLibraryLinkWiringSummary summary;
  summary.support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  summary.fail_closed = true;
  summary.runtime_library_archive_available =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  summary.driver_emits_runtime_link_contract = true;
  summary.execution_smoke_consumes_runtime_library = true;
  summary.strict_dispatch_errors_required = true;
  summary.ready_for_runtime_library_consumption =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library_core_feature);
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";binary_boundary_contract_id=" << summary.binary_boundary_contract_id
      << ";archive_static_link_contract_id="
      << summary.archive_static_link_contract_id
      << ";object_emission_closeout_contract_id="
      << summary.object_emission_closeout_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";registration_surface_path=" << summary.registration_surface_path
      << ";registration_payload_model="
      << summary.registration_payload_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";constructor_emission_mode=" << summary.constructor_emission_mode
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";binary_boundary_replay_key=" << summary.binary_boundary_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeTranslationUnitRegistrationContractSummary summary;
  summary.fail_closed = true;
  summary.boundary_frozen = true;
  summary.binary_boundary_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          runtime_ingest_binary_boundary);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  const std::string archive_static_link_summary =
      BuildArtifactRuntimeMetadataArchiveStaticLinkDiscoverySummary();
  summary.archive_static_link_surface_ready =
      archive_static_link_summary.find(
          std::string("contract=") +
          kObjc3ArtifactRuntimeArchiveStaticLinkDiscoveryContractId) !=
          std::string::npos &&
      archive_static_link_summary.find(
          std::string("translation_unit_identity_model=") +
          summary.translation_unit_identity_model) != std::string::npos;
  const std::string object_emission_closeout_summary =
      BuildArtifactRuntimeMetadataObjectEmissionCloseoutSummary();
  summary.object_emission_closeout_surface_ready =
      object_emission_closeout_summary.find(
          std::string("contract=") +
          kObjc3ArtifactRuntimeMetadataObjectEmissionCloseoutContractId) !=
          std::string::npos &&
      object_emission_closeout_summary.find(
          "non_goals=no-startup-registration-or-runtime-bootstrap") !=
          std::string::npos;
  summary.runtime_owned_payload_inventory_published = true;
  summary.constructor_root_reserved_not_emitted = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_bootstrap_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  if (summary.binary_boundary_ready) {
    summary.binary_boundary_replay_key = runtime_ingest_binary_boundary.replay_key;
  }
  summary.ready_for_registration_manifest_implementation =
      summary.binary_boundary_ready &&
      summary.archive_static_link_surface_ready &&
      summary.object_emission_closeout_surface_ready &&
      summary.runtime_support_library_link_wiring_ready;
  if (summary.ready_for_registration_manifest_implementation) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration surface contract is incomplete";
  }
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";translation_unit_registration_contract_id="
      << summary.translation_unit_registration_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";manifest_payload_model=" << summary.manifest_payload_model
      << ";manifest_artifact_relative_path="
      << summary.manifest_artifact_relative_path
      << ";runtime_support_library_archive_relative_path="
      << summary.runtime_support_library_archive_relative_path
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";constructor_init_stub_ownership_model="
      << summary.constructor_init_stub_ownership_model
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";translation_unit_registration_replay_key="
      << summary.translation_unit_registration_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    std::uint64_t translation_unit_registration_order_ordinal) {
  Objc3RuntimeTranslationUnitRegistrationManifestSummary summary;
  summary.fail_closed = true;
  summary.translation_unit_registration_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
          registration_contract);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.runtime_manifest_template_published = true;
  summary.constructor_root_manifest_authoritative = true;
  summary.constructor_root_reserved_for_lowering = true;
  summary.init_stub_emission_deferred_to_lowering = true;
  summary.runtime_registration_artifact_emitted_by_driver = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  summary.class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  summary.protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  summary.category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  summary.property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  summary.ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  summary.total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  if (translation_unit_registration_order_ordinal > 0) {
    summary.translation_unit_registration_order_ordinal =
        translation_unit_registration_order_ordinal;
  }
  if (summary.translation_unit_registration_contract_ready) {
    summary.translation_unit_registration_replay_key =
        registration_contract.replay_key;
  }
  summary.ready_for_lowering_init_stub_emission =
      summary.translation_unit_registration_contract_ready &&
      summary.runtime_support_library_link_wiring_ready;
  summary.launch_integration_ready =
      summary.ready_for_lowering_init_stub_emission;
  if (summary.ready_for_lowering_init_stub_emission) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationManifestReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration manifest summary is incomplete";
  }
  return summary;
}

}  // namespace objc3::artifacts::frontend
