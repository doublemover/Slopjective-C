#include "driver/objc3_driver_conformance_validation_paths.h"

#include "io/objc3_manifest_artifacts.h"
#include "lower/contracts/conformance_versioned_report_contracts.h"

bool TryDeriveObjc3DriverConformanceEmitPrefix(
    const std::filesystem::path &report_path,
    std::string &emit_prefix) {
  const std::string report_name = report_path.filename().string();
  const std::string suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  if (report_name.size() <= suffix.size() ||
      report_name.rfind(suffix) != report_name.size() - suffix.size()) {
    emit_prefix.clear();
    return false;
  }
  emit_prefix = report_name.substr(0u, report_name.size() - suffix.size());
  return true;
}

bool TryDeriveObjc3DriverConformancePublicationPath(
    const std::filesystem::path &report_path,
    std::filesystem::path &publication_path) {
  std::string emit_prefix;
  if (!TryDeriveObjc3DriverConformanceEmitPrefix(report_path, emit_prefix)) {
    return false;
  }
  publication_path =
      BuildConformancePublicationArtifactPath(report_path.parent_path(),
                                             emit_prefix);
  return true;
}
