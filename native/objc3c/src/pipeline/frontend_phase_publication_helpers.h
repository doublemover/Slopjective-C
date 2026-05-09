#pragma once

#include <utility>

#include "pipeline/frontend_source_closure_replay_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"
#include "sema/model/semantic_ownership.h"
#include "sema/model/semantic_symbol.h"

namespace objc3c::pipeline::orchestration {

inline Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary
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

inline Objc3ToolingFeatureSpecificFixitSynthesisSummary
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

inline Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult
BuildObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
    const Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options) {
  Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult phase;
  phase.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  phase.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface,
          phase.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold);
  phase.tooling_diagnostic_taxonomy_portability_contract_summary =
      BuildToolingDiagnosticTaxonomyPortabilityContractSummary(
          result.tooling_migration_canonicalization_source_completion_summary,
          phase
              .semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface);
  phase.tooling_feature_specific_fixit_synthesis_summary =
      BuildToolingFeatureSpecificFixitSynthesisSummary(
          phase.tooling_diagnostic_taxonomy_portability_contract_summary);
  phase.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  phase.parse_lowering_readiness_surface =
      BuildObjc3ParseLoweringReadinessSurface(result, options);
  phase.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(
          phase.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface,
          phase.parse_lowering_readiness_surface,
          result.typed_sema_to_lowering_contract_surface);
  phase
      .semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(
          phase.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface,
          phase.parse_lowering_readiness_surface);
  phase.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface,
          phase.parse_lowering_readiness_surface);
  phase
      .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(
          phase.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface,
          phase.parse_lowering_readiness_surface);
  phase
      .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface,
          phase.parse_lowering_readiness_surface);
  phase.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface,
          phase.parse_lowering_readiness_surface);
  phase
      .semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface,
          phase.parse_lowering_readiness_surface);
  return phase;
}

inline void AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
    Objc3FrontendPipelineResult &result,
    Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult phase) {
  result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =
      std::move(phase.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface);
  result.tooling_diagnostic_taxonomy_portability_contract_summary =
      std::move(phase.tooling_diagnostic_taxonomy_portability_contract_summary);
  result.tooling_feature_specific_fixit_synthesis_summary =
      std::move(phase.tooling_feature_specific_fixit_synthesis_summary);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface =
      std::move(phase.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface);
  result.parse_lowering_readiness_surface =
      std::move(phase.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface =
      std::move(phase
                    .semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface);
  result
      .semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =
      std::move(phase.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface);
  result
      .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface);
  result
      .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface);
  result
      .semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface =
      std::move(
          phase
              .semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface);
}

inline void PopulateObjc3FrontendReadinessLoweringPhaseResult(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options) {
  Objc3FrontendReadinessLoweringPhaseResult phase;
  phase.semantic_stability_spec_delta_closure_scaffold =
      BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_spec_delta_closure_scaffold =
      phase.semantic_stability_spec_delta_closure_scaffold;
  phase.semantic_stability_core_feature_implementation_surface =
      BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          phase.semantic_stability_spec_delta_closure_scaffold);
  result.semantic_stability_core_feature_implementation_surface =
      phase.semantic_stability_core_feature_implementation_surface;
  phase.lowering_runtime_stability_invariant_scaffold =
      BuildObjc3LoweringRuntimeStabilityInvariantScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.lowering_runtime_stability_invariant_scaffold =
      phase.lowering_runtime_stability_invariant_scaffold;
  phase.lowering_pipeline_pass_graph_scaffold =
      BuildObjc3LoweringPipelinePassGraphScaffold(result, options);
  result.lowering_pipeline_pass_graph_scaffold =
      phase.lowering_pipeline_pass_graph_scaffold;
  phase.lowering_pipeline_pass_graph_core_feature_surface =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(result, options);
  result.lowering_pipeline_pass_graph_core_feature_surface =
      phase.lowering_pipeline_pass_graph_core_feature_surface;
  phase.ir_emission_completeness_scaffold =
      BuildObjc3IREmissionCompletenessScaffold(result);
  result.ir_emission_completeness_scaffold =
      phase.ir_emission_completeness_scaffold;
  phase.lowering_runtime_diagnostics_surfacing_scaffold =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_scaffold =
      phase.lowering_runtime_diagnostics_surfacing_scaffold;
  phase.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface =
      phase.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  phase.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =
      phase.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface;
  phase.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface =
      phase.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface;
  phase
      .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =
      phase
          .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;
  phase.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface =
      phase.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface;
  phase
      .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =
      phase
          .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;
  phase
      .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface =
      phase
          .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface;
  phase.lowering_runtime_stability_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          phase.lowering_runtime_stability_invariant_scaffold);
  result.lowering_runtime_stability_core_feature_implementation_surface =
      phase.lowering_runtime_stability_core_feature_implementation_surface;
}

}  // namespace objc3c::pipeline::orchestration
