#include "lower/contracts/lowering_backend_handoff.h"

#include <sstream>

namespace {

bool RequestedArtifactPathsAreDeterministic(
    const Objc3LoweringBackendHandoff &handoff) {
  if (handoff.emit_prefix.empty()) {
    return false;
  }
  if (handoff.ir_requested && handoff.ir_relative_path.empty()) {
    return false;
  }
  if (handoff.object_requested && handoff.object_relative_path.empty()) {
    return false;
  }
  if (handoff.manifest_requested && handoff.manifest_relative_path.empty()) {
    return false;
  }
  if (handoff.runtime_metadata_requested &&
      handoff.runtime_metadata_relative_path.empty()) {
    return false;
  }
  return true;
}

}  // namespace

Objc3LoweringBackendHandoff Objc3BuildLoweringBackendHandoff(
    const Objc3LoweringArtifactPlan &plan) {
  Objc3LoweringBackendHandoff handoff;
  handoff.ir_requested = plan.emit_ir;
  handoff.object_requested = plan.emit_object;
  handoff.manifest_requested = plan.emit_manifest;
  handoff.runtime_metadata_requested = plan.emit_runtime_metadata;
  handoff.output_directory = plan.output_directory;
  handoff.emit_prefix = plan.emit_prefix;
  handoff.ir_relative_path = plan.ir_relative_path;
  handoff.object_relative_path = plan.object_relative_path;
  handoff.manifest_relative_path = plan.manifest_relative_path;
  handoff.runtime_metadata_relative_path = plan.runtime_metadata_relative_path;
  handoff.deterministic_paths =
      RequestedArtifactPathsAreDeterministic(handoff);
  handoff.object_backend_route_ready =
      !handoff.object_requested || !handoff.object_relative_path.empty();
  handoff.artifact_publication_route_ready =
      (!handoff.ir_requested || !handoff.ir_relative_path.empty()) &&
      Objc3LoweringArtifactPlanPublicationOwnerIsReady(plan);
  handoff.owner_split_explicit = Objc3LoweringStrictOwnerModelIsReady(
      handoff.backend_handoff_owner,
      handoff.backend_handoff_owner_model,
      handoff.strict_no_retired_route,
      handoff.strict_no_compatibility);
  handoff.replay_key = Objc3LoweringBackendHandoffReplayKey(handoff);
  return handoff;
}

bool Objc3LoweringBackendHandoffIsReady(
    const Objc3LoweringBackendHandoff &handoff) {
  return handoff.deterministic_paths && handoff.object_backend_route_ready &&
         handoff.artifact_publication_route_ready &&
         handoff.owner_split_explicit &&
         Objc3LoweringStrictOwnerModelIsReady(
             handoff.backend_handoff_owner,
             handoff.backend_handoff_owner_model,
             handoff.strict_no_retired_route,
             handoff.strict_no_compatibility);
}

std::string Objc3LoweringBackendHandoffReplayKey(
    const Objc3LoweringBackendHandoff &handoff) {
  std::ostringstream out;
  out << "ready=" << (Objc3LoweringBackendHandoffIsReady(handoff) ? "true"
                                                                  : "false")
      << ";ir_requested=" << (handoff.ir_requested ? "true" : "false")
      << ";object_requested=" << (handoff.object_requested ? "true" : "false")
      << ";manifest_requested="
      << (handoff.manifest_requested ? "true" : "false")
      << ";runtime_metadata_requested="
      << (handoff.runtime_metadata_requested ? "true" : "false")
      << ";deterministic_paths="
      << (handoff.deterministic_paths ? "true" : "false")
      << ";object_backend_route_ready="
      << (handoff.object_backend_route_ready ? "true" : "false")
      << ";artifact_publication_route_ready="
      << (handoff.artifact_publication_route_ready ? "true" : "false")
      << ";owner_split_explicit="
      << (handoff.owner_split_explicit ? "true" : "false")
      << ";out=" << handoff.output_directory
      << ";prefix=" << handoff.emit_prefix
      << ";ir_path=" << handoff.ir_relative_path
      << ";object_path=" << handoff.object_relative_path
      << ";manifest_path=" << handoff.manifest_relative_path
      << ";runtime_metadata_path=" << handoff.runtime_metadata_relative_path
      << ";"
      << Objc3LoweringOwnerReplayKey(
             handoff.backend_handoff_owner,
             handoff.backend_handoff_owner_model,
             handoff.strict_no_retired_route,
             handoff.strict_no_compatibility);
  return out.str();
}
