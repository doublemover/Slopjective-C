#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseRobustnessKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-edge-case-robustness:v1:"
      << "edge_case_compatibility_ready=" << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";edge_case_expansion_consistent=" << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_ready=" << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";compatibility_key_ready=" << (!surface.edge_case_compatibility_key.empty() ? "true" : "false")
      << ";parse_edge_robustness_key_ready="
      << (!surface.parse_artifact_edge_robustness_key.empty() ? "true" : "false")
      << ";long_tail_edge_robustness_key_ready="
      << (!surface.long_tail_grammar_edge_case_robustness_key.empty() ? "true" : "false")
      << ";typed_handoff_key_ready=" << (!surface.typed_handoff_key.empty() ? "true" : "false")
      << ";parse_recovery_key_ready="
      << (!surface.parse_recovery_determinism_hardening_key.empty() ? "true" : "false")
      << ";ownership_arc_profiled_sites=" << surface.ownership_arc_profiled_sites
      << ";compatibility_handoff_key=" << surface.compatibility_handoff_key
      << ";parse_artifact_edge_robustness_key=" << surface.parse_artifact_edge_robustness_key
      << ";long_tail_grammar_edge_case_robustness_key="
      << surface.long_tail_grammar_edge_case_robustness_key
      << ";typed_handoff_key=" << surface.typed_handoff_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";edge_case_compatibility_key=" << surface.edge_case_compatibility_key;
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface &compatibility_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface surface;
  surface.core_feature_expansion_ready = compatibility_surface.core_feature_expansion_ready;
  surface.edge_case_compatibility_consistent =
      compatibility_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready = compatibility_surface.edge_case_compatibility_ready;
  surface.ownership_arc_diagnostic_candidate_sites =
      compatibility_surface.ownership_arc_diagnostic_candidate_sites;
  surface.ownership_arc_fixit_available_sites =
      compatibility_surface.ownership_arc_fixit_available_sites;
  surface.ownership_arc_profiled_sites =
      compatibility_surface.ownership_arc_profiled_sites;
  surface.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      compatibility_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  surface.ownership_arc_empty_fixit_hint_sites =
      compatibility_surface.ownership_arc_empty_fixit_hint_sites;
  surface.ownership_arc_contract_violation_sites =
      compatibility_surface.ownership_arc_contract_violation_sites;
  surface.typed_handoff_key = compatibility_surface.typed_handoff_key;
  surface.compatibility_handoff_key = compatibility_surface.compatibility_handoff_key;
  surface.parse_artifact_edge_robustness_key =
      compatibility_surface.parse_artifact_edge_robustness_key;
  surface.long_tail_grammar_edge_case_robustness_key =
      parse_surface.long_tail_grammar_edge_case_robustness_key;
  surface.parse_recovery_determinism_hardening_key =
      compatibility_surface.parse_recovery_determinism_hardening_key;
  surface.edge_case_compatibility_key = compatibility_surface.edge_case_compatibility_key;

  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_consistent &&
      parse_surface.long_tail_grammar_edge_case_expansion_consistent &&
      parse_surface.long_tail_grammar_edge_case_robustness_ready &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_artifact_replay_key_deterministic;
  surface.edge_case_robustness_ready =
      surface.edge_case_compatibility_ready &&
      surface.edge_case_expansion_consistent &&
      !surface.edge_case_compatibility_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty();
  surface.edge_case_robustness_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseRobustnessKey(
          surface);
  surface.edge_case_robustness_ready =
      surface.edge_case_robustness_ready &&
      !surface.edge_case_robustness_key.empty();

  if (surface.edge_case_robustness_ready) {
    return surface;
  }

  if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it edge-case compatibility is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it edge-case expansion is inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it edge-case robustness is not ready";
  }
  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface &surface,
    std::string &reason) {
  if (surface.edge_case_robustness_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it edge-case robustness is not ready"
               : surface.failure_reason;
  return false;
}
