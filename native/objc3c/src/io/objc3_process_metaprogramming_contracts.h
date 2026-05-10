#pragma once

#include <filesystem>
#include <string>

struct Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs {
  std::string contract_id;
  std::string source_contract_id;
  std::string surface_path;
  std::string artifact_relative_path;
  std::string host_executable_relative_path;
  std::string cache_root_relative_path;
  std::string host_model;
  std::string toolchain_model;
  std::string cache_model;
  std::string fail_closed_model;
  std::string replay_key;
  bool deterministic = false;
};

bool TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error);
