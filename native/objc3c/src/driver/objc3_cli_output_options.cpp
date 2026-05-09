#include "driver/objc3_cli_output_options.h"

#include "driver/objc3_cli_option_reader.h"

bool TryApplyObjc3CliOutputOption(const std::string &flag,
                                  int &index,
                                  int argc,
                                  char **argv,
                                  Objc3CliOptions &options,
                                  std::string &error,
                                  bool &matched) {
  matched = true;
  std::string value;
  if (flag == "--out-dir") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.out_dir = value;
    return true;
  }
  if (flag == "--emit-prefix") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.emit_prefix = value;
    return true;
  }
  matched = false;
  return true;
}
