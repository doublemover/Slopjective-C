#include "artifacts/evidence/error_handling_replay_evidence.h"

#include <algorithm>
#include <sstream>

#include "io/json/json_writer.h"

namespace objc3::artifacts::evidence {
namespace {

inline constexpr const char
    *kObjc3ArtifactErrorHandlingThrowsAbiPropagationLoweringContractId =
        "objc3c.error_handling.throws.abi.propagation.lowering.v1";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayContractId =
        "objc3c.error_handling.result.and.bridging.artifact.replay.v1";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySourceModel =
        "error_handling-lowering-replay-keys-survive-object-emission-manifest-emission-and-emitted-sidecar-artifacts";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayModel =
        "provider-and-consumer-sidecar-artifacts-preserve-result-and-bridge-replay-packets-for-separate-compilation-proof";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayFailClosedModel =
        "missing-or-drifted-result-bridge-replay-sidecars-disable-separate-compilation-proof";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySurfacePath =
        "frontend.pipeline.semantic_surface.objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName =
        "objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char
    *kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix =
        ".error_handling-error-replay.json";

std::string BuildArtifactErrorHandlingResultAndBridgingReplaySummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayContractId
      << ";source_contract="
      << kObjc3ArtifactErrorHandlingThrowsAbiPropagationLoweringContractId
      << ";source_model="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySourceModel
      << ";replay_model="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayModel
      << ";surface_path="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySurfacePath
      << ";artifact_member="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName
      << ";artifact_suffix="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix
      << ";fail_closed_model="
      << kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayFailClosedModel
      << ";follow_on_surface=objc3c.errors.resultbridging.artifactsurface.v1";
  return out.str();
}

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
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayContractId;
  evidence.source_contract_id =
      kObjc3ArtifactErrorHandlingThrowsAbiPropagationLoweringContractId;
  evidence.surface_path =
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySurfacePath;
  evidence.import_artifact_member_name =
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName;
  evidence.source_model =
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplaySourceModel;
  evidence.replay_model =
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayModel;
  evidence.fail_closed_model =
      kObjc3ArtifactErrorHandlingResultAndBridgingArtifactReplayFailClosedModel;
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
  replay_key << BuildArtifactErrorHandlingResultAndBridgingReplaySummary()
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
  objc3::io::json::JsonObjectWriter object(out);
  object.StringField("contract_id", evidence.contract_id);
  object.StringField("source_contract_id", evidence.source_contract_id);
  object.StringField("surface_path", evidence.surface_path);
  object.StringField("import_artifact_member_name",
                     evidence.import_artifact_member_name);
  object.StringField("source_model", evidence.source_model);
  object.StringField("replay_model", evidence.replay_model);
  object.StringField("error_handling_replay_key",
                     evidence.error_handling_replay_key);
  object.StringField("throws_replay_key", evidence.throws_replay_key);
  object.StringField("result_like_replay_key", evidence.result_like_replay_key);
  object.StringField("ns_error_replay_key", evidence.ns_error_replay_key);
  object.StringField("unwind_replay_key", evidence.unwind_replay_key);
  object.StringArrayField("imported_module_names_lexicographic",
                          evidence.imported_module_names_lexicographic);
  object.StringArrayField(
      "imported_error_handling_replay_keys_lexicographic",
      evidence.imported_error_handling_replay_keys_lexicographic);
  object.StringArrayField("imported_result_like_replay_keys_lexicographic",
                          evidence
                              .imported_result_like_replay_keys_lexicographic);
  object.StringArrayField("imported_ns_error_replay_keys_lexicographic",
                          evidence.imported_ns_error_replay_keys_lexicographic);
  object.BoolField("binary_artifact_replay_ready",
                   evidence.binary_artifact_replay_ready);
  object.BoolField("runtime_import_artifact_ready",
                   evidence.runtime_import_artifact_ready);
  object.BoolField("separate_compilation_replay_ready",
                   evidence.separate_compilation_replay_ready);
  object.BoolField("deterministic", evidence.deterministic);
  object.StringField("fail_closed_model", evidence.fail_closed_model);
  object.StringField("replay_key", evidence.replay_key);
  object.End();
  return out.str();
}

}  // namespace objc3::artifacts::evidence
