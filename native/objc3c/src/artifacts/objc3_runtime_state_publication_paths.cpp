#include "artifacts/objc3_runtime_state_publication_paths.h"

#include <string>
#include <utility>

#include "ast/objc3_ast_contracts.h"

namespace objc3::artifacts::frontend {
namespace {

std::string BuildEmitPrefix(std::string_view registration_manifest_artifact) {
  constexpr std::string_view suffix =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix;
  if (registration_manifest_artifact.ends_with(suffix)) {
    return std::string(registration_manifest_artifact.substr(
        0, registration_manifest_artifact.size() - suffix.size()));
  }
  return "module";
}

std::pair<std::string, std::string> SplitPlatformId(std::string_view platform_id) {
  const std::size_t dash = platform_id.find('-');
  if (dash == std::string_view::npos) {
    return {std::string(platform_id), "unknown"};
  }
  return {std::string(platform_id.substr(0, dash)),
          std::string(platform_id.substr(dash + 1))};
}

}  // namespace

RuntimeStateObjectDebugIdentity BuildRuntimeStateObjectDebugIdentityForHost() {
  const objc3::artifacts::identity::Objc3NativeArtifactIdentity native_identity =
      objc3::artifacts::identity::BuildObjc3NativeArtifactIdentityForHost();
  const auto [host_os, host_arch] = SplitPlatformId(native_identity.platform_id);
  const bool platform_supported =
      native_identity.host_promotion_state == "supported-boundary" ||
      native_identity.host_promotion_state ==
          "fail-closed-until-native-host-evidence";

  RuntimeStateObjectDebugIdentity identity;
  identity.platform_id = native_identity.platform_id;
  identity.host_os = host_os;
  identity.host_arch = host_arch;
  identity.target_triple = native_identity.target_triple;
  identity.object_format = native_identity.object_format;
  identity.package_object_format = native_identity.object_format;
  identity.debug_format = native_identity.debug_format;
  identity.object_file_extension = native_identity.object_file_extension;
  identity.host_promotion_state = native_identity.host_promotion_state;
  identity.platform_identity_known = platform_supported;
  identity.object_artifact_publication_supported = platform_supported;
  if (!platform_supported) {
    identity.unsupported_host_behavior =
        kObjc3RuntimeStateObjectArtifactPublicationFailureBehavior;
  } else if (native_identity.host_promotion_state ==
             "fail-closed-until-native-host-evidence") {
    identity.unsupported_host_behavior =
        "fail-closed-before-package-install-native-execution-and-support-promotion";
  }
  return identity;
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
  if (identity.object_file_extension.empty()) {
    return {};
  }
  return objc3::artifacts::identity::BuildObjc3NativeArtifactName(
      emit_prefix, identity.object_file_extension);
}

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPathsForEmitPrefix(
    std::string_view emit_prefix) {
  RuntimeStatePublicationPaths paths;
  paths.emit_prefix =
      emit_prefix.empty()
          ? objc3::artifacts::identity::kObjc3NativeDefaultArtifactStem
          : std::string(emit_prefix);
  paths.compile_manifest_artifact =
      objc3::artifacts::identity::BuildObjc3NativeArtifactName(
          paths.emit_prefix,
          objc3::artifacts::identity::kObjc3NativeManifestArtifactSuffix);
  paths.object_debug_identity = BuildRuntimeStateObjectDebugIdentityForHost();
  paths.object_artifact = BuildRuntimeStateObjectArtifactName(
      paths.emit_prefix, paths.object_debug_identity);
  paths.backend_artifact =
      objc3::artifacts::identity::BuildObjc3NativeArtifactName(
          paths.emit_prefix,
          objc3::artifacts::identity::kObjc3NativeIrArtifactSuffix);
  return paths;
}

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPaths(
    std::string_view registration_manifest_artifact) {
  return BuildRuntimeStatePublicationPathsForEmitPrefix(
      BuildEmitPrefix(registration_manifest_artifact));
}

}  // namespace objc3::artifacts::frontend
