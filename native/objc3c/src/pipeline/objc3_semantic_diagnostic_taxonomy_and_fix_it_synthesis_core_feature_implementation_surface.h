#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationArcReplayKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-arc-core-feature:v1:"
      << "ownership_arc_diagnostic_candidate_sites=" << surface.ownership_arc_diagnostic_candidate_sites
      << ";ownership_arc_fixit_available_sites=" << surface.ownership_arc_fixit_available_sites
      << ";ownership_arc_profiled_sites=" << surface.ownership_arc_profiled_sites
      << ";ownership_arc_weak_unowned_conflict_diagnostic_sites="
      << surface.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ";ownership_arc_empty_fixit_hint_sites=" << surface.ownership_arc_empty_fixit_hint_sites
      << ";ownership_arc_contract_violation_sites=" << surface.ownership_arc_contract_violation_sites
      << ";arc_diagnostics_fixit_case_accounting_consistent="
      << (surface.arc_diagnostics_fixit_case_accounting_consistent ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-core-feature-impl:v1:"
      << "sema_pass_flow_summary_ready=" << (surface.sema_pass_flow_summary_ready ? "true" : "false")
      << ";sema_parity_surface_ready=" << (surface.sema_parity_surface_ready ? "true" : "false")
      << ";diagnostics_taxonomy_consistent=" << (surface.diagnostics_taxonomy_consistent ? "true" : "false")
      << ";arc_diagnostics_fixit_summary_deterministic="
      << (surface.arc_diagnostics_fixit_summary_deterministic ? "true" : "false")
      << ";arc_diagnostics_fixit_handoff_deterministic="
      << (surface.arc_diagnostics_fixit_handoff_deterministic ? "true" : "false")
      << ";typed_sema_handoff_deterministic="
      << (surface.typed_sema_handoff_deterministic ? "true" : "false")
      << ";modular_split_ready=" << (surface.modular_split_ready ? "true" : "false")
      << ";diagnostics_case_accounting_consistent="
      << (surface.diagnostics_case_accounting_consistent ? "true" : "false")
      << ";arc_diagnostics_fixit_case_accounting_consistent="
      << (surface.arc_diagnostics_fixit_case_accounting_consistent ? "true" : "false")
      << ";replay_keys_ready=" << (surface.replay_keys_ready ? "true" : "false")
      << ";diagnostics_total=" << surface.diagnostics_total
      << ";diagnostics_after_pass_final=" << surface.diagnostics_after_pass_final
      << ";diagnostics_emitted_total=" << surface.diagnostics_emitted_total
      << ";sema_pass_flow_handoff_key=" << surface.sema_pass_flow_handoff_key
      << ";typed_handoff_key=" << surface.typed_handoff_key
      << ";recovery_replay_key=" << surface.recovery_replay_key
      << ";arc_diagnostics_fixit_replay_key=" << surface.arc_diagnostics_fixit_replay_key
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(
    const Objc3SemaPassFlowSummary &sema_pass_flow_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold &scaffold) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface surface;
  surface.sema_pass_flow_summary_ready = IsReadyObjc3SemaPassFlowSummary(sema_pass_flow_summary);
  surface.sema_parity_surface_ready = IsReadyObjc3SemaParityContractSurface(sema_parity_surface);
  surface.diagnostics_taxonomy_consistent = scaffold.diagnostics_taxonomy_consistent;
  surface.arc_diagnostics_fixit_summary_deterministic =
      scaffold.arc_diagnostics_fixit_summary_deterministic;
  surface.arc_diagnostics_fixit_handoff_deterministic =
      scaffold.arc_diagnostics_fixit_handoff_deterministic;
  surface.typed_sema_handoff_deterministic =
      scaffold.typed_sema_handoff_deterministic &&
      typed_surface.semantic_handoff_deterministic;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.diagnostics_total = sema_pass_flow_summary.diagnostics_total;
  surface.diagnostics_after_pass_final = sema_pass_flow_summary.diagnostics_after_pass.back();
  surface.diagnostics_emitted_total =
      sema_pass_flow_summary.diagnostics_emitted_by_pass[0] +
      sema_pass_flow_summary.diagnostics_emitted_by_pass[1] +
      sema_pass_flow_summary.diagnostics_emitted_by_pass[2];
  surface.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  surface.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  surface.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  surface.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  surface.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  surface.ownership_arc_contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  surface.sema_pass_flow_handoff_key = scaffold.sema_pass_flow_handoff_key;
  surface.typed_handoff_key = scaffold.typed_handoff_key;
  surface.recovery_replay_key = sema_pass_flow_summary.recovery_replay_key;

  const Objc3ArcDiagnosticsFixitSummary &arc_summary =
      sema_parity_surface.arc_diagnostics_fixit_summary;
  const bool diagnostics_case_accounting_consistent =
      sema_pass_flow_summary.diagnostics_after_pass_monotonic &&
      sema_pass_flow_summary.diagnostics_emission_totals_consistent &&
      sema_pass_flow_summary.diagnostics_accounting_consistent &&
      sema_pass_flow_summary.diagnostics_bus_publish_consistent &&
      sema_pass_flow_summary.diagnostics_canonicalized &&
      sema_pass_flow_summary.diagnostics_hardening_satisfied &&
      sema_pass_flow_summary.diagnostics_total ==
          sema_pass_flow_summary.diagnostics_after_pass.back() &&
      sema_pass_flow_summary.diagnostics_total ==
          surface.diagnostics_emitted_total &&
      sema_pass_flow_summary.diagnostics_total ==
          sema_parity_surface.diagnostics_total &&
      sema_parity_surface.sema_pass_flow_summary.diagnostics_total ==
          sema_parity_surface.diagnostics_total;

  const bool arc_summary_totals_match =
      arc_summary.ownership_arc_diagnostic_candidate_sites ==
          surface.ownership_arc_diagnostic_candidate_sites &&
      arc_summary.ownership_arc_fixit_available_sites ==
          surface.ownership_arc_fixit_available_sites &&
      arc_summary.ownership_arc_profiled_sites ==
          surface.ownership_arc_profiled_sites &&
      arc_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          surface.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
      arc_summary.ownership_arc_empty_fixit_hint_sites ==
          surface.ownership_arc_empty_fixit_hint_sites &&
      arc_summary.contract_violation_sites ==
          surface.ownership_arc_contract_violation_sites;
  const bool arc_summary_bounds_consistent =
      arc_summary.ownership_arc_fixit_available_sites <=
          arc_summary.ownership_arc_diagnostic_candidate_sites +
              arc_summary.contract_violation_sites &&
      arc_summary.ownership_arc_profiled_sites <=
          arc_summary.ownership_arc_diagnostic_candidate_sites +
              arc_summary.contract_violation_sites &&
      arc_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          arc_summary.ownership_arc_diagnostic_candidate_sites +
              arc_summary.contract_violation_sites &&
      arc_summary.ownership_arc_empty_fixit_hint_sites <=
          arc_summary.ownership_arc_fixit_available_sites +
              arc_summary.contract_violation_sites;

  surface.diagnostics_case_accounting_consistent =
      diagnostics_case_accounting_consistent;
  surface.arc_diagnostics_fixit_case_accounting_consistent =
      arc_summary.deterministic &&
      arc_summary_totals_match &&
      arc_summary_bounds_consistent;
  surface.arc_diagnostics_fixit_replay_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationArcReplayKey(
          surface);
  surface.replay_keys_ready =
      !surface.sema_pass_flow_handoff_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.recovery_replay_key.empty() &&
      !surface.arc_diagnostics_fixit_replay_key.empty();

  surface.core_feature_impl_ready =
      surface.sema_pass_flow_summary_ready &&
      surface.sema_parity_surface_ready &&
      surface.diagnostics_taxonomy_consistent &&
      surface.arc_diagnostics_fixit_summary_deterministic &&
      surface.arc_diagnostics_fixit_handoff_deterministic &&
      surface.typed_sema_handoff_deterministic &&
      surface.modular_split_ready &&
      surface.diagnostics_case_accounting_consistent &&
      surface.arc_diagnostics_fixit_case_accounting_consistent &&
      surface.replay_keys_ready;
  surface.core_feature_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationKey(
          surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.sema_pass_flow_summary_ready) {
    surface.failure_reason = "sema pass-flow summary is not ready";
  } else if (!surface.sema_parity_surface_ready) {
    surface.failure_reason = "sema parity surface is not ready";
  } else if (!surface.diagnostics_taxonomy_consistent) {
    surface.failure_reason = "diagnostics taxonomy consistency is not ready";
  } else if (!surface.arc_diagnostics_fixit_summary_deterministic) {
    surface.failure_reason =
        "arc diagnostics/fix-it summary determinism is not ready";
  } else if (!surface.arc_diagnostics_fixit_handoff_deterministic) {
    surface.failure_reason =
        "arc diagnostics/fix-it handoff determinism is not ready";
  } else if (!surface.typed_sema_handoff_deterministic) {
    surface.failure_reason = "typed sema handoff determinism is not ready";
  } else if (!surface.modular_split_ready) {
    surface.failure_reason = "semantic diagnostic taxonomy/fix-it scaffold is not ready";
  } else if (!surface.diagnostics_case_accounting_consistent) {
    surface.failure_reason = "semantic diagnostics case accounting is inconsistent";
  } else if (!surface.arc_diagnostics_fixit_case_accounting_consistent) {
    surface.failure_reason = "arc diagnostics/fix-it case accounting is inconsistent";
  } else if (!surface.replay_keys_ready) {
    surface.failure_reason = "core-feature replay keys are incomplete";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature implementation is not ready";
  }

  return surface;
}

inline bool
IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
