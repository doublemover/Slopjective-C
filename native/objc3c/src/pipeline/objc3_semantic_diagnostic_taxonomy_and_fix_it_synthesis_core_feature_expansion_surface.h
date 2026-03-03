#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-core-feature-expansion:v1:"
      << "core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false")
      << ";modular_split_ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";typed_handoff_key_consistent="
      << (surface.typed_handoff_key_consistent ? "true" : "false")
      << ";arc_diagnostics_fixit_replay_key_consistent="
      << (surface.arc_diagnostics_fixit_replay_key_consistent ? "true" : "false")
      << ";diagnostics_fixit_payload_accounting_consistent="
      << (surface.diagnostics_fixit_payload_accounting_consistent ? "true" : "false")
      << ";expansion_replay_keys_ready="
      << (surface.expansion_replay_keys_ready ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";ownership_arc_diagnostic_candidate_sites="
      << surface.ownership_arc_diagnostic_candidate_sites
      << ";ownership_arc_fixit_available_sites="
      << surface.ownership_arc_fixit_available_sites
      << ";ownership_arc_profiled_sites=" << surface.ownership_arc_profiled_sites
      << ";ownership_arc_weak_unowned_conflict_diagnostic_sites="
      << surface.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ";ownership_arc_empty_fixit_hint_sites="
      << surface.ownership_arc_empty_fixit_hint_sites
      << ";ownership_arc_contract_violation_sites="
      << surface.ownership_arc_contract_violation_sites
      << ";core_feature_key=" << surface.core_feature_key
      << ";arc_diagnostics_fixit_replay_key=" << surface.arc_diagnostics_fixit_replay_key;
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface &core_surface,
    const Objc3SemaParityContractSurface &sema_parity_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface surface;
  surface.core_feature_impl_ready = core_surface.core_feature_impl_ready;
  surface.modular_split_ready = core_surface.modular_split_ready;
  surface.ownership_arc_diagnostic_candidate_sites =
      core_surface.ownership_arc_diagnostic_candidate_sites;
  surface.ownership_arc_fixit_available_sites =
      core_surface.ownership_arc_fixit_available_sites;
  surface.ownership_arc_profiled_sites =
      core_surface.ownership_arc_profiled_sites;
  surface.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      core_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  surface.ownership_arc_empty_fixit_hint_sites =
      core_surface.ownership_arc_empty_fixit_hint_sites;
  surface.ownership_arc_contract_violation_sites =
      core_surface.ownership_arc_contract_violation_sites;
  surface.sema_pass_flow_handoff_key = core_surface.sema_pass_flow_handoff_key;
  surface.typed_handoff_key = core_surface.typed_handoff_key;
  surface.recovery_replay_key = core_surface.recovery_replay_key;
  surface.arc_diagnostics_fixit_replay_key =
      core_surface.arc_diagnostics_fixit_replay_key;
  surface.core_feature_key = core_surface.core_feature_key;

  const Objc3ArcDiagnosticsFixitSummary &arc_summary =
      sema_parity_surface.arc_diagnostics_fixit_summary;
  surface.typed_handoff_key_consistent =
      typed_surface.semantic_handoff_deterministic &&
      typed_surface.typed_handoff_key_deterministic &&
      !core_surface.typed_handoff_key.empty() &&
      core_surface.typed_handoff_key == typed_surface.typed_handoff_key;
  surface.arc_diagnostics_fixit_replay_key_consistent =
      !core_surface.arc_diagnostics_fixit_replay_key.empty() &&
      core_surface.arc_diagnostics_fixit_replay_key ==
          BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationArcReplayKey(
              core_surface);
  surface.diagnostics_fixit_payload_accounting_consistent =
      core_surface.arc_diagnostics_fixit_case_accounting_consistent &&
      arc_summary.deterministic &&
      arc_summary.ownership_arc_diagnostic_candidate_sites ==
          core_surface.ownership_arc_diagnostic_candidate_sites &&
      arc_summary.ownership_arc_fixit_available_sites ==
          core_surface.ownership_arc_fixit_available_sites &&
      arc_summary.ownership_arc_profiled_sites ==
          core_surface.ownership_arc_profiled_sites &&
      arc_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          core_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
      arc_summary.ownership_arc_empty_fixit_hint_sites ==
          core_surface.ownership_arc_empty_fixit_hint_sites &&
      arc_summary.contract_violation_sites ==
          core_surface.ownership_arc_contract_violation_sites;
  surface.expansion_replay_keys_ready =
      !core_surface.sema_pass_flow_handoff_key.empty() &&
      !core_surface.typed_handoff_key.empty() &&
      !core_surface.recovery_replay_key.empty() &&
      !core_surface.core_feature_key.empty() &&
      surface.arc_diagnostics_fixit_replay_key_consistent;
  surface.core_feature_expansion_ready =
      surface.core_feature_impl_ready &&
      surface.modular_split_ready &&
      surface.typed_handoff_key_consistent &&
      surface.diagnostics_fixit_payload_accounting_consistent &&
      surface.expansion_replay_keys_ready;
  surface.expansion_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionKey(
          surface);

  if (surface.core_feature_expansion_ready) {
    return surface;
  }

  if (!surface.core_feature_impl_ready) {
    surface.failure_reason = "semantic diagnostic taxonomy/fix-it core feature implementation is not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason = "semantic diagnostic taxonomy/fix-it modular split scaffold is not ready";
  } else if (!surface.typed_handoff_key_consistent) {
    surface.failure_reason = "typed sema handoff key continuity is inconsistent";
  } else if (!surface.diagnostics_fixit_payload_accounting_consistent) {
    surface.failure_reason = "arc diagnostics/fix-it payload accounting is inconsistent";
  } else if (!surface.expansion_replay_keys_ready) {
    surface.failure_reason = "core feature expansion replay keys are incomplete";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature expansion is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface &surface,
    std::string &reason) {
  if (surface.core_feature_expansion_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it core feature expansion is not ready"
               : surface.failure_reason;
  return false;
}
