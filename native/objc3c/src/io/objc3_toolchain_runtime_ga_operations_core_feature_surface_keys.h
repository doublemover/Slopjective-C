#pragma once

#include <sstream>
#include <string>

#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface_types.h"

inline std::string BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-core-feature-expansion:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_payload_consistent="
      << (surface.backend_output_payload_consistent ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path;
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseConsistencyKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-edge-case-consistency:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_payload_consistent="
      << (surface.backend_output_payload_consistent ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";edge_case_consistency_contract_consistent="
      << (surface.edge_case_consistency_contract_consistent ? "true" : "false")
      << ";edge_case_consistency_contract_ready="
      << (surface.edge_case_consistency_contract_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-core-feature:v1:"
      << "backend=" << surface.backend_route_key
      << ";scaffold_ready=" << (surface.scaffold_ready ? "true" : "false")
      << ";backend_route_deterministic=" << (surface.backend_route_deterministic ? "true" : "false")
      << ";compile_status_success=" << (surface.compile_status_success ? "true" : "false")
      << ";backend_output_recorded=" << (surface.backend_output_recorded ? "true" : "false")
      << ";backend_dispatch_consistent=" << (surface.backend_dispatch_consistent ? "true" : "false")
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_payload_consistent="
      << (surface.backend_output_payload_consistent ? "true" : "false")
      << ";core_feature_expansion_ready=" << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";edge_case_consistency_contract_consistent="
      << (surface.edge_case_consistency_contract_consistent ? "true" : "false")
      << ";edge_case_consistency_contract_ready="
      << (surface.edge_case_consistency_contract_ready ? "true" : "false")
      << ";edge_case_consistency_contract_key_ready="
      << (!surface.edge_case_consistency_contract_key.empty() ? "true" : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_consistent="
      << (surface.edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";edge_case_robustness_key_ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key_ready="
      << (surface.diagnostics_hardening_key_ready ? "true" : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery_determinism_key_ready="
      << (surface.recovery_determinism_key_ready ? "true" : "false")
      << ";conformance_matrix_consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance_matrix_key_ready="
      << (surface.conformance_matrix_key_ready ? "true" : "false")
      << ";conformance_corpus_consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance_corpus_ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance_corpus_key_ready="
      << (surface.conformance_corpus_key_ready ? "true" : "false")
      << ";performance_quality_guardrails_consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance_quality_guardrails_ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";performance_quality_guardrails_key_ready="
      << (surface.performance_quality_guardrails_key_ready ? "true" : "false")
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseRobustnessKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-edge-case-robustness:v1:"
      << "backend=" << surface.backend_route_key
      << ";edge_case_consistency_contract_ready="
      << (surface.edge_case_consistency_contract_ready ? "true" : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_consistent="
      << (surface.edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-diagnostics-hardening:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-recovery-determinism-hardening:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key_ready="
      << (surface.diagnostics_hardening_key_ready ? "true" : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-conformance-matrix:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery_determinism_key_ready="
      << (surface.recovery_determinism_key_ready ? "true" : "false")
      << ";conformance_matrix_consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  const std::string accept_case_id =
      surface.backend_route_key == "clang" ? "objc3c.toolchain.backendroute.capabilities.clang.accept.v1"
      : (surface.backend_route_key == "llvm-direct" ? "objc3c.toolchain.backendroute.capabilities.llvmdirect.accept.v1"
                                                     : "objc3c.toolchain.backendroute.capabilities.generic.accept.v1");
  const std::string reject_case_id =
      surface.backend_route_key == "clang" ? "objc3c.toolchain.backendroute.capabilities.clang.reject.v1"
      : (surface.backend_route_key == "llvm-direct" ? "objc3c.toolchain.backendroute.capabilities.llvmdirect.reject.v1"
                                                     : "objc3c.toolchain.backendroute.capabilities.generic.reject.v1");

  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-conformance-corpus:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance_matrix_key_ready="
      << (surface.conformance_matrix_key_ready ? "true" : "false")
      << ";conformance_corpus_consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance_corpus_ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";accept_case_id=" << accept_case_id
      << ";reject_case_id=" << reject_case_id;
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-performance-quality-guardrails:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_path=" << surface.backend_output_path
      << ";conformance_corpus_ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance_corpus_key_ready="
      << (surface.conformance_corpus_key_ready ? "true" : "false")
      << ";performance_quality_guardrails_consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance_quality_guardrails_ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false");
  return key.str();
}
