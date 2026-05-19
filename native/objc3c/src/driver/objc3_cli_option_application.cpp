#include "driver/objc3_cli_option_application.h"

#include "diagnostics/modes/canonical_rejections.h"
#include "driver/objc3_cli_option_groups.h"

bool ApplyObjc3CliOption(int &index,
                         int argc,
                         char **argv,
                         Objc3CliOptions &options,
                         std::string &error) {
  const std::string flag = argv[index];
  bool matched = false;
  if (!TryApplyObjc3CliPrimaryOptionGroup(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  if (objc3c::diagnostics::modes::BuildCanonicalModeRejectionDiagnostic(
          flag, error)) {
    return false;
  }
  if (!TryApplyObjc3CliExtendedOptionGroup(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }

  error = "unknown arg: " + flag;
  return false;
}
