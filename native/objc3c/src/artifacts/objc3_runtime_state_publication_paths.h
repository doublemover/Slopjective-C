#pragma once

#include <string>
#include <string_view>

#include "artifacts/identity/artifact_identity.h"

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kObjc3RuntimeStateObjectDebugIdentitySurfaceContractId =
        "objc3c.runtime.state.object.debug.identity.surface.v1";
inline constexpr const char
    *kObjc3RuntimeStateObjectArtifactPublicationFailureBehavior =
        "fail-closed-before-object-artifact-publication";
inline constexpr const char
    *kObjc3RuntimeStateObjectEmissionSupportTruthPolicy =
        "object-emission-alone-does-not-promote-platform-support";

struct RuntimeStateObjectDebugIdentity {
  std::string contract_id =
      kObjc3RuntimeStateObjectDebugIdentitySurfaceContractId;
  std::string platform_id = "windows-x64";
  std::string host_os = "windows";
  std::string host_arch = "x64";
  std::string target_triple = "x86_64-pc-windows-msvc";
  std::string object_format = "COFF";
  std::string package_object_format = "COFF";
  std::string debug_format = "CodeView/PDB";
  std::string object_file_extension =
      objc3::artifacts::identity::kObjc3NativeObjectFileExtension;
  std::string host_promotion_state = "supported-boundary";
  std::string wrong_format_behavior = "fail-closed-before-package-publication";
  std::string wrong_arch_behavior = "fail-closed-before-install";
  std::string support_truth_policy =
      kObjc3RuntimeStateObjectEmissionSupportTruthPolicy;
  std::string unsupported_host_behavior;
  bool platform_identity_known = true;
  bool object_artifact_publication_supported = true;
  bool object_emission_alone_supports_platform = false;
};

struct RuntimeStatePublicationPaths {
  std::string emit_prefix =
      objc3::artifacts::identity::kObjc3NativeDefaultArtifactStem;
  std::string compile_manifest_artifact =
      objc3::artifacts::identity::kObjc3NativeDefaultManifestArtifactName;
  std::string object_artifact =
      objc3::artifacts::identity::kObjc3NativeDefaultObjectArtifactName;
  std::string backend_artifact =
      objc3::artifacts::identity::kObjc3NativeDefaultIrArtifactName;
  RuntimeStateObjectDebugIdentity object_debug_identity;
};

RuntimeStateObjectDebugIdentity BuildRuntimeStateObjectDebugIdentityForHost();

bool IsRuntimeStateObjectDebugIdentityReady(
    const RuntimeStateObjectDebugIdentity &identity,
    std::string &reason);

std::string BuildRuntimeStateObjectArtifactName(
    std::string_view emit_prefix,
    const RuntimeStateObjectDebugIdentity &identity);

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPathsForEmitPrefix(
    std::string_view emit_prefix);

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPaths(
    std::string_view registration_manifest_artifact);

}  // namespace objc3::artifacts::frontend
