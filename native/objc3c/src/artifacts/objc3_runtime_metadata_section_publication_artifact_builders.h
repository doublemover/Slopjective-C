#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend::runtime_metadata_section_publication {

struct RuntimeMetadataSectionPublicationArtifactBuilder {
  [[nodiscard]] static Objc3RuntimeMetadataSectionAbiFreezeSummary BuildAbiFreeze(
      const Objc3RuntimeMetadataSourceOwnershipBoundary
          &runtime_metadata_source_ownership,
      const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
      const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement);

  [[nodiscard]] static Objc3RuntimeMetadataSectionPublicationSummary
  BuildPublication(
      const Objc3RuntimeMetadataSectionAbiFreezeSummary
          &runtime_metadata_section_abi,
      const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
      const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement);

  [[nodiscard]] static Objc3RuntimeMetadataObjectInspectionHarnessSummary
  BuildObjectInspectionHarness(
      const Objc3RuntimeMetadataSectionAbiFreezeSummary
          &runtime_metadata_section_abi,
      const Objc3RuntimeMetadataSectionPublicationSummary
          &runtime_metadata_section_publication);
};

}  // namespace objc3::artifacts::frontend::runtime_metadata_section_publication
