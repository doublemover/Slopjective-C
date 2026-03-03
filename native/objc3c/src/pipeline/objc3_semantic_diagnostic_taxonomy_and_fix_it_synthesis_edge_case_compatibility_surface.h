#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilityKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-edge-case-compatibility:v1:"
      << "core_feature_expansion_ready=" << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";typed_handoff_key_consistent=" << (surface.typed_handoff_key_consistent ? "true" : "false")
      << ";compatibility_handoff_consistent=" << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";parse_artifact_edge_case_robustness_consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false")
      << ";parse_artifact_replay_key_deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";parse_recovery_determinism_hardening_consistent="
      << (surface.parse_recovery_determinism_hardening_consistent ? "true" : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready=" << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";ownership_arc_diagnostic_candidate_sites="
      << surface.ownership_arc_diagnostic_candidate_sites
      << ";ownership_arc_fixit_available_sites=" << surface.ownership_arc_fixit_available_sites
      << ";ownership_arc_profiled_sites=" << surface.ownership_arc_profiled_sites
      << ";ownership_arc_weak_unowned_conflict_diagnostic_sites="
      << surface.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ";ownership_arc_empty_fixit_hint_sites="
      << surface.ownership_arc_empty_fixit_hint_sites
      << ";ownership_arc_contract_violation_sites="
      << surface.ownership_arc_contract_violation_sites
      << ";typed_handoff_key=" << surface.typed_handoff_key
      << ";compatibility_handoff_key=" << surface.compatibility_handoff_key
      << ";parse_artifact_edge_robustness_key=" << surface.parse_artifact_edge_robustness_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";expansion_key=" << surface.expansion_key;
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface &expansion_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface surface;
  surface.core_feature_expansion_ready = expansion_surface.core_feature_expansion_ready;
  surface.ownership_arc_diagnostic_candidate_sites =
      expansion_surface.ownership_arc_diagnostic_candidate_sites;
  surface.ownership_arc_fixit_available_sites =
      expansion_surface.ownership_arc_fixit_available_sites;
  surface.ownership_arc_profiled_sites =
      expansion_surface.ownership_arc_profiled_sites;
  surface.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      expansion_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  surface.ownership_arc_empty_fixit_hint_sites =
      expansion_surface.ownership_arc_empty_fixit_hint_sites;
  surface.ownership_arc_contract_violation_sites =
      expansion_surface.ownership_arc_contract_violation_sites;
  surface.typed_handoff_key = expansion_surface.typed_handoff_key;
  surface.compatibility_handoff_key = parse_surface.compatibility_handoff_key;
  surface.parse_artifact_edge_robustness_key =
      parse_surface.parse_artifact_edge_robustness_key;
  surface.parse_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.expansion_key = expansion_surface.expansion_key;

  surface.typed_handoff_key_consistent =
      expansion_surface.typed_handoff_key_consistent &&
      typed_surface.semantic_handoff_deterministic &&
      typed_surface.typed_handoff_key_deterministic &&
      !surface.typed_handoff_key.empty() &&
      !typed_surface.typed_handoff_key.empty() &&
      surface.typed_handoff_key == typed_surface.typed_handoff_key;
  surface.compatibility_handoff_consistent =
      parse_surface.compatibility_handoff_consistent;
  surface.language_version_pragma_coordinate_order_consistent =
      parse_surface.language_version_pragma_coordinate_order_consistent;
  surface.parse_artifact_edge_case_robustness_consistent =
      parse_surface.parse_artifact_edge_case_robustness_consistent;
  surface.parse_artifact_replay_key_deterministic =
      parse_surface.parse_artifact_replay_key_deterministic;
  surface.parse_recovery_determinism_hardening_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent;
  surface.edge_case_compatibility_consistent =
      surface.core_feature_expansion_ready &&
      surface.typed_handoff_key_consistent &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_artifact_replay_key_deterministic;
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_consistent &&
      surface.parse_recovery_determinism_hardening_consistent &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.expansion_key.empty();
  surface.edge_case_compatibility_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilityKey(
          surface);

  if (surface.edge_case_compatibility_ready) {
    return surface;
  }

  if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature expansion is not ready";
  } else if (!surface.typed_handoff_key_consistent) {
    surface.failure_reason = "typed sema handoff key continuity is inconsistent";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason = "language-version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "parse artifact edge-case robustness is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "parse artifact replay key determinism is inconsistent";
  } else if (!surface.parse_recovery_determinism_hardening_consistent) {
    surface.failure_reason = "parse recovery determinism hardening is inconsistent";
  } else if (!surface.edge_case_compatibility_consistent) {
    surface.failure_reason = "semantic diagnostic taxonomy/fix-it edge-case compatibility is inconsistent";
  } else {
    surface.failure_reason = "semantic diagnostic taxonomy/fix-it edge-case compatibility is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface &surface,
    std::string &reason) {
  if (surface.edge_case_compatibility_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it edge-case compatibility is not ready"
               : surface.failure_reason;
  return false;
}
