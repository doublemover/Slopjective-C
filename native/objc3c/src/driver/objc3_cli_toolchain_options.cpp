#include "driver/objc3_cli_toolchain_options.h"

#include "driver/objc3_cli_ir_backend_options.h"
#include "driver/objc3_cli_option_reader.h"

bool TryApplyObjc3CliToolchainOption(const std::string &flag,
                                     int &index,
                                     int argc,
                                     char **argv,
                                     Objc3CliOptions &options,
                                     std::string &error,
                                     bool &matched) {
  matched = true;
  std::string value;
  if (flag == "--clang") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.clang_path = value;
    options.clang_path_explicit = true;
    return true;
  }
  if (flag == "--llc") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.llc_path = value;
    options.llc_path_explicit = true;
    return true;
  }
  if (flag == "--objc3-ir-object-backend") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (!ParseObjc3CliIrObjectBackend(value, options.ir_object_backend)) {
      error =
          "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " +
          value;
      return false;
    }
    return true;
  }
  if (flag == "--llvm-capabilities-summary") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.llvm_capabilities_summary = value;
    return true;
  }
  if (flag == "--objc3-route-backend-from-capabilities") {
    options.route_backend_from_capabilities = true;
    return true;
  }
  matched = false;
  return true;
}
