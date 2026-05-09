#include "artifacts/objc3_runtime_state_publication_paths.h"

#include <string>

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

}  // namespace

RuntimeStatePublicationPaths BuildRuntimeStatePublicationPaths(
    std::string_view registration_manifest_artifact) {
  RuntimeStatePublicationPaths paths;
  paths.emit_prefix = BuildEmitPrefix(registration_manifest_artifact);
  paths.compile_manifest_artifact = paths.emit_prefix + ".manifest.json";
  paths.object_artifact = paths.emit_prefix + ".obj";
  paths.backend_artifact = paths.emit_prefix + ".ll";
  return paths;
}

}  // namespace objc3::artifacts::frontend
