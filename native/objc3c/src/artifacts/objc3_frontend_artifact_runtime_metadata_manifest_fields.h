#pragma once

#include <iosfwd>

struct Objc3RuntimeExportEnforcementSummary;
struct Objc3RuntimeExportLegalityBoundary;
struct Objc3RuntimeMetadataObjectInspectionHarnessSummary;
struct Objc3RuntimeMetadataSectionAbiFreezeSummary;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeMetadataSourceOwnershipBoundary;

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
        &runtime_metadata_object_inspection);

}  // namespace objc3::artifacts::frontend
