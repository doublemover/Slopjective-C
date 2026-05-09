#include "driver/objc3_cli_conformance_options.h"

#include "driver/objc3_cli_conformance_profile.h"
#include "driver/objc3_cli_option_reader.h"

bool TryApplyObjc3CliConformanceOption(const std::string &flag,
                                       int &index,
                                       int argc,
                                       char **argv,
                                       Objc3CliOptions &options,
                                       std::string &error,
                                       bool &matched) {
  matched = true;
  std::string value;
  if (flag == "--objc3-conformance-profile") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (!ParseObjc3ConformanceProfile(value, options.conformance_profile)) {
      error =
          "invalid --objc3-conformance-profile (expected core|strict|strict-concurrency|strict-system): " +
          value;
      return false;
    }
    return true;
  }
  if (flag == "--emit-objc3-conformance") {
    options.emit_objc3_conformance = true;
    return true;
  }
  if (flag == "--emit-objc3-conformance-format") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.emit_objc3_conformance_format = value;
    return true;
  }
  if (flag == "--validate-objc3-conformance") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.command_mode = Objc3CliCommandMode::kValidateConformance;
    options.validate_conformance_report_path = value;
    return true;
  }
  matched = false;
  return true;
}
