#include "driver/objc3_cli_artifact_options_contract.h"

#include <cctype>

namespace {

bool IsObjc3ArtifactStemChar(char value) {
  const unsigned char c = static_cast<unsigned char>(value);
  return std::isalnum(c) != 0 || value == '_' || value == '-' || value == '.';
}

}  // namespace

bool ValidateObjc3CliArtifactOptions(const Objc3CliOptions &options,
                                     std::string &error) {
  if (options.out_dir.empty()) {
    error = "invalid --out-dir (expected non-empty directory path)";
    return false;
  }

  if (options.emit_prefix.empty()) {
    error = "invalid --emit-prefix (expected non-empty artifact filename stem)";
    return false;
  }

  if (options.emit_prefix == "." || options.emit_prefix == "..") {
    error =
        "invalid --emit-prefix (artifact filename stem cannot be . or ..): " +
        options.emit_prefix;
    return false;
  }

  for (const char value : options.emit_prefix) {
    if (!IsObjc3ArtifactStemChar(value)) {
      error =
          "invalid --emit-prefix (expected artifact filename stem [A-Za-z0-9._-]+): " +
          options.emit_prefix;
      return false;
    }
  }

  return true;
}
