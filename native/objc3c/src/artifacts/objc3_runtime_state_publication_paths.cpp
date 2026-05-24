#include "artifacts/objc3_runtime_state_publication_paths.h"

#include <string>
#include <utility>

#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {
namespace {

RuntimeStateObjectDebugIdentity BuildUnsupportedHostIdentity(
    std::string platform_id,
    std::string host_os,
    std::string host_arch) {
  RuntimeStateObjectDebugIdentity identity;
  identity.platform_id = std::move(platform_id);
  identity.host_os = std::move(host_os);
  identity.host_arch = std::move(host_arch);
  identity.target_triple.clear();
  identity.object_format.clear();
  identity.package_object_format.clear();
  identity.debug_format.clear();
  identity.object_file_extension.clear();
  identity.host_promotion_state = "unsupported-host";
  identity.unsupported_host_behavior =
      kObjc3RuntimeStateObjectArtifactPublicationFailureBehavior;
  identity.platform_identity_known = false;
  identity.object_artifact_publication_supported = false;
  identity.object_emission_alone_supports_platform = false;
  return identity;
}

std::string BuildEmitPrefix(std::string_view registration_manifest_artifact) {
  constexpr std::string_view suffix =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix;
  if (registration_manifest_artifact.ends_with(suffix)) {
    return std::string(registration_manifest_artifact.substr(
        0, registration_manifest_artifact.size() - suffix.size()));
  }
  return "module";
}

}  // namespace

RuntimeStateObjectDebugIdentity BuildRuntimeStateObjectDebugIdentityForHost() {
#if defined(_WIN32) && \
    (defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__))
  RuntimeStateObjectDebugIdentity identity;
  return identity;
#elif defined(__linux__) && (defined(__x86_64__) || defined(__amd64__))
  RuntimeStateObjectDebugIdentity identity;
  identity.platform_id = "linux-x64";
  identity.host_os = "linux";
  identity.host_arch = "x64";
  identity.target_triple = "x86_64-unknown-linux-gnu";
  identity.object_format = "elf";
  identity.package_object_format = "ELF";
  identity.debug_format = "DWARF";
  identity.object_file_extension = ".o";
  identity.host_promotion_state = "fail-closed-until-native-host-evidence";
  identity.unsupported_host_behavior =
      "fail-closed-before-package-install-native-execution-and-support-promotion";
  return identity;
#elif defined(__APPLE__) && defined(__MACH__) && \
    (defined(__aarch64__) || defined(__arm64__))
  RuntimeStateObjectDebugIdentity identity;
  identity.platform_id = "darwin-arm64";
  identity.host_os = "darwin";
  identity.host_arch = "arm64";
  identity.target_triple = "aarch64-apple-darwin";
  identity.object_format = "mach-o";
  identity.package_object_format = "Mach-O";
  identity.debug_format = "DWARF/dSYM";
  identity.object_file_extension = ".o";
  identity.host_promotion_state = "fail-closed-until-native-host-evidence";
  identity.unsupported_host_behavior =
      "fail-closed-before-package-install-native-execution-and-support-promotion";
  return identity;
#elif defined(_WIN32)
  return BuildUnsupportedHostIdentity("windows-unsupported-arch", "windows",
                                      "unsupported");
#elif defined(__linux__)
  return BuildUnsupportedHostIdentity("linux-unsupported-arch", "linux",
                                      "unsupported");
#elif defined(__APPLE__) && defined(__MACH__)
  return BuildUnsupportedHostIdentity("darwin-unsupported-arch", "darwin",
                                      "unsupported");
#else
  return BuildUnsupportedHostIdentity("unsupported-host", "unsupported",
                                      "unsupported");
#endif
}

bool IsRuntimeStateObjectDebugIdentityReady(
    const RuntimeStateObjectDebugIdentity &identity,
    std::string &reason) {
  if (identity.contract_id.empty()) {
    reason = "runtime state object/debug identity contract id is empty";
    return false;
  }
  if (identity.platform_id.empty() || identity.host_os.empty() ||
      identity.host_arch.empty()) {
    reason = "runtime state host platform identity is incomplete";
    return false;
  }
  if (!identity.platform_identity_known) {
    reason = "runtime state host platform identity is unsupported";
    return false;
  }
  if (!identity.object_artifact_publication_supported) {
    reason = identity.unsupported_host_behavior.empty()
                 ? kObjc3RuntimeStateObjectArtifactPublicationFailureBehavior
                 : identity.unsupported_host_behavior;
    return false;
  }
  if (identity.target_triple.empty() || identity.object_format.empty() ||
      identity.package_object_format.empty() || identity.debug_format.empty() ||
      identity.object_file_extension.empty()) {
    reason = "runtime state object/debug artifact identity is incomplete";
    return false;
  }
  if (identity.object_file_extension != ".obj" &&
      identity.object_file_extension != ".o") {
    reason = "runtime state object artifact extension is unsupported";
    return false;
  }
  if (identity.object_emission_alone_supports_platform) {
    reason = "object emission alone must not promote platform support";
    return false;
  }
  reason.clear();
  return true;
}

std::string BuildRuntimeStateObjectArtifactName(
    std::string_view emit_prefix,
    const RuntimeStateObjectDebugIdentity &identity) {
  if (emit_prefix.empty() || identity.object_file_extension.empty()) {
    return {};
  }
  return std::string(emit_prefix) + identity.object_file_extension;
}

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPathsForEmitPrefix(
    std::string_view emit_prefix) {
  RuntimeStatePublicationPaths paths;
  paths.emit_prefix = emit_prefix.empty() ? "module" : std::string(emit_prefix);
  paths.compile_manifest_artifact = paths.emit_prefix + ".manifest.json";
  paths.object_debug_identity = BuildRuntimeStateObjectDebugIdentityForHost();
  paths.object_artifact = BuildRuntimeStateObjectArtifactName(
      paths.emit_prefix, paths.object_debug_identity);
  paths.backend_artifact = paths.emit_prefix + ".ll";
  return paths;
}

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPaths(
    std::string_view registration_manifest_artifact) {
  return BuildRuntimeStatePublicationPathsForEmitPrefix(
      BuildEmitPrefix(registration_manifest_artifact));
}

}  // namespace objc3::artifacts::frontend
