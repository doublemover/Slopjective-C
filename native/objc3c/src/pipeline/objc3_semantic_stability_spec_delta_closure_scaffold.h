#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3SemanticStabilitySpecDeltaClosureScaffoldKey(
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold) {
  std::ostringstream key;
  key << "semantic-stability-spec-delta-scaffold:v1:"
      << "typed_surface_present=" << (scaffold.typed_surface_present ? "true" : "false")
      << ";parse_readiness_surface_present="
      << (scaffold.parse_readiness_surface_present ? "true" : "false")
      << ";semantic_handoff_deterministic="
      << (scaffold.semantic_handoff_deterministic ? "true" : "false")
      << ";typed_core_feature_expansion_consistent="
      << (scaffold.typed_core_feature_expansion_consistent ? "true" : "false")
      << ";parse_lowering_conformance_matrix_consistent="
      << (scaffold.parse_lowering_conformance_matrix_consistent ? "true" : "false")
      << ";parse_lowering_conformance_corpus_consistent="
      << (scaffold.parse_lowering_conformance_corpus_consistent ? "true" : "false")
      << ";parse_lowering_performance_quality_guardrails_consistent="
      << (scaffold.parse_lowering_performance_quality_guardrails_consistent ? "true" : "false")
      << ";spec_delta_closed=" << (scaffold.spec_delta_closed ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3SemanticStabilitySpecDeltaClosureScaffold BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticStabilitySpecDeltaClosureScaffold scaffold;
  scaffold.typed_surface_present =
      !typed_surface.typed_handoff_key.empty() ||
      typed_surface.ready_for_lowering ||
      !typed_surface.failure_reason.empty();
  scaffold.parse_readiness_surface_present =
      !parse_surface.parse_artifact_replay_key.empty() ||
      parse_surface.ready_for_lowering ||
      !parse_surface.failure_reason.empty();
  scaffold.semantic_handoff_deterministic = typed_surface.semantic_handoff_deterministic;
  scaffold.typed_core_feature_expansion_consistent =
      typed_surface.typed_core_feature_expansion_consistent;
  scaffold.parse_lowering_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent;
  scaffold.parse_lowering_conformance_corpus_consistent =
      parse_surface.parse_lowering_conformance_corpus_consistent;
  scaffold.parse_lowering_performance_quality_guardrails_consistent =
      parse_surface.parse_lowering_performance_quality_guardrails_consistent;
  scaffold.typed_handoff_key = typed_surface.typed_handoff_key;
  scaffold.parse_artifact_replay_key = parse_surface.parse_artifact_replay_key;
  scaffold.spec_delta_closed =
      scaffold.typed_surface_present &&
      scaffold.parse_readiness_surface_present &&
      typed_surface.typed_handoff_key_deterministic &&
      parse_surface.typed_handoff_key_deterministic &&
      scaffold.semantic_handoff_deterministic &&
      scaffold.typed_core_feature_expansion_consistent &&
      scaffold.parse_lowering_conformance_matrix_consistent &&
      scaffold.parse_lowering_conformance_corpus_consistent &&
      scaffold.parse_lowering_performance_quality_guardrails_consistent &&
      !scaffold.typed_handoff_key.empty() &&
      !scaffold.parse_artifact_replay_key.empty();
  scaffold.modular_split_ready =
      scaffold.spec_delta_closed &&
      typed_surface.ready_for_lowering &&
      parse_surface.ready_for_lowering;
  scaffold.scaffold_key = BuildObjc3SemanticStabilitySpecDeltaClosureScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.typed_surface_present) {
    scaffold.failure_reason = "typed sema-to-lowering surface missing";
  } else if (!scaffold.parse_readiness_surface_present) {
    scaffold.failure_reason = "parse-lowering readiness surface missing";
  } else if (!scaffold.semantic_handoff_deterministic) {
    scaffold.failure_reason = "semantic handoff is not deterministic";
  } else if (!scaffold.typed_core_feature_expansion_consistent) {
    scaffold.failure_reason = "typed core feature expansion is inconsistent";
  } else if (!scaffold.parse_lowering_conformance_matrix_consistent) {
    scaffold.failure_reason = "parse-lowering conformance matrix is inconsistent";
  } else if (!scaffold.parse_lowering_conformance_corpus_consistent) {
    scaffold.failure_reason = "parse-lowering conformance corpus is inconsistent";
  } else if (!scaffold.parse_lowering_performance_quality_guardrails_consistent) {
    scaffold.failure_reason = "parse-lowering performance/quality guardrails are inconsistent";
  } else if (!scaffold.spec_delta_closed) {
    scaffold.failure_reason = "semantic stability spec delta is not closed";
  } else {
    scaffold.failure_reason = "semantic stability modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3SemanticStabilitySpecDeltaClosureScaffoldReady(
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty() ? "semantic stability spec delta scaffold not ready"
                                           : scaffold.failure_reason;
  return false;
}
