#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffoldKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold &scaffold) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-modular-split-scaffold:v1:"
      << "sema_pass_flow_summary_present="
      << (scaffold.sema_pass_flow_summary_present ? "true" : "false")
      << ";sema_pass_flow_summary_ready="
      << (scaffold.sema_pass_flow_summary_ready ? "true" : "false")
      << ";sema_parity_surface_present="
      << (scaffold.sema_parity_surface_present ? "true" : "false")
      << ";sema_parity_surface_ready="
      << (scaffold.sema_parity_surface_ready ? "true" : "false")
      << ";diagnostics_taxonomy_consistent="
      << (scaffold.diagnostics_taxonomy_consistent ? "true" : "false")
      << ";arc_diagnostics_fixit_handoff_deterministic="
      << (scaffold.arc_diagnostics_fixit_handoff_deterministic ? "true" : "false")
      << ";typed_sema_handoff_deterministic="
      << (scaffold.typed_sema_handoff_deterministic ? "true" : "false")
      << ";modular_split_ready=" << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(
    const Objc3SemaPassFlowSummary &sema_pass_flow_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold scaffold;
  scaffold.sema_pass_flow_summary_present =
      !sema_pass_flow_summary.deterministic_handoff_key.empty() ||
      sema_pass_flow_summary.configured_pass_count > 0u ||
      sema_pass_flow_summary.diagnostics_total > 0u;
  scaffold.sema_parity_surface_present =
      sema_parity_surface.ready ||
      !sema_parity_surface.pass_flow_recovery_replay_key.empty() ||
      sema_parity_surface.diagnostics_total > 0u;
  scaffold.sema_pass_flow_summary_ready =
      IsReadyObjc3SemaPassFlowSummary(sema_pass_flow_summary);
  scaffold.sema_parity_surface_ready =
      IsReadyObjc3SemaParityContractSurface(sema_parity_surface);
  scaffold.diagnostics_taxonomy_consistent =
      sema_pass_flow_summary.diagnostics_after_pass_monotonic &&
      sema_pass_flow_summary.diagnostics_emission_totals_consistent &&
      sema_pass_flow_summary.diagnostics_accounting_consistent &&
      sema_pass_flow_summary.diagnostics_bus_publish_consistent &&
      sema_pass_flow_summary.diagnostics_canonicalized &&
      sema_pass_flow_summary.diagnostics_hardening_satisfied &&
      sema_pass_flow_summary.deterministic;
  scaffold.arc_diagnostics_fixit_summary_deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic;
  scaffold.arc_diagnostics_fixit_handoff_deterministic =
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff &&
      scaffold.arc_diagnostics_fixit_summary_deterministic;
  scaffold.typed_sema_handoff_deterministic =
      typed_surface.sema_parity_surface_deterministic &&
      typed_surface.semantic_handoff_deterministic &&
      typed_surface.typed_handoff_key_deterministic;
  scaffold.sema_pass_flow_handoff_key =
      sema_pass_flow_summary.deterministic_handoff_key;
  scaffold.typed_handoff_key = typed_surface.typed_handoff_key;
  scaffold.modular_split_ready =
      scaffold.sema_pass_flow_summary_present &&
      scaffold.sema_pass_flow_summary_ready &&
      scaffold.sema_parity_surface_present &&
      scaffold.sema_parity_surface_ready &&
      scaffold.diagnostics_taxonomy_consistent &&
      scaffold.arc_diagnostics_fixit_handoff_deterministic &&
      scaffold.typed_sema_handoff_deterministic &&
      !scaffold.sema_pass_flow_handoff_key.empty() &&
      !scaffold.typed_handoff_key.empty();
  scaffold.scaffold_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.sema_pass_flow_summary_present) {
    scaffold.failure_reason = "sema pass-flow summary missing";
  } else if (!scaffold.sema_pass_flow_summary_ready) {
    scaffold.failure_reason = "sema pass-flow summary is not ready";
  } else if (!scaffold.sema_parity_surface_present) {
    scaffold.failure_reason = "sema parity surface missing";
  } else if (!scaffold.sema_parity_surface_ready) {
    scaffold.failure_reason = "sema parity surface is not ready";
  } else if (!scaffold.diagnostics_taxonomy_consistent) {
    scaffold.failure_reason =
        "semantic diagnostics taxonomy accounting is inconsistent";
  } else if (!scaffold.arc_diagnostics_fixit_handoff_deterministic) {
    scaffold.failure_reason = "arc diagnostics/fix-it handoff is not deterministic";
  } else if (!scaffold.typed_sema_handoff_deterministic) {
    scaffold.failure_reason = "typed sema handoff is not deterministic";
  } else if (scaffold.sema_pass_flow_handoff_key.empty()) {
    scaffold.failure_reason = "sema pass-flow handoff key is empty";
  } else if (scaffold.typed_handoff_key.empty()) {
    scaffold.failure_reason = "typed sema handoff key is empty";
  } else {
    scaffold.failure_reason =
        "semantic diagnostic taxonomy/fix-it modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffoldReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it scaffold not ready"
               : scaffold.failure_reason;
  return false;
}
