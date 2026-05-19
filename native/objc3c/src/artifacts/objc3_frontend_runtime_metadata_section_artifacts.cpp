#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include "artifacts/objc3_runtime_metadata_section_publication_artifact_builders.h"
#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildAbiFreeze(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildPublication(
          runtime_metadata_section_abi,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::
          BuildObjectInspectionHarness(runtime_metadata_section_abi,
                                       runtime_metadata_section_publication);
}

}  // namespace objc3::artifacts::frontend
