#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3LoweringRuntimeStabilityInvariantScaffoldKey(
    const Objc3LoweringRuntimeStabilityInvariantScaffold &scaffold) {
  std::ostringstream key;
  key << "lowering-runtime-stability-invariant-scaffold:v1:"
      << "typed_surface_present=" << (scaffold.typed_surface_present ? "true" : "false")
      << ";parse_readiness_surface_present="
      << (scaffold.parse_readiness_surface_present ? "true" : "false")
      << ";lowering_boundary_ready=" << (scaffold.lowering_boundary_ready ? "true" : "false")
      << ";runtime_dispatch_contract_consistent="
      << (scaffold.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";typed_handoff_key_deterministic="
      << (scaffold.typed_handoff_key_deterministic ? "true" : "false")
      << ";typed_core_feature_consistent=" << (scaffold.typed_core_feature_consistent ? "true" : "false")
      << ";parse_ready_for_lowering=" << (scaffold.parse_ready_for_lowering ? "true" : "false")
      << ";invariant_proofs_ready=" << (scaffold.invariant_proofs_ready ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3LoweringRuntimeStabilityInvariantScaffold BuildObjc3LoweringRuntimeStabilityInvariantScaffold(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3LoweringRuntimeStabilityInvariantScaffold scaffold;
  scaffold.typed_surface_present =
      !typed_surface.typed_handoff_key.empty() ||
      typed_surface.ready_for_lowering ||
      !typed_surface.failure_reason.empty();
  scaffold.parse_readiness_surface_present =
      !parse_surface.parse_artifact_replay_key.empty() ||
      parse_surface.ready_for_lowering ||
      !parse_surface.failure_reason.empty();
  scaffold.lowering_boundary_ready =
      typed_surface.lowering_boundary_ready &&
      parse_surface.lowering_boundary_ready;
  scaffold.runtime_dispatch_contract_consistent =
      typed_surface.runtime_dispatch_contract_consistent;
  scaffold.typed_handoff_key_deterministic =
      typed_surface.typed_handoff_key_deterministic &&
      parse_surface.typed_handoff_key_deterministic;
  scaffold.typed_core_feature_consistent =
      typed_surface.typed_core_feature_consistent &&
      parse_surface.typed_sema_core_feature_consistent;
  scaffold.parse_ready_for_lowering = parse_surface.ready_for_lowering;
  scaffold.lowering_boundary_replay_key = typed_surface.lowering_boundary_replay_key;
  if (scaffold.lowering_boundary_replay_key.empty()) {
    scaffold.lowering_boundary_replay_key = parse_surface.lowering_boundary_replay_key;
  }
  scaffold.typed_handoff_key = typed_surface.typed_handoff_key;
  scaffold.parse_artifact_replay_key = parse_surface.parse_artifact_replay_key;
  scaffold.invariant_proofs_ready =
      scaffold.lowering_boundary_ready &&
      scaffold.runtime_dispatch_contract_consistent &&
      scaffold.typed_handoff_key_deterministic &&
      scaffold.typed_core_feature_consistent &&
      scaffold.parse_ready_for_lowering &&
      !scaffold.lowering_boundary_replay_key.empty() &&
      !scaffold.typed_handoff_key.empty() &&
      !scaffold.parse_artifact_replay_key.empty();
  scaffold.modular_split_ready =
      scaffold.typed_surface_present &&
      scaffold.parse_readiness_surface_present &&
      scaffold.invariant_proofs_ready;
  scaffold.scaffold_key = BuildObjc3LoweringRuntimeStabilityInvariantScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.typed_surface_present) {
    scaffold.failure_reason = "typed sema-to-lowering surface missing";
  } else if (!scaffold.parse_readiness_surface_present) {
    scaffold.failure_reason = "parse-lowering readiness surface missing";
  } else if (!scaffold.lowering_boundary_ready) {
    scaffold.failure_reason = "lowering boundary is not ready";
  } else if (!scaffold.runtime_dispatch_contract_consistent) {
    scaffold.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!scaffold.typed_handoff_key_deterministic) {
    scaffold.failure_reason = "typed handoff key is not deterministic";
  } else if (!scaffold.typed_core_feature_consistent) {
    scaffold.failure_reason = "typed core feature is inconsistent";
  } else if (!scaffold.parse_ready_for_lowering) {
    scaffold.failure_reason = "parse-lowering readiness is false";
  } else if (!scaffold.invariant_proofs_ready) {
    scaffold.failure_reason = "lowering/runtime invariant proofs are not ready";
  } else {
    scaffold.failure_reason = "lowering/runtime modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3LoweringRuntimeStabilityInvariantScaffoldReady(
    const Objc3LoweringRuntimeStabilityInvariantScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "lowering/runtime stability invariant scaffold not ready"
               : scaffold.failure_reason;
  return false;
}
