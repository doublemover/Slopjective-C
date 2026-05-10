#include "pipeline/frontend_phase_publication_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
BuildToolingDiagnosticTaxonomyPortabilityContractSummary(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &migration_summary,
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
        &diagnostic_surface) {
  Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary summary;
  summary.portability_dependency_count =
      summary.portability_dependency_surface_ids.size();
  summary.diagnostics_total = diagnostic_surface.diagnostics_total;
  summary.diagnostics_after_pass_final =
      diagnostic_surface.diagnostics_after_pass_final;
  summary.diagnostics_emitted_total =
      diagnostic_surface.diagnostics_emitted_total;
  summary.ownership_arc_diagnostic_candidate_sites =
      diagnostic_surface.ownership_arc_diagnostic_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      diagnostic_surface.ownership_arc_fixit_available_sites;
  summary.ownership_arc_profiled_sites =
      diagnostic_surface.ownership_arc_profiled_sites;
  summary.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      diagnostic_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      diagnostic_surface.ownership_arc_empty_fixit_hint_sites;
  summary.ownership_arc_contract_violation_sites =
      diagnostic_surface.ownership_arc_contract_violation_sites;
  summary.migration_canonicalization_candidate_sites =
      migration_summary.canonicalization_candidate_sites;
  summary.migration_surface_ready =
      migration_summary.ready_for_semantic_expansion;
  summary.portability_dependencies_frozen =
      summary.portability_dependency_count == 11;
  summary.recovery_replay_key = diagnostic_surface.recovery_replay_key;
  summary.arc_diagnostics_fixit_replay_key =
      diagnostic_surface.arc_diagnostics_fixit_replay_key;
  summary.deterministic_handoff =
      summary.migration_surface_ready &&
      summary.portability_dependencies_frozen &&
      diagnostic_surface.core_feature_impl_ready &&
      diagnostic_surface.arc_diagnostics_fixit_summary_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_handoff_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_case_accounting_consistent &&
      diagnostic_surface.replay_keys_ready &&
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites +
              summary.ownership_arc_contract_violation_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.migration_surface_ready) {
    summary.failure_reason =
        "tooling migration/canonicalization source completion prerequisite is not ready";
  } else if (!diagnostic_surface.core_feature_impl_ready) {
    summary.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature implementation surface is not ready";
  }
  summary.replay_key =
      BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(summary);
  return summary;
}

Objc3ToolingFeatureSpecificFixitSynthesisSummary
BuildToolingFeatureSpecificFixitSynthesisSummary(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
        &taxonomy_summary) {
  Objc3ToolingFeatureSpecificFixitSynthesisSummary summary;
  summary.fixit_family_count = summary.fixit_family_ids.size();
  summary.migration_fixit_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.migrator_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      taxonomy_summary.ownership_arc_fixit_available_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      taxonomy_summary.ownership_arc_empty_fixit_hint_sites;
  summary.diagnostic_taxonomy_ready =
      taxonomy_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      summary.diagnostic_taxonomy_ready && summary.fixit_family_count == 2 &&
      summary.migration_fixit_candidate_sites ==
          summary.migrator_candidate_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.diagnostic_taxonomy_ready) {
    summary.failure_reason =
        "tooling diagnostic taxonomy and portability contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingFeatureSpecificFixitSynthesisReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
