#pragma once

#include <filesystem>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "io/objc3_artifact_paths.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline {

struct Objc3ImportedRuntimeModuleLinkPlan {
  std::filesystem::path import_surface_path;
  std::filesystem::path import_surface_parent_path;
  std::string emit_prefix;
  std::filesystem::path registration_manifest_path;
  std::filesystem::path discovery_artifact_path;
  std::filesystem::path linker_response_artifact_path;
};

inline bool RuntimeImportPathEndsWith(const std::string &text,
                                      const std::string &suffix) {
  return text.size() >= suffix.size() &&
         text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool TryResolveRuntimeImportEmitPrefixFromSurfacePath(
    const std::filesystem::path &path,
    std::string &emit_prefix,
    std::string &error) {
  const std::string filename = path.filename().generic_string();
  const std::string suffix =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix;
  if (!RuntimeImportPathEndsWith(filename, suffix) ||
      filename.size() <= suffix.size()) {
    error = "import surface path does not end with the canonical artifact suffix";
    return false;
  }
  emit_prefix = filename.substr(0, filename.size() - suffix.size());
  if (emit_prefix.empty()) {
    error = "import surface path does not contain an emit prefix";
    return false;
  }
  return true;
}

inline bool BuildObjc3ImportedRuntimeModuleLinkPlan(
    const std::filesystem::path &import_surface_path,
    Objc3ImportedRuntimeModuleLinkPlan &plan,
    std::string &error) {
  plan = Objc3ImportedRuntimeModuleLinkPlan{};
  plan.import_surface_path = import_surface_path;
  if (!TryResolveRuntimeImportEmitPrefixFromSurfacePath(
          import_surface_path, plan.emit_prefix, error)) {
    error = import_surface_path.generic_string() + ": " + error;
    return false;
  }

  plan.import_surface_parent_path = import_surface_path.parent_path();
  plan.registration_manifest_path =
      BuildRuntimeRegistrationManifestArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  plan.discovery_artifact_path =
      BuildRuntimeMetadataDiscoveryArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  plan.linker_response_artifact_path =
      BuildRuntimeMetadataLinkerResponseArtifactPath(
          plan.import_surface_parent_path, plan.emit_prefix)
          .lexically_normal();
  return true;
}

inline std::vector<std::string> SplitRuntimeImportLinkerResponseFlags(
    const std::string &text) {
  std::vector<std::string> flags;
  std::istringstream input(text);
  for (std::string line; std::getline(input, line);) {
    if (!line.empty()) {
      flags.push_back(std::move(line));
    }
  }
  return flags;
}

}  // namespace objc3c::pipeline
