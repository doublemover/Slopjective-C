#pragma once

#include <string>

struct Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface {
  bool scaffold_ready = false;
  bool backend_route_deterministic = false;
  bool compile_status_success = false;
  bool backend_output_recorded = false;
  bool backend_dispatch_consistent = false;
  bool backend_output_path_deterministic = false;
  bool backend_output_payload_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_consistency_contract_consistent = false;
  bool edge_case_consistency_contract_ready = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool diagnostics_hardening_key_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool recovery_determinism_key_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_matrix_key_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool conformance_corpus_key_ready = false;
  bool performance_quality_guardrails_consistent = false;
  bool performance_quality_guardrails_ready = false;
  bool performance_quality_guardrails_key_ready = false;
  bool core_feature_impl_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string backend_output_path;
  std::string core_feature_expansion_key;
  std::string edge_case_consistency_contract_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string core_feature_key;
  std::string failure_reason;
};
