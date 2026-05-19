#include "driver/objc3_cli_option_validation.h"

#include "config/objc3_language_profile.h"
#include "driver/objc3_cli_artifact_options_contract.h"
#include "driver/objc3_cli_usage.h"
#include "io/objc3_process.h"

bool ValidateObjc3CliOptions(const Objc3CliOptions &options,
                             std::string &error) {
  if (options.command_mode == Objc3CliCommandMode::kValidateConformance) {
    if (!options.input.empty()) {
      error =
          "--validate-objc3-conformance is a validation-only mode and does not accept a source input";
      return false;
    }
    if (options.validate_conformance_report_path.empty()) {
      error = "missing --validate-objc3-conformance <report.json> path";
      return false;
    }
  } else if (options.input.empty()) {
    error = Objc3CliUsage();
    return false;
  }

  if (!ValidateObjc3CliArtifactOptions(options, error)) {
    return false;
  }

  if (!IsObjc3JsonConformanceFormat(options.emit_objc3_conformance_format)) {
    error = BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
        options.emit_objc3_conformance_format);
    return false;
  }

  if (options.command_mode == Objc3CliCommandMode::kCompile &&
      !objc3c::config::IsCanonicalLanguageVersion(options.language_version)) {
    error = objc3c::config::UnsupportedLanguageVersionDiagnostic(
        options.language_version);
    return false;
  }

  error.clear();
  return true;
}
