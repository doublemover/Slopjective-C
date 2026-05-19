#include <string>

#include "pipeline/readiness/objc3_toolchain_runtime_ga_operations_advanced_readiness_keys.h"
#include "support/objc3_string_predicates.h"

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceConsistent(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_integration_consistent &&
         toolchain_runtime_ga_operations_advanced_integration_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_integration_key,
             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceReady(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_integration_key,
             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceKey(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready) {
  const bool advanced_integration_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_integration_key,
          "toolchain_runtime_ga_operations_advanced_conformance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_integration_consistent=") +
         (toolchain_runtime_ga_operations_advanced_integration_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_integration_ready=" +
         (toolchain_runtime_ga_operations_advanced_integration_ready ? "true" : "false") +
         ";advanced_integration_key_shape_deterministic=" +
         (advanced_integration_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         toolchain_runtime_ga_operations_advanced_performance_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready) {
  const bool advanced_performance_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_performance_key,
          "toolchain_runtime_ga_operations_advanced_integration_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_performance_consistent=") +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_performance_ready=" +
         (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false") +
         ";advanced_performance_key_shape_deterministic=" +
         (advanced_performance_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false");
}

bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         toolchain_runtime_ga_operations_advanced_core_shard2_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_integration_closeout_signoff_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

std::string BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready) {
  const bool advanced_core_shard2_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          "toolchain_runtime_ga_operations_advanced_performance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_core_shard2_consistent=") +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_core_shard2_ready=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false") +
         ";advanced_core_shard2_key_shape_deterministic=" +
         (advanced_core_shard2_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_integration_closeout_signoff_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_integration_closeout_signoff_ready ? "true" : "false");
}
