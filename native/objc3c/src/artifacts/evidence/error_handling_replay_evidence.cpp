#include "artifacts/evidence/error_handling_replay_evidence.h"

#include <algorithm>
#include <sstream>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "lower/objc3_lowering_contract.h"

namespace objc3::artifacts::evidence {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

ErrorHandlingResultAndBridgingArtifactReplayEvidence
BuildErrorHandlingResultAndBridgingArtifactReplayEvidence(
    const std::string &error_handling_replay_key,
    const std::string &throws_replay_key,
    const std::string &result_like_replay_key,
    const std::string &ns_error_replay_key,
    const std::string &unwind_replay_key,
    bool deterministic_error_handling_handoff,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  ErrorHandlingResultAndBridgingArtifactReplayEvidence evidence;
  evidence.contract_id =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId;
  evidence.source_contract_id =
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId;
  evidence.surface_path =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplaySurfacePath;
  evidence.import_artifact_member_name =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName;
  evidence.source_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel;
  evidence.replay_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel;
  evidence.fail_closed_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel;
  evidence.error_handling_replay_key = error_handling_replay_key;
  evidence.throws_replay_key = throws_replay_key;
  evidence.result_like_replay_key = result_like_replay_key;
  evidence.ns_error_replay_key = ns_error_replay_key;
  evidence.unwind_replay_key = unwind_replay_key;
  evidence.binary_artifact_replay_ready =
      !error_handling_replay_key.empty() && !throws_replay_key.empty() &&
      !result_like_replay_key.empty() && !ns_error_replay_key.empty() &&
      !unwind_replay_key.empty();
  evidence.runtime_import_artifact_ready =
      runtime_import_artifact_ready && evidence.binary_artifact_replay_ready;
  evidence.deterministic = deterministic_error_handling_handoff;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.error_handling_result_and_bridging_artifact_replay_present) {
      continue;
    }
    evidence.imported_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
    evidence.imported_error_handling_replay_keys_lexicographic.push_back(
        surface.error_handling_error_handling_replay_key);
    evidence.imported_result_like_replay_keys_lexicographic.push_back(
        surface.error_handling_result_like_replay_key);
    evidence.imported_ns_error_replay_keys_lexicographic.push_back(
        surface.error_handling_ns_error_replay_key);
    evidence.deterministic =
        evidence.deterministic && surface.error_handling_deterministic;
  }
  std::sort(evidence.imported_module_names_lexicographic.begin(),
            evidence.imported_module_names_lexicographic.end());
  std::sort(
      evidence.imported_error_handling_replay_keys_lexicographic.begin(),
      evidence.imported_error_handling_replay_keys_lexicographic.end());
  std::sort(evidence.imported_result_like_replay_keys_lexicographic.begin(),
            evidence.imported_result_like_replay_keys_lexicographic.end());
  std::sort(evidence.imported_ns_error_replay_keys_lexicographic.begin(),
            evidence.imported_ns_error_replay_keys_lexicographic.end());
  const std::size_t imported_error_handling_module_count =
      std::count_if(imported_runtime_module_surfaces.begin(),
                    imported_runtime_module_surfaces.end(),
                    [](const Objc3ImportedRuntimeModuleSurface &surface) {
                      return surface
                          .error_handling_result_and_bridging_artifact_replay_present;
                    });
  evidence.separate_compilation_replay_ready =
      evidence.binary_artifact_replay_ready &&
      evidence.runtime_import_artifact_ready &&
      evidence.imported_module_names_lexicographic.size() ==
          imported_error_handling_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary()
             << ";binary_artifact_replay_ready="
             << (evidence.binary_artifact_replay_ready ? "true" : "false")
             << ";runtime_import_artifact_ready="
             << (evidence.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_replay_ready="
             << (evidence.separate_compilation_replay_ready ? "true" : "false")
             << ";imported_module_count="
             << evidence.imported_module_names_lexicographic.size()
             << ";deterministic="
             << (evidence.deterministic ? "true" : "false")
             << ";error_handling_replay_key=" << error_handling_replay_key
             << ";result_like_replay_key=" << result_like_replay_key
             << ";ns_error_replay_key=" << ns_error_replay_key;
  evidence.replay_key = replay_key.str();
  return evidence;
}

std::string BuildErrorHandlingResultAndBridgingArtifactReplayJson(
    const ErrorHandlingResultAndBridgingArtifactReplayEvidence &evidence) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(evidence.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(evidence.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(evidence.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(evidence.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(evidence.source_model)
      << "\",\"replay_model\":\"" << EscapeJsonString(evidence.replay_model)
      << "\",\"error_handling_replay_key\":\""
      << EscapeJsonString(evidence.error_handling_replay_key)
      << "\",\"throws_replay_key\":\""
      << EscapeJsonString(evidence.throws_replay_key)
      << "\",\"result_like_replay_key\":\""
      << EscapeJsonString(evidence.result_like_replay_key)
      << "\",\"ns_error_replay_key\":\""
      << EscapeJsonString(evidence.ns_error_replay_key)
      << "\",\"unwind_replay_key\":\""
      << EscapeJsonString(evidence.unwind_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << objc3::io::json::RenderJsonStringArray(
             evidence.imported_module_names_lexicographic)
      << ",\"imported_error_handling_replay_keys_lexicographic\":"
      << objc3::io::json::RenderJsonStringArray(
             evidence.imported_error_handling_replay_keys_lexicographic)
      << ",\"imported_result_like_replay_keys_lexicographic\":"
      << objc3::io::json::RenderJsonStringArray(
             evidence.imported_result_like_replay_keys_lexicographic)
      << ",\"imported_ns_error_replay_keys_lexicographic\":"
      << objc3::io::json::RenderJsonStringArray(
             evidence.imported_ns_error_replay_keys_lexicographic)
      << ",\"binary_artifact_replay_ready\":"
      << (evidence.binary_artifact_replay_ready ? "true" : "false")
      << ",\"runtime_import_artifact_ready\":"
      << (evidence.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_replay_ready\":"
      << (evidence.separate_compilation_replay_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (evidence.deterministic ? "true" : "false")
      << ",\"fail_closed_model\":\""
      << EscapeJsonString(evidence.fail_closed_model)
      << "\",\"replay_key\":\"" << EscapeJsonString(evidence.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::evidence
