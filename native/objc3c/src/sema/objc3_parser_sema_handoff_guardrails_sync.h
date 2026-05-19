#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_contract.h"

inline Objc3ParserSemaPerformanceQualityGuardrails BuildObjc3ParserSemaPerformanceQualityGuardrails(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus) {
  Objc3ParserSemaPerformanceQualityGuardrails guardrails;
  guardrails.conformance_matrix_builder_max_lines = kObjc3ParserSemaConformanceMatrixBuilderMaxLines;
  guardrails.conformance_corpus_builder_max_lines = kObjc3ParserSemaConformanceCorpusBuilderMaxLines;
  guardrails.handoff_scaffold_builder_max_lines = kObjc3ParserSemaHandoffScaffoldBuilderMaxLines;
  guardrails.conformance_matrix_builder_budget_guarded =
      guardrails.conformance_matrix_builder_max_lines >= 150u &&
      guardrails.conformance_matrix_builder_max_lines <= 220u;
  guardrails.conformance_corpus_builder_budget_guarded =
      guardrails.conformance_corpus_builder_max_lines >= 40u &&
      guardrails.conformance_corpus_builder_max_lines <= 100u;
  guardrails.handoff_scaffold_builder_budget_guarded =
      guardrails.handoff_scaffold_builder_max_lines >= 40u &&
      guardrails.handoff_scaffold_builder_max_lines <= 120u;
  guardrails.matrix_diagnostic_budget_consistent =
      matrix.parser_diagnostic_budget_consistent;
  guardrails.matrix_token_top_level_budget_consistent =
      matrix.parser_token_top_level_budget_consistent;
  guardrails.matrix_subset_budget_consistent = matrix.parser_subset_count_consistent;
  guardrails.corpus_case_budget_consistent =
      corpus.required_case_count == 5u &&
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u;
  guardrails.required_guardrail_count = 7u;
  guardrails.passed_guardrail_count =
      static_cast<std::size_t>(guardrails.conformance_matrix_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.conformance_corpus_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.handoff_scaffold_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.matrix_diagnostic_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_token_top_level_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_subset_budget_consistent) +
      static_cast<std::size_t>(guardrails.corpus_case_budget_consistent);
  guardrails.failed_guardrail_count =
      guardrails.required_guardrail_count >= guardrails.passed_guardrail_count
          ? (guardrails.required_guardrail_count - guardrails.passed_guardrail_count)
          : guardrails.required_guardrail_count;
  guardrails.deterministic =
      matrix.deterministic &&
      corpus.deterministic &&
      guardrails.required_guardrail_count == 7u &&
      guardrails.passed_guardrail_count == guardrails.required_guardrail_count &&
      guardrails.failed_guardrail_count == 0u;
  return guardrails;
}
