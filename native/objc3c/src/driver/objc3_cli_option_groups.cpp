#include "driver/objc3_cli_option_groups.h"

#include "driver/objc3_cli_conformance_options.h"
#include "driver/objc3_cli_language_options.h"
#include "driver/objc3_cli_output_options.h"
#include "driver/objc3_cli_runtime_options.h"
#include "driver/objc3_cli_toolchain_options.h"

bool TryApplyObjc3CliPrimaryOptionGroup(const std::string &flag,
                                        int &index,
                                        int argc,
                                        char **argv,
                                        Objc3CliOptions &options,
                                        std::string &error,
                                        bool &matched) {
  if (!TryApplyObjc3CliLanguageOption(
          flag, index, argc, argv, options, error, matched)) {
    return false;
  }
  if (matched) {
    return true;
  }
  return TryApplyObjc3CliOutputOption(
      flag, index, argc, argv, options, error, matched);
}

bool TryApplyObjc3CliExtendedOptionGroup(const std::string &flag,
                                         int &index,
                                         int argc,
                                         char **argv,
                                         Objc3CliOptions &options,
                                         std::string &error,
                                         bool &matched) {
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
  return TryApplyObjc3CliToolchainOption(
      flag, index, argc, argv, options, error, matched);
}
