#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

std::string Objc3VersionedConformanceReportLoweringContractSummary() {
  std::ostringstream out;
  // lowering freeze anchor: lane-C lowers the existing A/B truth
  // packets into one emitted machine-readable sidecar so later runtime
  // capability reporting and driver publication consume the same versioned
  // report instead of reconstructing feature truth from docs or release notes.
  out << "contract=" << kObjc3VersionedConformanceReportLoweringContractId
      << ";semantic_contract_id="
      << kObjc3VersionedConformanceReportLoweringSemanticContractId
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";artifact_schema_id="
      << kObjc3VersionedConformanceReportLoweringArtifactSchemaId
      << ";surface_path="
      << kObjc3VersionedConformanceReportLoweringSurfacePath
      << ";payload_model="
      << kObjc3VersionedConformanceReportLoweringPayloadModel
      << ";authority_model="
      << kObjc3VersionedConformanceReportLoweringAuthorityModel
      << ";known_unsupported_model="
      << kObjc3VersionedConformanceReportKnownUnsupportedModel
      << ";selection_model="
      << kObjc3VersionedConformanceReportSelectionModel
      << ";canonical_interface_mode="
      << kObjc3VersionedConformanceReportCanonicalInterfaceMode
      << ";publication_model="
      << kObjc3VersionedConformanceReportPublicationModel
      << ";non_goals=no-new-runtime-capability-families-or-strictness-enablement";
  return out.str();
}

std::string Objc3ToolingMachineReadableConformanceReportContractLoweringSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ToolingMachineReadableConformanceReportContractId
      << ";dependency_contract_id="
      << kObjc3ToolingMachineReadableConformanceReportDependencyContractId
      << ";surface_path="
      << kObjc3ToolingMachineReadableConformanceReportSurfacePath
      << ";payload_model="
      << kObjc3ToolingMachineReadableConformanceReportPayloadModel
      << ";authority_model="
      << kObjc3ToolingMachineReadableConformanceReportAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";artifact_schema_id="
      << kObjc3VersionedConformanceReportLoweringArtifactSchemaId
      << ";runtime_capability_schema_id="
      << kObjc3RuntimeCapabilityReportingSchemaId
      << ";non_goals=no-second-report-format-or-release-truth-surface";
  return out.str();
}

std::string Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ToolingFeatureAwareConformanceReportEmissionContractId
      << ";dependency_contract_id="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId
      << ";surface_path="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath
      << ";payload_model="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel
      << ";authority_model="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";non_goals=no-second-report-sidecar-or-report-authority";
  return out.str();
}

std::string Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId
      << ";dependency_contract_id="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId
      << ";surface_path="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath
      << ";payload_model="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel
      << ";authority_model="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";non_goals=no-second-release-evidence-sidecar-or-driver-only-shard-truth";
  return out.str();
}

std::string Objc3RuntimeCapabilityReportingContractSummary() {
  std::ostringstream out;
  // capability-reporting anchor: lane-C must publish one truthful
  // machine-readable runtime/public capability payload that mirrors the
  // lowered conformance truth surface instead of deriving product claims from
  // release notes, docs, or ad hoc driver state.
  out << "contract=" << kObjc3RuntimeCapabilityReportingContractId
      << ";schema_id=" << kObjc3RuntimeCapabilityReportingSchemaId
      << ";surface_path=" << kObjc3RuntimeCapabilityReportingSurfacePath
      << ";profile_model=" << kObjc3RuntimeCapabilityReportingProfileModel
      << ";optional_feature_model="
      << kObjc3RuntimeCapabilityReportingOptionalFeatureModel
      << ";version_model=" << kObjc3RuntimeCapabilityReportingVersionModel
      << ";public_schema_id=" << kObjc3RuntimeCapabilityPublicSchemaId
      << ";strictness_mode=" << kObjc3RuntimeCapabilityStrictnessMode
      << ";concurrency_mode=" << kObjc3RuntimeCapabilityConcurrencyMode
      << ";target_triple=" << kObjc3RuntimeCapabilityTargetTriple
      << ";language_version=" << kObjc3RuntimeCapabilityLanguageVersion
      << ";non_goals=no-strictness-promotion-or-runtime-capability-overclaim";
  return out.str();
}

std::string Objc3RuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(object_format, logical_section);
}

std::string Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name) {
  return BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
      object_format, symbol_name);
}

std::string Objc3RuntimeMetadataHostSectionForLogicalName(
    const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(HostRuntimeMetadataObjectFormat(),
                                                  logical_section);
}
