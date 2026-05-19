#include "artifacts/objc3_frontend_artifact_runtime_metadata_manifest_fields.h"

#include <ostream>

#include "runtime/metadata/runtime_metadata_export_surfaces.h"
#include "runtime/metadata/runtime_metadata_section_surfaces.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeMetadataPublicationManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_source_ownership.inc"
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_export_legality.inc"
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_export_enforcement.inc"
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_section_abi.inc"
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_section_publication.inc"
#include "objc3_frontend_artifact_runtime_metadata_manifest_fields_object_inspection.inc"
}

}  // namespace objc3::artifacts::frontend
