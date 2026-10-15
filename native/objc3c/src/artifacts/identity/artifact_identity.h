#pragma once

#include <filesystem>
#include <string>
#include <string_view>

// Literal macros keep host-specific artifact names constexpr for contract
// headers; they are private to this header and undefined at the end.
#include "lower/contracts/native_artifact_names.h"

namespace objc3::artifacts::identity {

struct Objc3NativeArtifactIdentity {
  std::string platform_id;
  std::string host_promotion_state;
  std::string target_triple;
  std::string object_file_extension;
  std::string object_format;
  std::string debug_format;
  std::string runtime_library_kind;
  std::string native_executable_relative_path;
  std::string frontend_runner_relative_path;
  std::string runtime_library_relative_path;
  std::string runtime_library_name;
};

struct Objc3TranslationUnitIdentityEvidence {
  std::filesystem::path input_path;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
};

[[nodiscard]] Objc3NativeArtifactIdentity
BuildObjc3NativeArtifactIdentityForHost();

[[nodiscard]] inline std::string BuildObjc3NativeFrontendRunnerProbeCommand(
    std::string_view input_path,
    std::string_view out_dir,
    std::string_view emit_prefix,
    bool suppress_ir,
    bool suppress_object) {
  if (input_path.empty() || out_dir.empty() || emit_prefix.empty()) {
    return {};
  }

  std::string command = std::string(kObjc3NativeFrontendRunnerRelativePath) +
                        " " + std::string(input_path) + " --out-dir " +
                        std::string(out_dir) + " --emit-prefix " +
                        std::string(emit_prefix);
  if (suppress_ir) {
    command += " --no-emit-ir";
  }
  if (suppress_object) {
    command += " --no-emit-object";
  }
  return command;
}

[[nodiscard]] std::string BuildObjc3TranslationUnitIdentityKey(
    const Objc3TranslationUnitIdentityEvidence &evidence);

}  // namespace objc3::artifacts::identity
