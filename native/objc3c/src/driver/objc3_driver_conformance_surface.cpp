#include "driver/objc3_driver_conformance_surface.h"

#include <string>

#include "driver/objc3_cli_conformance_profile.h"
#include "io/objc3_process.h"

bool ValidateObjc3DriverConformanceSelection(
    const Objc3CliOptions &cli_options,
    std::string &error) {
  error.clear();
  if (!IsObjc3JsonConformanceFormat(cli_options.emit_objc3_conformance_format)) {
    error = BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
        cli_options.emit_objc3_conformance_format);
    return false;
  }

  const std::string selected_profile =
      ConformanceProfileName(cli_options.conformance_profile);
  if (!IsObjc3ClaimedConformanceProfile(selected_profile)) {
    error =
        BuildUnsupportedObjc3ConformanceProfileSelectionDiagnostic(selected_profile);
    return false;
  }
  return true;
}

Objc3DriverConformanceProfileSelection
BuildObjc3DriverConformanceProfileSelection(
    const Objc3CliOptions &cli_options) {
  Objc3DriverConformanceProfileSelection selection;
  selection.selected_profile =
      ConformanceProfileName(cli_options.conformance_profile);
  selection.selected_profile_supported =
      IsObjc3ClaimedConformanceProfile(selection.selected_profile);
  selection.supported_profile_ids = BuildObjc3ClaimedConformanceProfileIds();
  selection.rejected_profile_ids = BuildObjc3RejectedConformanceProfileIds();
  selection.release_targeted_profile_ids =
      BuildObjc3ReleaseTargetedProfileIds();
  return selection;
}
