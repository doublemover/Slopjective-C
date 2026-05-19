#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"

#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

namespace {

using objc3::artifacts::frontend::BuildExecutableMetadataDebugProjectionSummary;
using objc3::artifacts::frontend::
    BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary;
using objc3::artifacts::frontend::BuildExecutableMetadataRuntimeIngestBinaryEnvelope;
using objc3::artifacts::frontend::
    BuildExecutableMetadataRuntimeIngestPackagingContractSummary;
using objc3::artifacts::frontend::
    BuildRuntimeMetadataObjectInspectionHarnessSummary;
using objc3::artifacts::frontend::BuildRuntimeMetadataSectionAbiFreezeSummary;
using objc3::artifacts::frontend::BuildRuntimeMetadataSectionPublicationSummary;
using objc3::artifacts::frontend::
    BuildRuntimeMetadataSourceToSectionMatrixSummary;

}  // namespace

Objc3FrontendArtifactRuntimeMetadataPlan
BuildObjc3FrontendArtifactRuntimeMetadataPlan(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactRuntimeMetadataPlan plan;
  const Objc3ExecutableMetadataTypedLoweringHandoff
      &executable_metadata_typed_lowering_handoff =
          pipeline_result.executable_metadata_typed_lowering_handoff;
  const Objc3RuntimeMetadataSourceOwnershipBoundary
      &runtime_metadata_source_ownership =
          pipeline_result.runtime_metadata_source_ownership_boundary;
  const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
      pipeline_result.runtime_export_legality_boundary;
  const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
      pipeline_result.runtime_export_enforcement_summary;

  plan.runtime_metadata_section_abi =
      BuildRuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
  plan.runtime_metadata_section_publication =
      BuildRuntimeMetadataSectionPublicationSummary(
          plan.runtime_metadata_section_abi,
          runtime_export_legality,
          runtime_export_enforcement);
  plan.runtime_metadata_object_inspection =
      BuildRuntimeMetadataObjectInspectionHarnessSummary(
          plan.runtime_metadata_section_abi,
          plan.runtime_metadata_section_publication);
  plan.runtime_metadata_source_to_section_matrix =
      BuildRuntimeMetadataSourceToSectionMatrixSummary(
          pipeline_result.executable_metadata_source_graph,
          plan.runtime_metadata_section_abi,
          plan.runtime_metadata_section_publication,
          plan.runtime_metadata_object_inspection);
  plan.executable_metadata_debug_projection =
      BuildExecutableMetadataDebugProjectionSummary(
          executable_metadata_typed_lowering_handoff);
  plan.executable_metadata_runtime_ingest_packaging_contract =
      BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
          executable_metadata_typed_lowering_handoff,
          plan.executable_metadata_debug_projection);
  plan.executable_metadata_runtime_ingest_binary_payload =
      BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
          plan.executable_metadata_runtime_ingest_packaging_contract,
          executable_metadata_typed_lowering_handoff,
          plan.executable_metadata_debug_projection);
  plan.executable_metadata_runtime_ingest_binary_boundary =
      BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          plan.executable_metadata_runtime_ingest_packaging_contract,
          executable_metadata_typed_lowering_handoff,
          plan.executable_metadata_debug_projection,
          plan.executable_metadata_runtime_ingest_binary_payload);
  return plan;
}
