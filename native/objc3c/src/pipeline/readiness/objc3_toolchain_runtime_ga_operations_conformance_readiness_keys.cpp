#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_readiness_keys.h"
#include "support/objc3_string_predicates.h"

bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixConsistent(
    bool toolchain_runtime_ga_operations_recovery_determinism_consistent,
    bool toolchain_runtime_ga_operations_recovery_determinism_ready,
    bool parse_lowering_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_ready,
    const std::string &parse_recovery_determinism_hardening_key,
    const std::string &long_tail_grammar_recovery_determinism_key) {
  return toolchain_runtime_ga_operations_recovery_determinism_consistent &&
         toolchain_runtime_ga_operations_recovery_determinism_ready &&
         parse_lowering_conformance_matrix_consistent &&
         long_tail_grammar_conformance_matrix_consistent &&
         long_tail_grammar_conformance_matrix_ready &&
         parse_recovery_determinism_hardening_key.find(
             "toolchain_runtime_ga_operations_recovery_determinism_key=") != std::string::npos &&
         long_tail_grammar_recovery_determinism_key.find(
             "toolchain_runtime_ga_operations_recovery_determinism_key=") != std::string::npos;
}

bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_matrix_consistent &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
    bool parse_lowering_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_consistent,
    bool long_tail_grammar_conformance_matrix_ready,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    bool toolchain_runtime_ga_operations_conformance_matrix_ready) {
  const bool parse_lowering_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_matrix_consistent=") +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_consistent=" +
         (long_tail_grammar_conformance_matrix_consistent ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_ready=" +
         (long_tail_grammar_conformance_matrix_ready ? "true" : "false") +
         ";parse_lowering_conformance_matrix_key_shape_deterministic=" +
         (parse_lowering_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_conformance_matrix_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_conformance_matrix_ready ? "true" : "false");
}

bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusConsistent(
    bool toolchain_runtime_ga_operations_conformance_matrix_consistent,
    bool toolchain_runtime_ga_operations_conformance_matrix_ready,
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_matrix_consistent &&
         toolchain_runtime_ga_operations_conformance_matrix_ready &&
         parse_lowering_conformance_matrix_consistent &&
         parse_lowering_conformance_corpus_consistent &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_matrix_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_corpus_key) {
  return toolchain_runtime_ga_operations_conformance_corpus_consistent &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    const std::string &parse_lowering_conformance_matrix_key,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    bool toolchain_runtime_ga_operations_conformance_corpus_ready) {
  const bool parse_lowering_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_conformance_matrix_key,
          "case_count=");
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_matrix_consistent=") +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";parse_lowering_conformance_matrix_key_shape_deterministic=" +
         (parse_lowering_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_conformance_corpus_key_shape_deterministic=" +
         (parse_lowering_conformance_corpus_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_conformance_corpus_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_conformance_corpus_ready ? "true" : "false");
}

bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsConsistent(
    bool toolchain_runtime_ga_operations_conformance_corpus_consistent,
    bool toolchain_runtime_ga_operations_conformance_corpus_ready,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    const std::string &long_tail_grammar_conformance_matrix_key) {
  return toolchain_runtime_ga_operations_conformance_corpus_consistent &&
         toolchain_runtime_ga_operations_conformance_corpus_ready &&
         parse_lowering_conformance_corpus_consistent &&
         parse_lowering_performance_quality_guardrails_consistent &&
         objc3c::support::StartsWith(
             parse_lowering_conformance_corpus_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_conformance_matrix_key,
             "conformance_matrix_case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_performance_quality_guardrails_consistent &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent,
    const std::string &parse_lowering_conformance_corpus_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    const std::string &long_tail_grammar_conformance_matrix_key,
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_consistent,
    bool toolchain_runtime_ga_operations_performance_quality_guardrails_ready) {
  const bool parse_lowering_conformance_corpus_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_conformance_corpus_key,
          "case_count=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  const bool long_tail_grammar_conformance_matrix_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_conformance_matrix_key,
          "conformance_matrix_case_count=");
  return std::string("parse_lowering_conformance_corpus_consistent=") +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_key_shape_deterministic=" +
         (parse_lowering_conformance_corpus_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_conformance_matrix_key_shape_deterministic=" +
         (long_tail_grammar_conformance_matrix_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_performance_quality_guardrails_consistent ? "true"
                                                                                    : "false") +
         ";ready=" +
         (toolchain_runtime_ga_operations_performance_quality_guardrails_ready ? "true" : "false");
}
