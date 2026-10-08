#pragma once

#include <string>

#include "lower/contracts/conformance_runtime_capability_contracts.h"
#include "lower/contracts/conformance_tooling_report_contracts.h"
#include "pipeline/results/versioned_conformance_report_dto.h"

struct Objc3ToolingMachineReadableConformanceReportContractSummary {
  std::string contract_id =
      kObjc3ToolingMachineReadableConformanceReportContractId;
  std::string dependency_contract_id =
      kObjc3ToolingMachineReadableConformanceReportDependencyContractId;
  std::string lowering_contract_id =
      kObjc3VersionedConformanceReportLoweringContractId;
  std::string runtime_capability_contract_id =
      kObjc3RuntimeCapabilityReportingContractId;
  std::string frontend_surface_path =
      kObjc3ToolingMachineReadableConformanceReportSurfacePath;
  std::string payload_model =
      kObjc3ToolingMachineReadableConformanceReportPayloadModel;
  std::string authority_model =
      kObjc3ToolingMachineReadableConformanceReportAuthorityModel;
  std::string artifact_suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  std::string artifact_schema_id =
      kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  std::string runtime_capability_schema_id =
      kObjc3RuntimeCapabilityReportingSchemaId;
  std::string effective_language_profile = "canonical";
  bool migration_semantics_ready = false;
  bool lowering_contract_ready = false;
  bool runtime_capability_surface_published = false;
  bool deterministic_handoff = false;
  bool ready_for_runtime_publication = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingMachineReadableConformanceReportContractSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical" ||
      summary.effective_language_profile == "strict" ||
      summary.effective_language_profile == "strict-concurrency";
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.lowering_contract_id.empty() &&
         !summary.runtime_capability_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() &&
         !summary.artifact_suffix.empty() &&
         !summary.artifact_schema_id.empty() &&
         !summary.runtime_capability_schema_id.empty() &&
         language_profile_valid && summary.migration_semantics_ready &&
         summary.lowering_contract_ready &&
         summary.runtime_capability_surface_published &&
         summary.deterministic_handoff &&
         summary.ready_for_runtime_publication && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}
