#include "lower/contracts/lowering_ir_handoff.h"

#include <sstream>

namespace {

bool RequestedArtifactsHaveOwnedPaths(const Objc3LoweringArtifactPlan &plan) {
  return (!plan.emit_ir || !plan.ir_relative_path.empty()) &&
         (!plan.emit_object || !plan.object_relative_path.empty()) &&
         (!plan.emit_manifest || !plan.manifest_relative_path.empty()) &&
         (!plan.emit_runtime_metadata ||
          !plan.runtime_metadata_relative_path.empty());
}

bool StrictOwnerReady(const std::string &owner, const std::string &owner_model,
                      bool strict_no_fallback,
                      bool strict_no_compatibility) {
  return Objc3LoweringStrictOwnerModelIsReady(
      owner, owner_model, strict_no_fallback, strict_no_compatibility);
}

std::string FirstFailureReason(const Objc3LoweringIRHandoff &handoff) {
  if (!handoff.lower_handoff_owner_ready) {
    return "lowering-to-ir handoff owner is not explicit";
  }
  if (!handoff.ir_artifact_owner_ready) {
    return "ir module artifact owner is not explicit";
  }
  if (!handoff.runtime_dispatch_owner_ready) {
    return "runtime dispatch lowering owner is not explicit";
  }
  if (!handoff.runtime_dispatch_result_owner_ready) {
    return "runtime dispatch result owner is not explicit";
  }
  if (!handoff.artifact_publication_owner_ready) {
    return "lowering artifact publication owner is not explicit";
  }
  if (!handoff.canonical_runtime_dispatch_symbol) {
    return "runtime dispatch symbol is not the canonical hard-cutover entrypoint";
  }
  if (!handoff.fixed_runtime_dispatch_slots) {
    return "runtime dispatch argument slot count is outside the owned ABI range";
  }
  if (!handoff.requested_artifact_paths_owned) {
    return "requested lowering/ir artifact paths are not deterministic";
  }
  if (!handoff.phase_handoff_explicit) {
    return "lowering-to-ir phase handoff ownership is incomplete";
  }
  return {};
}

}  // namespace

Objc3LoweringIRHandoff Objc3BuildLoweringIRHandoff(
    const Objc3LoweringArtifactPlan &artifacts,
    const std::string &runtime_dispatch_symbol,
    std::size_t runtime_dispatch_arg_slots) {
  Objc3LoweringIRHandoff handoff;
  handoff.artifacts = artifacts;
  handoff.runtime_dispatch_symbol = runtime_dispatch_symbol;
  handoff.runtime_dispatch_arg_slots = runtime_dispatch_arg_slots;
  handoff.lower_handoff_owner_ready =
      StrictOwnerReady(handoff.lower_handoff_owner, handoff.owner_model,
                       handoff.strict_no_fallback,
                       handoff.strict_no_compatibility);
  handoff.ir_artifact_owner_ready =
      StrictOwnerReady(handoff.ir_artifact_owner, handoff.owner_model,
                       handoff.strict_no_fallback,
                       handoff.strict_no_compatibility);
  handoff.runtime_dispatch_owner_ready =
      StrictOwnerReady(handoff.runtime_dispatch_owner, handoff.owner_model,
                       handoff.strict_no_fallback,
                       handoff.strict_no_compatibility);
  handoff.runtime_dispatch_result_owner_ready =
      StrictOwnerReady(handoff.runtime_dispatch_result_owner, handoff.owner_model,
                       handoff.strict_no_fallback,
                       handoff.strict_no_compatibility);
  handoff.artifact_publication_owner_ready =
      Objc3LoweringArtifactPlanPublicationOwnerIsReady(artifacts);
  handoff.canonical_runtime_dispatch_symbol =
      handoff.runtime_dispatch_symbol ==
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  handoff.fixed_runtime_dispatch_slots =
      handoff.runtime_dispatch_arg_slots > 0u &&
      handoff.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs;
  handoff.requested_artifact_paths_owned =
      RequestedArtifactsHaveOwnedPaths(handoff.artifacts);
  handoff.phase_handoff_explicit =
      handoff.lower_handoff_owner_ready && handoff.ir_artifact_owner_ready &&
      handoff.runtime_dispatch_owner_ready &&
      handoff.runtime_dispatch_result_owner_ready &&
      handoff.artifact_publication_owner_ready &&
      handoff.strict_no_fallback && handoff.strict_no_compatibility;
  handoff.ready = Objc3LoweringIRHandoffIsReady(handoff);
  handoff.failure_reason = FirstFailureReason(handoff);
  handoff.replay_key = Objc3LoweringIRHandoffReplayKey(handoff);
  return handoff;
}

bool Objc3LoweringIRHandoffIsReady(const Objc3LoweringIRHandoff &handoff) {
  return handoff.lower_handoff_owner_ready && handoff.ir_artifact_owner_ready &&
         handoff.runtime_dispatch_owner_ready &&
         handoff.runtime_dispatch_result_owner_ready &&
         handoff.artifact_publication_owner_ready &&
         handoff.canonical_runtime_dispatch_symbol &&
         handoff.fixed_runtime_dispatch_slots &&
         handoff.requested_artifact_paths_owned &&
         handoff.phase_handoff_explicit &&
         handoff.strict_no_fallback && handoff.strict_no_compatibility;
}

std::string Objc3LoweringIRHandoffReplayKey(
    const Objc3LoweringIRHandoff &handoff) {
  std::ostringstream out;
  out << "ready=" << (Objc3LoweringIRHandoffIsReady(handoff) ? "true"
                                                             : "false")
      << ";lower_handoff_owner=" << handoff.lower_handoff_owner
      << ";ir_artifact_owner=" << handoff.ir_artifact_owner
      << ";runtime_dispatch_owner=" << handoff.runtime_dispatch_owner
      << ";runtime_dispatch_result_owner="
      << handoff.runtime_dispatch_result_owner
      << ";runtime_dispatch_symbol=" << handoff.runtime_dispatch_symbol
      << ";runtime_dispatch_arg_slots="
      << handoff.runtime_dispatch_arg_slots
      << ";canonical_runtime_dispatch_symbol="
      << (handoff.canonical_runtime_dispatch_symbol ? "true" : "false")
      << ";fixed_runtime_dispatch_slots="
      << (handoff.fixed_runtime_dispatch_slots ? "true" : "false")
      << ";requested_artifact_paths_owned="
      << (handoff.requested_artifact_paths_owned ? "true" : "false")
      << ";phase_handoff_explicit="
      << (handoff.phase_handoff_explicit ? "true" : "false")
      << ";artifact_publication="
      << Objc3LoweringArtifactPlanReplayKey(handoff.artifacts)
      << ";failure_reason=" << handoff.failure_reason << ";"
      << Objc3LoweringOwnerReplayKey(
             handoff.lower_handoff_owner, handoff.owner_model,
             handoff.strict_no_fallback, handoff.strict_no_compatibility);
  return out.str();
}
