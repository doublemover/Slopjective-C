#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_advanced_readiness_keys.h"
#include "support/objc3_string_predicates.h"

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_diagnostics_consistent &&
         toolchain_runtime_ga_operations_advanced_diagnostics_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_conformance_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_diagnostics_key,
             "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(
    bool toolchain_runtime_ga_operations_advanced_diagnostics_consistent,
    bool toolchain_runtime_ga_operations_advanced_diagnostics_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_diagnostics_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready) {
  const bool advanced_diagnostics_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_diagnostics_key,
          "toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") +
         (toolchain_runtime_ga_operations_advanced_diagnostics_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_diagnostics_ready=" +
         (toolchain_runtime_ga_operations_advanced_diagnostics_ready ? "true" : "false") +
         ";advanced_diagnostics_key_shape_deterministic=" +
         (advanced_diagnostics_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_conformance_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_conformance_ready ? "true" : "false");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationConsistent(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_conformance_consistent &&
         toolchain_runtime_ga_operations_advanced_conformance_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_integration_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_conformance_key,
             "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationKey(
    bool toolchain_runtime_ga_operations_advanced_conformance_consistent,
    bool toolchain_runtime_ga_operations_advanced_conformance_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_conformance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready) {
  const bool advanced_conformance_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_conformance_key,
          "toolchain_runtime_ga_operations_advanced_diagnostics_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_conformance_consistent=") +
         (toolchain_runtime_ga_operations_advanced_conformance_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_conformance_ready=" +
         (toolchain_runtime_ga_operations_advanced_conformance_ready ? "true" : "false") +
         ";advanced_conformance_key_shape_deterministic=" +
         (advanced_conformance_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_integration_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_integration_ready ? "true" : "false");
}
