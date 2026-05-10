#include "pipeline/frontend_phase_publication_helpers.h"

#include <utility>

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
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"

namespace objc3c::pipeline::orchestration {

void AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
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
      std::move(
          phase.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface);
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
      std::move(
          phase.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface);
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

void PopulateObjc3FrontendReadinessLoweringPhaseResult(
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
