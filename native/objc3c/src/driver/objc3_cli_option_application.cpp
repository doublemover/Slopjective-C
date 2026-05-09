#include "driver/objc3_cli_option_application.h"

#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "driver/objc3_cli_conformance_options.h"
#include "driver/objc3_cli_language_options.h"
#include "driver/objc3_cli_output_options.h"
#include "driver/objc3_cli_runtime_options.h"
#include "driver/objc3_cli_toolchain_options.h"

bool ApplyObjc3CliOption(int &index,
                         int argc,
                         char **argv,
                         Objc3CliOptions &options,
                         std::string &error) {
  const std::string flag = argv[index];
  bool matched = false;
  if (!TryApplyObjc3CliLanguageOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  if (!TryApplyObjc3CliOutputOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(flag,
                                                                   error)) {
    return false;
  }
  if (!TryApplyObjc3CliConformanceOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  if (!TryApplyObjc3CliRuntimeOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  if (!TryApplyObjc3CliToolchainOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }

  error = "unknown arg: " + flag;
  return false;
}
