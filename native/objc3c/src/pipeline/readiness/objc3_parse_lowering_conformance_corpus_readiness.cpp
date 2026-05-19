#include "pipeline/readiness/objc3_parse_lowering_conformance_performance_readiness_private.h"

#include <cstddef>
#include <string>

#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"

void ApplyObjc3ParseLoweringConformanceCorpusReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    Objc3ParseLoweringConformancePerformanceReadinessRecord &record,
    bool sema_handoff_ready,
    bool semantic_handoff_deterministic) {
  const bool parser_contract_snapshot_case_passed =
      surface.parser_contract_snapshot_present;
  const bool parse_artifact_handoff_case_passed =
      (surface.parse_artifact_handoff_deterministic);
  const bool parse_artifact_replay_case_passed =
      (surface.parse_artifact_replay_key_deterministic);
  const bool parse_artifact_diagnostics_hardening_case_passed =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_artifact_edge_robustness_case_passed =
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool parse_recovery_determinism_hardening_case_passed =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool semantic_handoff_case_passed =
      surface.semantic_integration_surface_built &&
      semantic_handoff_deterministic &&
      sema_handoff_ready;
  const bool lowering_boundary_case_passed =
      surface.lowering_boundary_ready &&
      !surface.lowering_boundary_replay_key.empty();
  surface.parse_lowering_conformance_corpus_passed_case_count =
      static_cast<std::size_t>(parser_contract_snapshot_case_passed) +
      static_cast<std::size_t>(parse_artifact_handoff_case_passed) +
      static_cast<std::size_t>(parse_artifact_replay_case_passed) +
      static_cast<std::size_t>(parse_artifact_diagnostics_hardening_case_passed) +
      static_cast<std::size_t>(parse_artifact_edge_robustness_case_passed) +
      static_cast<std::size_t>(parse_recovery_determinism_hardening_case_passed) +
      static_cast<std::size_t>(semantic_handoff_case_passed) +
      static_cast<std::size_t>(lowering_boundary_case_passed);
  surface.parse_lowering_conformance_corpus_failed_case_count =
      surface.parse_lowering_conformance_corpus_case_count >=
              surface.parse_lowering_conformance_corpus_passed_case_count
          ? (surface.parse_lowering_conformance_corpus_case_count -
             surface.parse_lowering_conformance_corpus_passed_case_count)
          : surface.parse_lowering_conformance_corpus_case_count;
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.long_tail_grammar_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_conformance_corpus_passed_case_count ==
          surface.parse_lowering_conformance_corpus_case_count &&
      surface.parse_lowering_conformance_corpus_failed_case_count == 0 &&
      parser_contract_snapshot_case_passed &&
      parse_artifact_handoff_case_passed &&
      parse_artifact_replay_case_passed &&
      parse_artifact_diagnostics_hardening_case_passed &&
      parse_artifact_edge_robustness_case_passed &&
      parse_recovery_determinism_hardening_case_passed &&
      semantic_handoff_case_passed &&
      lowering_boundary_case_passed;
  surface.parse_lowering_conformance_corpus_key =
      BuildObjc3ParseLoweringConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          parser_contract_snapshot_case_passed,
          parse_artifact_handoff_case_passed,
          parse_artifact_replay_case_passed,
          parse_artifact_diagnostics_hardening_case_passed,
          parse_artifact_edge_robustness_case_passed,
          parse_recovery_determinism_hardening_case_passed,
          semantic_handoff_case_passed,
          lowering_boundary_case_passed,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent);
  record.toolchain_runtime_ga_operations_conformance_corpus_consistent =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusConsistent(
          record.toolchain_runtime_ga_operations_conformance_matrix_consistent,
          record.toolchain_runtime_ga_operations_conformance_matrix_ready,
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key);
  record.toolchain_runtime_ga_operations_conformance_corpus_ready =
      IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
          record.toolchain_runtime_ga_operations_conformance_corpus_consistent,
          surface.parse_lowering_conformance_corpus_key);
  const std::string toolchain_runtime_ga_operations_conformance_corpus_key =
      BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
          surface.parse_lowering_conformance_matrix_consistent,
          surface.parse_lowering_conformance_corpus_consistent,
          surface.parse_lowering_conformance_matrix_key,
          surface.parse_lowering_conformance_corpus_key,
          surface.long_tail_grammar_conformance_matrix_key,
          record.toolchain_runtime_ga_operations_conformance_corpus_consistent,
          record.toolchain_runtime_ga_operations_conformance_corpus_ready);
  surface.parse_lowering_conformance_corpus_consistent =
      surface.parse_lowering_conformance_corpus_consistent &&
      record.toolchain_runtime_ga_operations_conformance_corpus_consistent;
  surface.parse_lowering_conformance_corpus_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  surface.long_tail_grammar_conformance_matrix_key +=
      ";toolchain_runtime_ga_operations_conformance_corpus_key=" +
      toolchain_runtime_ga_operations_conformance_corpus_key;
  record.parse_lowering_conformance_corpus_ready =
      surface.parse_lowering_conformance_corpus_consistent &&
      record.toolchain_runtime_ga_operations_conformance_corpus_ready;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      surface.parse_lowering_conformance_corpus_consistent &&
      surface.parse_lowering_conformance_corpus_case_count ==
          kObjc3ParseLoweringConformanceCorpusCaseCount &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.parse_lowering_conformance_corpus_key.empty();
  const bool parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate =
      surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent &&
      record.parse_lowering_conformance_corpus_ready &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_key =
      BuildObjc3DiagnosticGrammarHooksConformanceCorpusKey(
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_conformance_corpus_passed_case_count,
          surface.parse_lowering_conformance_corpus_failed_case_count,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_conformance_corpus_consistent,
          parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate);
  surface.parser_diagnostic_grammar_hooks_conformance_corpus_ready =
      parser_diagnostic_grammar_hooks_conformance_corpus_ready_gate &&
      !surface.parser_diagnostic_grammar_hooks_conformance_corpus_key.empty();
}
