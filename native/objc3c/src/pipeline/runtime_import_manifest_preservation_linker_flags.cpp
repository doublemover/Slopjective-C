#include "pipeline/runtime_import_manifest_preservation.h"

namespace objc3c::pipeline {

bool ValidateImportedRuntimeRegistrationManifestLinkerFlags(
    const std::vector<std::string> &driver_linker_flags,
    std::string &error) {
  for (const auto &flag : driver_linker_flags) {
    if (flag.empty()) {
      error =
          "runtime registration manifest contains an empty driver linker flag";
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::pipeline
