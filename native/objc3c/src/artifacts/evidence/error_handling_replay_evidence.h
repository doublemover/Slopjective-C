#pragma once

#include <string>
#include <vector>

#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3::artifacts::evidence {

struct ErrorHandlingResultAndBridgingArtifactReplayEvidence {
  std::string contract_id;
  std::string source_contract_id;
  std::string surface_path;
  std::string import_artifact_member_name;
  std::string source_model;
  std::string replay_model;
  std::string fail_closed_model;
  std::string replay_key;
  std::string error_handling_replay_key;
  std::string throws_replay_key;
  std::string result_like_replay_key;
  std::string ns_error_replay_key;
  std::string unwind_replay_key;
  std::vector<std::string> imported_module_names_lexicographic;
  std::vector<std::string> imported_error_handling_replay_keys_lexicographic;
  std::vector<std::string> imported_result_like_replay_keys_lexicographic;
  std::vector<std::string> imported_ns_error_replay_keys_lexicographic;
  bool binary_artifact_replay_ready = false;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_replay_ready = false;
  bool deterministic = false;
};

[[nodiscard]] ErrorHandlingResultAndBridgingArtifactReplayEvidence
BuildErrorHandlingResultAndBridgingArtifactReplayEvidence(
    const std::string &error_handling_replay_key,
    const std::string &throws_replay_key,
    const std::string &result_like_replay_key,
    const std::string &ns_error_replay_key,
    const std::string &unwind_replay_key,
    bool deterministic_error_handling_handoff,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces);

[[nodiscard]] std::string BuildErrorHandlingResultAndBridgingArtifactReplayJson(
    const ErrorHandlingResultAndBridgingArtifactReplayEvidence &evidence);

}  // namespace objc3::artifacts::evidence
