#pragma once

#include <filesystem>
#include <cstdint>
#include <string>
#include <vector>

struct Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs {
  std::string contract_id;
  std::string source_contract_id;
  std::string surface_path;
  std::string import_artifact_member_name;
  std::string artifact_relative_path;
  std::string host_executable_relative_path;
  std::string cache_root_relative_path;
  std::string host_model;
  std::string toolchain_model;
  std::string cache_model;
  std::string invalidation_model;
  std::string sandbox_policy_model;
  std::string diagnostics_model;
  std::string fail_closed_model;
  std::string replay_key;
  std::string runtime_dispatch_symbol;
  std::uint32_t max_message_send_args = 0;
  std::uint64_t bootstrap_registration_order_ordinal = 0;
  bool allow_live_error_runtime_surface = false;
  std::vector<std::filesystem::path> imported_runtime_surface_paths;
  bool deterministic = false;
};

bool TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error);
