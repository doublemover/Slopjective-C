#include "driver/objc3_driver_shell.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

bool ValidateObjc3DriverShellInputs(const Objc3CliOptions &cli_options,
                                    Objc3DriverInputKind input_kind,
                                    std::string &error) {
  if (!fs::exists(cli_options.input)) {
    error = "input file not found: " + cli_options.input.string();
    return false;
  }

  if (NeedsObjc3DriverClangPath(input_kind, cli_options.ir_object_backend) && cli_options.clang_path.has_root_path() &&
      !fs::exists(cli_options.clang_path)) {
    error = "clang executable not found: " + cli_options.clang_path.string();
    return false;
  }

  if (NeedsObjc3DriverLlcPath(input_kind, cli_options.ir_object_backend) && cli_options.llc_path.has_root_path() &&
      !fs::exists(cli_options.llc_path)) {
    error = "llc executable not found: " + cli_options.llc_path.string();
    return false;
  }

  return true;
}
