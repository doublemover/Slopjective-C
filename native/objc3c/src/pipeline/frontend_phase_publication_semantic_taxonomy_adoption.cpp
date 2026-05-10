#include "pipeline/frontend_phase_publication_helpers.h"

#include <utility>

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

}  // namespace objc3c::pipeline::orchestration
