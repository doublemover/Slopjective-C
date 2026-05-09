#include "diag/objc3_diag_catalog_artifact_tooling_data.h"

#include <array>

namespace {

constexpr std::array<NativeDiagCodeCatalogEntry,
                     kNativeDiagArtifactToolingCatalogEntryCount>
    kNativeDiagArtifactToolingCatalog = {{
        {'A', Objc3DiagnosticSubsystem::kArtifact,
         Objc3DiagnosticCategory::kArtifact,
         "artifact, manifest, and schema diagnostics", 1, 399,
         kObjc3ArtifactDiagnosticCatalogOwner},
        {'T', Objc3DiagnosticSubsystem::kTooling,
         Objc3DiagnosticCategory::kTooling,
         "tooling and workflow diagnostics", 1, 399,
         kObjc3ToolingDiagnosticCatalogOwner},
    }};

}  // namespace

std::span<const NativeDiagCodeCatalogEntry>
NativeDiagArtifactToolingCatalogData() {
  return std::span<const NativeDiagCodeCatalogEntry>(
      kNativeDiagArtifactToolingCatalog.data(),
      kNativeDiagArtifactToolingCatalog.size());
}
