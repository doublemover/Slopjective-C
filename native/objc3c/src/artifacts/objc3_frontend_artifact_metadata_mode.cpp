#include "artifacts/objc3_frontend_artifact_metadata_mode.h"

#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <string>

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

bool Objc3FrontendArtifactMetadataOnlyIrEmissionMode(
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3ExecutableMetadataTypedLoweringHandoff
      &executable_metadata_typed_lowering_handoff =
          pipeline_result.executable_metadata_typed_lowering_handoff;
  const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =
      pipeline_result.executable_metadata_source_graph;
  const Objc3RuntimeMetadataSourceOwnershipBoundary
      &runtime_metadata_source_ownership =
          pipeline_result.runtime_metadata_source_ownership_boundary;
  const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
      pipeline_result.runtime_export_legality_boundary;
  const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
      pipeline_result.runtime_export_enforcement_summary;
  const Objc3RuntimeMetadataSectionAbiFreezeSummary runtime_metadata_section_abi =
      BuildRuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
  const Objc3RuntimeMetadataSectionPublicationSummary
      runtime_metadata_section_publication =
          BuildRuntimeMetadataSectionPublicationSummary(
              runtime_metadata_section_abi,
              runtime_export_legality,
              runtime_export_enforcement);
  const Objc3RuntimeMetadataObjectInspectionHarnessSummary
      runtime_metadata_object_inspection =
          BuildRuntimeMetadataObjectInspectionHarnessSummary(
              runtime_metadata_section_abi,
              runtime_metadata_section_publication);
  const Objc3RuntimeMetadataSourceToSectionMatrixSummary
      runtime_metadata_source_to_section_matrix =
          BuildRuntimeMetadataSourceToSectionMatrixSummary(
              executable_metadata_source_graph,
              runtime_metadata_section_abi,
              runtime_metadata_section_publication,
              runtime_metadata_object_inspection);
  const Objc3ExecutableMetadataDebugProjectionSummary
      executable_metadata_debug_projection =
          BuildExecutableMetadataDebugProjectionSummary(
              executable_metadata_typed_lowering_handoff);
  const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
      executable_metadata_runtime_ingest_packaging_contract =
          BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
              executable_metadata_typed_lowering_handoff,
              executable_metadata_debug_projection);
  const std::string executable_metadata_runtime_ingest_binary_payload =
      BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
          executable_metadata_runtime_ingest_packaging_contract,
          executable_metadata_typed_lowering_handoff,
          executable_metadata_debug_projection);
  const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
      executable_metadata_runtime_ingest_binary_boundary =
          BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
              executable_metadata_runtime_ingest_packaging_contract,
              executable_metadata_typed_lowering_handoff,
              executable_metadata_debug_projection,
              executable_metadata_runtime_ingest_binary_payload);
  return IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
             executable_metadata_typed_lowering_handoff) &&
         IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
             runtime_metadata_source_ownership) &&
         IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) &&
         IsReadyObjc3RuntimeExportEnforcementSummary(
             runtime_export_enforcement) &&
         IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
             runtime_metadata_section_abi) &&
         IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
             runtime_metadata_section_publication) &&
         IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
             runtime_metadata_source_to_section_matrix) &&
         IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
             executable_metadata_debug_projection) &&
         IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
             executable_metadata_runtime_ingest_packaging_contract) &&
         IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
             executable_metadata_runtime_ingest_binary_boundary) &&
         runtime_metadata_section_publication.class_descriptor_count > 0u;
}
