#include "pipeline/frontend_phase_publication_helpers.h"

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

namespace objc3c::pipeline::orchestration {

Objc3FrontendSemanticDiagnosticTaxonomyPhaseResult
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

}  // namespace objc3c::pipeline::orchestration
