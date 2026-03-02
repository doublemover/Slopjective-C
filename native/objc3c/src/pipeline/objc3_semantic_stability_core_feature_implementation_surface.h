#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3SemanticStabilityCoreFeatureImplementationKey(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "semantic-stability-core-feature-impl:v1:"
      << "typed_core_feature_case_count=" << surface.typed_core_feature_case_count
      << ";typed_core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";typed_core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";typed_core_feature_expansion_case_count=" << surface.typed_core_feature_expansion_case_count
      << ";typed_core_feature_expansion_passed_case_count="
      << surface.typed_core_feature_expansion_passed_case_count
      << ";typed_core_feature_expansion_failed_case_count="
      << surface.typed_core_feature_expansion_failed_case_count
      << ";parse_lowering_conformance_matrix_case_count="
      << surface.parse_lowering_conformance_matrix_case_count
      << ";parse_lowering_conformance_corpus_case_count="
      << surface.parse_lowering_conformance_corpus_case_count
      << ";parse_lowering_conformance_corpus_passed_case_count="
      << surface.parse_lowering_conformance_corpus_passed_case_count
      << ";parse_lowering_conformance_corpus_failed_case_count="
      << surface.parse_lowering_conformance_corpus_failed_case_count
      << ";parse_lowering_performance_quality_guardrails_case_count="
      << surface.parse_lowering_performance_quality_guardrails_case_count
      << ";parse_lowering_performance_quality_guardrails_passed_case_count="
      << surface.parse_lowering_performance_quality_guardrails_passed_case_count
      << ";parse_lowering_performance_quality_guardrails_failed_case_count="
      << surface.parse_lowering_performance_quality_guardrails_failed_case_count
      << ";semantic_handoff_deterministic="
      << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";spec_delta_closed=" << (surface.spec_delta_closed ? "true" : "false")
      << ";modular_split_ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";typed_core_feature_expansion_accounting_consistent="
      << (surface.typed_core_feature_expansion_accounting_consistent ? "true" : "false")
      << ";parse_conformance_accounting_consistent="
      << (surface.parse_conformance_accounting_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";expansion_ready=" << (surface.expansion_ready ? "true" : "false")
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline Objc3SemanticStabilityCoreFeatureImplementationSurface
BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold) {
  Objc3SemanticStabilityCoreFeatureImplementationSurface surface;
  surface.semantic_handoff_deterministic = scaffold.semantic_handoff_deterministic;
  surface.typed_core_feature_consistent = typed_surface.typed_core_feature_consistent;
  surface.typed_core_feature_expansion_consistent =
      typed_surface.typed_core_feature_expansion_consistent;
  surface.typed_sema_core_feature_consistent = parse_surface.typed_sema_core_feature_consistent;
  surface.typed_sema_core_feature_expansion_consistent =
      parse_surface.typed_sema_core_feature_expansion_consistent;
  surface.parse_lowering_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent;
  surface.parse_lowering_conformance_corpus_consistent =
      parse_surface.parse_lowering_conformance_corpus_consistent;
  surface.parse_lowering_performance_quality_guardrails_consistent =
      parse_surface.parse_lowering_performance_quality_guardrails_consistent;
  surface.spec_delta_closed = scaffold.spec_delta_closed;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.typed_core_feature_case_count = typed_surface.typed_core_feature_case_count;
  surface.typed_core_feature_passed_case_count =
      typed_surface.typed_core_feature_passed_case_count;
  surface.typed_core_feature_failed_case_count =
      typed_surface.typed_core_feature_failed_case_count;
  surface.typed_core_feature_expansion_case_count =
      typed_surface.typed_core_feature_expansion_case_count;
  surface.typed_core_feature_expansion_passed_case_count =
      typed_surface.typed_core_feature_expansion_passed_case_count;
  surface.typed_core_feature_expansion_failed_case_count =
      typed_surface.typed_core_feature_expansion_failed_case_count;
  surface.parse_lowering_conformance_matrix_case_count =
      parse_surface.parse_lowering_conformance_matrix_case_count;
  surface.parse_lowering_conformance_corpus_case_count =
      parse_surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_passed_case_count =
      parse_surface.parse_lowering_conformance_corpus_passed_case_count;
  surface.parse_lowering_conformance_corpus_failed_case_count =
      parse_surface.parse_lowering_conformance_corpus_failed_case_count;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_case_count;
  surface.parse_lowering_performance_quality_guardrails_passed_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_passed_case_count;
  surface.parse_lowering_performance_quality_guardrails_failed_case_count =
      parse_surface.parse_lowering_performance_quality_guardrails_failed_case_count;
  surface.typed_handoff_key = typed_surface.typed_handoff_key;
  surface.parse_artifact_replay_key = parse_surface.parse_artifact_replay_key;

  const bool typed_core_feature_case_accounting_consistent =
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <=
          surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count -
           surface.typed_core_feature_passed_case_count);
  const bool typed_core_feature_expansion_case_accounting_consistent =
      surface.typed_core_feature_expansion_case_count > 0 &&
      surface.typed_core_feature_expansion_passed_case_count <=
          surface.typed_core_feature_expansion_case_count &&
      surface.typed_core_feature_expansion_failed_case_count ==
          (surface.typed_core_feature_expansion_case_count -
           surface.typed_core_feature_expansion_passed_case_count);
  const bool parse_corpus_case_accounting_consistent =
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count <=
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count ==
          (surface.parse_lowering_conformance_corpus_case_count -
           surface.parse_lowering_conformance_corpus_passed_case_count);
  const bool parse_guardrails_case_accounting_consistent =
      surface.parse_lowering_performance_quality_guardrails_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_passed_case_count <=
          surface.parse_lowering_performance_quality_guardrails_case_count &&
      surface.parse_lowering_performance_quality_guardrails_failed_case_count ==
          (surface.parse_lowering_performance_quality_guardrails_case_count -
           surface.parse_lowering_performance_quality_guardrails_passed_case_count);

  const bool parse_matrix_case_count_ready =
      surface.parse_lowering_conformance_matrix_case_count > 0;
  const bool typed_parse_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_core_feature_expansion_consistent &&
      surface.typed_sema_core_feature_consistent &&
      surface.typed_sema_core_feature_expansion_consistent;
  const bool parse_conformance_consistent =
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_performance_quality_guardrails_consistent;
  const bool typed_core_feature_expansion_accounting_consistent =
      typed_core_feature_case_accounting_consistent &&
      typed_core_feature_expansion_case_accounting_consistent;
  const bool parse_conformance_accounting_consistent =
      parse_matrix_case_count_ready &&
      parse_corpus_case_accounting_consistent &&
      parse_guardrails_case_accounting_consistent;
  const bool replay_keys_ready =
      !surface.typed_handoff_key.empty() && !surface.parse_artifact_replay_key.empty();
  const bool edge_case_compatibility_ready =
      parse_surface.compatibility_handoff_consistent &&
      parse_surface.language_version_pragma_coordinate_order_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic &&
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      !parse_surface.compatibility_handoff_key.empty() &&
      !parse_surface.parse_artifact_edge_robustness_key.empty();
  const bool expansion_ready =
      typed_parse_core_feature_consistent &&
      parse_conformance_consistent &&
      typed_core_feature_expansion_accounting_consistent &&
      parse_conformance_accounting_consistent &&
      replay_keys_ready &&
      edge_case_compatibility_ready;
  surface.typed_core_feature_expansion_accounting_consistent =
      typed_core_feature_expansion_accounting_consistent;
  surface.parse_conformance_accounting_consistent =
      parse_conformance_accounting_consistent;
  surface.replay_keys_ready = replay_keys_ready;
  surface.expansion_ready = expansion_ready;

  surface.core_feature_impl_ready =
      surface.semantic_handoff_deterministic &&
      surface.spec_delta_closed &&
      surface.modular_split_ready &&
      expansion_ready;
  surface.core_feature_key =
      BuildObjc3SemanticStabilityCoreFeatureImplementationKey(surface);
  surface.expansion_key =
      "semantic-stability-core-feature-expansion:v1:typed-expansion-consistent=" +
      std::string(surface.typed_core_feature_expansion_accounting_consistent ? "true" : "false") +
      ";parse-accounting-consistent=" +
      std::string(surface.parse_conformance_accounting_consistent ? "true" : "false") +
      ";replay-keys-ready=" +
      std::string(surface.replay_keys_ready ? "true" : "false") +
      ";edge-compat-ready=" +
      std::string(edge_case_compatibility_ready ? "true" : "false") +
      ";compat-handoff-consistent=" +
      std::string(parse_surface.compatibility_handoff_consistent ? "true" : "false") +
      ";parse-edge-robustness-consistent=" +
      std::string(parse_surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false");

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.spec_delta_closed) {
    surface.failure_reason = "semantic stability spec delta is not closed";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason = "semantic stability modular split scaffold is not ready";
  } else if (!typed_parse_core_feature_consistent) {
    surface.failure_reason = "typed/parse core feature consistency is incomplete";
  } else if (!parse_conformance_consistent) {
    surface.failure_reason = "parse conformance consistency is incomplete";
  } else if (!typed_core_feature_expansion_accounting_consistent) {
    surface.failure_reason =
        "typed core feature expansion case accounting is inconsistent";
  } else if (!parse_conformance_accounting_consistent) {
    surface.failure_reason = "parse guardrails case accounting is inconsistent";
  } else if (!replay_keys_ready) {
    surface.failure_reason = "typed/parse replay keys are not ready";
  } else if (!edge_case_compatibility_ready) {
    surface.failure_reason = "semantic stability edge-case compatibility is not ready";
  } else if (!expansion_ready) {
    surface.failure_reason = "semantic stability core feature expansion is not ready";
  } else {
    surface.failure_reason = "semantic stability core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticStabilityCoreFeatureImplementationSurfaceReady(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "semantic stability core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
