#include "artifacts/objc3_frontend_artifact_metadata_mode.h"

#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"

bool Objc3FrontendArtifactMetadataOnlyIrEmissionMode(
    const Objc3FrontendPipelineResult &pipeline_result) {
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
  const Objc3FrontendArtifactRuntimeMetadataPlan runtime_metadata_plan =
      BuildObjc3FrontendArtifactRuntimeMetadataPlan(pipeline_result);
  return IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
             executable_metadata_typed_lowering_handoff) &&
         IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
             runtime_metadata_source_ownership) &&
         IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) &&
         IsReadyObjc3RuntimeExportEnforcementSummary(
             runtime_export_enforcement) &&
         IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
             runtime_metadata_plan.runtime_metadata_section_abi) &&
         IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
             runtime_metadata_plan.runtime_metadata_section_publication) &&
         IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
             runtime_metadata_plan.runtime_metadata_source_to_section_matrix) &&
         IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
             runtime_metadata_plan.executable_metadata_debug_projection) &&
         IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
             runtime_metadata_plan
                 .executable_metadata_runtime_ingest_packaging_contract) &&
         IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
             runtime_metadata_plan
                 .executable_metadata_runtime_ingest_binary_boundary) &&
         runtime_metadata_plan.runtime_metadata_section_publication
                 .class_descriptor_count > 0u;
}
