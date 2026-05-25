#include "artifacts/identity/artifact_identity.h"

#include <string>
#include <utility>

namespace objc3::artifacts::identity {
namespace {

Objc3NativeArtifactIdentity BuildNativeArtifactIdentity(
    std::string platform_id,
    std::string host_promotion_state,
    std::string target_triple,
    std::string object_format,
    std::string debug_format,
    std::string runtime_library_kind = kObjc3NativeRuntimeLibraryKind) {
  Objc3NativeArtifactIdentity identity;
  identity.platform_id = std::move(platform_id);
  identity.host_promotion_state = std::move(host_promotion_state);
  identity.target_triple = std::move(target_triple);
  identity.object_file_extension = kObjc3NativeObjectFileExtension;
  identity.object_format = std::move(object_format);
  identity.debug_format = std::move(debug_format);
  identity.runtime_library_kind = std::move(runtime_library_kind);
  identity.native_executable_relative_path = kObjc3NativeExecutableRelativePath;
  identity.frontend_runner_relative_path = kObjc3NativeFrontendRunnerRelativePath;
  identity.runtime_library_relative_path = kObjc3NativeRuntimeLibraryRelativePath;
  identity.runtime_library_name = kObjc3NativeRuntimeLibraryName;
  return identity;
}

}  // namespace

Objc3NativeArtifactIdentity BuildObjc3NativeArtifactIdentityForHost() {
#if defined(_WIN32)
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
  return BuildNativeArtifactIdentity("windows-x64",
                                     "supported-boundary",
                                     "x86_64-pc-windows-msvc",
                                     "COFF",
                                     "CodeView/PDB");
#elif defined(_M_ARM64) || defined(__aarch64__)
  return BuildNativeArtifactIdentity("windows-arm64",
                                     "unsupported-host",
                                     "aarch64-pc-windows-msvc",
                                     "COFF",
                                     "CodeView/PDB");
#else
  return BuildNativeArtifactIdentity("windows-unsupported",
                                     "unsupported-host",
                                     "unsupported-pc-windows-msvc",
                                     "COFF",
                                     "CodeView/PDB");
#endif
#elif defined(__linux__)
#if defined(__x86_64__) || defined(__amd64__)
  return BuildNativeArtifactIdentity("linux-x64",
                                     "fail-closed-until-native-host-evidence",
                                     "x86_64-unknown-linux-gnu",
                                     "ELF",
                                     "DWARF");
#elif defined(__aarch64__) || defined(__arm64__)
  return BuildNativeArtifactIdentity("linux-arm64",
                                     "unsupported-host",
                                     "aarch64-unknown-linux-gnu",
                                     "ELF",
                                     "DWARF");
#else
  return BuildNativeArtifactIdentity("linux-unsupported",
                                     "unsupported-host",
                                     "unsupported-unknown-linux-gnu",
                                     "unknown",
                                     "unknown");
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#if defined(__aarch64__) || defined(__arm64__)
  return BuildNativeArtifactIdentity("darwin-arm64",
                                     "fail-closed-until-native-host-evidence",
                                     "aarch64-apple-darwin",
                                     "Mach-O",
                                     "DWARF/dSYM");
#elif defined(__x86_64__) || defined(__amd64__)
  return BuildNativeArtifactIdentity("darwin-x64",
                                     "unsupported-host",
                                     "x86_64-apple-darwin",
                                     "Mach-O",
                                     "DWARF/dSYM");
#else
  return BuildNativeArtifactIdentity("darwin-unsupported",
                                     "unsupported-host",
                                     "unsupported-apple-darwin",
                                     "unknown",
                                     "unknown");
#endif
#else
  return BuildNativeArtifactIdentity("unsupported-host",
                                     "unsupported-host",
                                     "unsupported-host",
                                     "unknown",
                                     "unknown");
#endif
}

std::string BuildObjc3NativeArtifactName(
    std::string_view stem,
    std::string_view suffix) {
  if (stem.empty()) {
    return {};
  }
  return std::string(stem) + std::string(suffix);
}

std::string BuildObjc3NativeObjectArtifactName(std::string_view stem) {
  return BuildObjc3NativeArtifactName(stem, kObjc3NativeObjectFileExtension);
}

std::string BuildObjc3TranslationUnitIdentityKey(
    const Objc3TranslationUnitIdentityEvidence &evidence) {
  return evidence.input_path.generic_string() + "|" +
         evidence.parse_artifact_replay_key + "|" +
         evidence.lowering_boundary_replay_key;
}

}  // namespace objc3::artifacts::identity
