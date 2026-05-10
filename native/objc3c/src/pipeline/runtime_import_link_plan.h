#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace objc3c::pipeline {

struct Objc3ImportedRuntimeModuleLinkPlan {
  std::filesystem::path import_surface_path;
  std::filesystem::path import_surface_parent_path;
  std::string emit_prefix;
  std::filesystem::path registration_manifest_path;
  std::filesystem::path discovery_artifact_path;
  std::filesystem::path linker_response_artifact_path;
};

bool RuntimeImportPathEndsWith(const std::string &text,
                               const std::string &suffix);

bool TryResolveRuntimeImportEmitPrefixFromSurfacePath(
    const std::filesystem::path &path,
    std::string &emit_prefix,
    std::string &error);

bool BuildObjc3ImportedRuntimeModuleLinkPlan(
    const std::filesystem::path &import_surface_path,
    Objc3ImportedRuntimeModuleLinkPlan &plan,
    std::string &error);

std::vector<std::string> SplitRuntimeImportLinkerResponseFlags(
    const std::string &text);

}  // namespace objc3c::pipeline
