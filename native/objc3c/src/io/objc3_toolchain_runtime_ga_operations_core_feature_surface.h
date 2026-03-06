#pragma once

#include <filesystem>
#include <sstream>
#include <string>

#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"

struct Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface {
  bool scaffold_ready = false;
  bool backend_route_deterministic = false;
  bool compile_status_success = false;
  bool backend_output_recorded = false;
  bool backend_dispatch_consistent = false;
  bool backend_output_path_deterministic = false;
  bool backend_output_payload_consistent = false;
  bool core_feature_expansion_ready = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
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
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string performance_quality_guardrails_key;
  std::string core_feature_key;
  std::string failure_reason;
};

inline bool Objc3ToolchainRuntimeGaOperationsHasSuffix(const std::string &value,
                                                       const std::string &suffix) {
  return value.size() >= suffix.size() &&
         value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

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

inline std::string BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-edge-case-compatibility:v1:"
      << "backend=" << surface.backend_route_key
      << ";backend_output_path_deterministic="
      << (surface.backend_output_path_deterministic ? "true" : "false")
      << ";backend_output_payload_consistent="
      << (surface.backend_output_payload_consistent ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false");
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
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";edge_case_compatibility_key_ready="
      << (!surface.edge_case_compatibility_key.empty() ? "true" : "false")
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
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
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
      surface.backend_route_key == "clang" ? "M228-D010-C001"
      : (surface.backend_route_key == "llvm-direct" ? "M228-D010-C002"
                                                     : "M228-D010-C000");
  const std::string reject_case_id =
      surface.backend_route_key == "clang" ? "M228-D010-R001"
      : (surface.backend_route_key == "llvm-direct" ? "M228-D010-R002"
                                                     : "M228-D010-R000");

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

inline Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
    const Objc3ToolchainRuntimeGaOperationsScaffold &scaffold,
    int compile_status,
    bool backend_output_recorded,
    const std::filesystem::path &backend_output_path,
    const std::string &backend_output_payload) {
  Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface surface;
  surface.scaffold_ready = scaffold.modular_split_ready;
  surface.backend_route_key = scaffold.backend_route_key;
  surface.scaffold_key = scaffold.scaffold_key;
  surface.backend_output_path = backend_output_path.generic_string();
  surface.backend_route_deterministic =
      scaffold.backend_route_key == "clang" || scaffold.backend_route_key == "llvm-direct";
  surface.compile_status_success = compile_status == 0;
  surface.backend_output_recorded = backend_output_recorded;
  surface.backend_dispatch_consistent = surface.compile_status_success && surface.backend_output_recorded;
  const std::string expected_backend_output_payload =
      scaffold.backend_route_key == "clang" ? "clang\n"
      : (scaffold.backend_route_key == "llvm-direct" ? "llvm-direct\n"
                                                      : std::string{});
  surface.backend_output_path_deterministic =
      backend_output_path.has_filename() &&
      Objc3ToolchainRuntimeGaOperationsHasSuffix(
          backend_output_path.filename().string(), ".object-backend.txt");
  surface.backend_output_payload_consistent =
      surface.backend_output_recorded &&
      !expected_backend_output_payload.empty() &&
      backend_output_payload == expected_backend_output_payload;
  surface.core_feature_expansion_ready =
      surface.backend_output_path_deterministic &&
      surface.backend_output_payload_consistent &&
      !surface.backend_output_path.empty();
  const bool edge_case_route_compatibility_consistent =
      scaffold.compile_route_ready &&
      ((scaffold.backend_route_key == "clang" &&
        scaffold.clang_backend_selected &&
        !scaffold.llvm_direct_backend_selected &&
        scaffold.clang_path_configured) ||
       (scaffold.backend_route_key == "llvm-direct" &&
        scaffold.llvm_direct_backend_selected &&
        !scaffold.clang_backend_selected &&
        scaffold.llc_path_configured &&
        scaffold.llvm_direct_backend_enabled));
  const bool edge_case_output_compatibility_consistent =
      surface.core_feature_expansion_ready &&
      surface.backend_output_recorded &&
      surface.backend_dispatch_consistent &&
      !surface.backend_output_path.empty();
  surface.edge_case_compatibility_consistent =
      edge_case_route_compatibility_consistent &&
      edge_case_output_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_consistent &&
      !surface.backend_route_key.empty() &&
      scaffold.object_artifact_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey(surface);
  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_consistent &&
      scaffold.compile_route_ready &&
      scaffold.object_artifact_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent;
  surface.edge_case_robustness_consistent =
      surface.edge_case_expansion_consistent &&
      surface.edge_case_compatibility_ready &&
      surface.backend_output_path_deterministic &&
      !surface.backend_output_path.empty();
  surface.edge_case_robustness_ready =
      surface.edge_case_robustness_consistent &&
      !surface.backend_route_key.empty();
  surface.edge_case_robustness_key =
      BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseRobustnessKey(surface);
  surface.diagnostics_hardening_consistent =
      surface.edge_case_expansion_consistent &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent &&
      surface.compile_status_success;
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_consistent &&
      surface.edge_case_robustness_ready &&
      !surface.backend_route_key.empty() &&
      !surface.backend_output_path.empty();
  surface.diagnostics_hardening_key =
      BuildObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningKey(surface);
  surface.diagnostics_hardening_key_ready =
      surface.diagnostics_hardening_ready &&
      !surface.diagnostics_hardening_key.empty() &&
      surface.diagnostics_hardening_key.find("backend=" + surface.backend_route_key) != std::string::npos &&
      surface.diagnostics_hardening_key.find(";backend_output_path=" + surface.backend_output_path) !=
          std::string::npos;
  surface.recovery_determinism_consistent =
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_key_ready &&
      surface.edge_case_robustness_consistent &&
      surface.backend_output_path_deterministic &&
      surface.backend_dispatch_consistent;
  surface.recovery_determinism_ready =
      surface.recovery_determinism_consistent &&
      surface.diagnostics_hardening_ready &&
      surface.backend_output_payload_consistent &&
      !surface.backend_route_key.empty() &&
      !surface.backend_output_path.empty();
  surface.recovery_determinism_key =
      BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(surface);
  surface.recovery_determinism_key_ready =
      surface.recovery_determinism_ready &&
      !surface.recovery_determinism_key.empty() &&
      surface.recovery_determinism_key.find("backend=" + surface.backend_route_key) != std::string::npos &&
      surface.recovery_determinism_key.find(";backend_output_path=" + surface.backend_output_path) !=
          std::string::npos &&
      surface.recovery_determinism_key.find(";diagnostics_hardening_key_ready=true") != std::string::npos;
  surface.conformance_matrix_consistent =
      surface.recovery_determinism_consistent &&
      surface.recovery_determinism_key_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent &&
      surface.backend_output_path_deterministic;
  surface.conformance_matrix_ready =
      surface.conformance_matrix_consistent &&
      surface.recovery_determinism_ready &&
      !surface.backend_route_key.empty() &&
      !surface.backend_output_path.empty();
  surface.conformance_matrix_key = BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(surface);
  surface.conformance_matrix_key_ready =
      surface.conformance_matrix_ready &&
      !surface.conformance_matrix_key.empty() &&
      surface.conformance_matrix_key.find("backend=" + surface.backend_route_key) != std::string::npos &&
      surface.conformance_matrix_key.find(";backend_output_path=" + surface.backend_output_path) !=
          std::string::npos &&
      surface.conformance_matrix_key.find(";recovery_determinism_key_ready=true") != std::string::npos;
  surface.conformance_corpus_consistent =
      surface.conformance_matrix_consistent &&
      surface.conformance_matrix_key_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent &&
      surface.backend_output_path_deterministic;
  surface.conformance_corpus_ready =
      surface.conformance_corpus_consistent &&
      surface.conformance_matrix_ready &&
      !surface.backend_route_key.empty() &&
      !surface.backend_output_path.empty();
  surface.conformance_corpus_key = BuildObjc3ToolchainRuntimeGaOperationsConformanceCorpusKey(surface);
  surface.conformance_corpus_key_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty() &&
      surface.conformance_corpus_key.find("backend=" + surface.backend_route_key) != std::string::npos &&
      surface.conformance_corpus_key.find(";backend_output_path=" + surface.backend_output_path) !=
          std::string::npos &&
      surface.conformance_corpus_key.find(";conformance_matrix_key_ready=true") != std::string::npos;
  surface.performance_quality_guardrails_consistent =
      surface.conformance_corpus_consistent &&
      surface.conformance_corpus_key_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent &&
      surface.backend_output_path_deterministic;
  surface.performance_quality_guardrails_ready =
      surface.performance_quality_guardrails_consistent &&
      surface.conformance_corpus_ready &&
      !surface.backend_route_key.empty() &&
      !surface.backend_output_path.empty();
  surface.performance_quality_guardrails_key =
      BuildObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsKey(surface);
  surface.performance_quality_guardrails_key_ready =
      surface.performance_quality_guardrails_ready &&
      !surface.performance_quality_guardrails_key.empty() &&
      surface.performance_quality_guardrails_key.find("backend=" + surface.backend_route_key) !=
          std::string::npos &&
      surface.performance_quality_guardrails_key.find(";backend_output_path=" + surface.backend_output_path) !=
          std::string::npos &&
      surface.performance_quality_guardrails_key.find(";conformance_corpus_key_ready=true") !=
          std::string::npos;
  surface.core_feature_impl_ready =
      surface.scaffold_ready &&
      surface.backend_route_deterministic &&
      surface.compile_status_success &&
      surface.backend_dispatch_consistent &&
      !surface.scaffold_key.empty();
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.core_feature_expansion_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.edge_case_compatibility_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && !surface.edge_case_compatibility_key.empty();
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.edge_case_robustness_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.edge_case_robustness_consistent;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && !surface.edge_case_robustness_key.empty();
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.diagnostics_hardening_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.diagnostics_hardening_key_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.recovery_determinism_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.recovery_determinism_key_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_matrix_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_matrix_key_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_corpus_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.conformance_corpus_key_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.performance_quality_guardrails_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.performance_quality_guardrails_key_ready;
  surface.core_feature_expansion_key = BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(surface);
  surface.core_feature_key = BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureKey(surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.scaffold_ready) {
    surface.failure_reason = scaffold.failure_reason.empty()
                                 ? "toolchain/runtime scaffold is not ready"
                                 : scaffold.failure_reason;
  } else if (!surface.backend_route_deterministic) {
    surface.failure_reason = "toolchain/runtime backend route key is not deterministic";
  } else if (!surface.compile_status_success) {
    surface.failure_reason = "toolchain/runtime backend object emission command failed";
  } else if (!surface.backend_output_recorded) {
    surface.failure_reason = "toolchain/runtime backend output marker was not recorded";
  } else if (!surface.backend_output_path_deterministic) {
    surface.failure_reason = "toolchain/runtime backend output path is not deterministic";
  } else if (!surface.backend_output_payload_consistent) {
    surface.failure_reason = "toolchain/runtime backend output marker payload is inconsistent";
  } else if (!surface.core_feature_expansion_ready) {
    surface.failure_reason = "toolchain/runtime core feature expansion is not ready";
  } else if (!surface.edge_case_compatibility_consistent) {
    surface.failure_reason = "toolchain/runtime edge-case compatibility is inconsistent";
  } else if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason = "toolchain/runtime edge-case compatibility is not ready";
  } else if (surface.edge_case_compatibility_key.empty()) {
    surface.failure_reason = "toolchain/runtime edge-case compatibility key is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason = "toolchain/runtime edge-case expansion is inconsistent";
  } else if (!surface.edge_case_robustness_consistent) {
    surface.failure_reason = "toolchain/runtime edge-case robustness is inconsistent";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason = "toolchain/runtime edge-case robustness is not ready";
  } else if (surface.edge_case_robustness_key.empty()) {
    surface.failure_reason = "toolchain/runtime edge-case robustness key is not ready";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason = "toolchain/runtime diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason = "toolchain/runtime diagnostics hardening is not ready";
  } else if (!surface.diagnostics_hardening_key_ready) {
    surface.failure_reason = "toolchain/runtime diagnostics hardening key is not ready";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason = "toolchain/runtime recovery and determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_ready) {
    surface.failure_reason = "toolchain/runtime recovery and determinism hardening is not ready";
  } else if (!surface.recovery_determinism_key_ready) {
    surface.failure_reason = "toolchain/runtime recovery and determinism hardening key is not ready";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason = "toolchain/runtime conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_ready) {
    surface.failure_reason = "toolchain/runtime conformance matrix is not ready";
  } else if (!surface.conformance_matrix_key_ready) {
    surface.failure_reason = "toolchain/runtime conformance matrix key is not ready";
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason = "toolchain/runtime conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_ready) {
    surface.failure_reason = "toolchain/runtime conformance corpus is not ready";
  } else if (!surface.conformance_corpus_key_ready) {
    surface.failure_reason = "toolchain/runtime conformance corpus key is not ready";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.failure_reason = "toolchain/runtime performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_ready) {
    surface.failure_reason = "toolchain/runtime performance quality guardrails are not ready";
  } else if (!surface.performance_quality_guardrails_key_ready) {
    surface.failure_reason = "toolchain/runtime performance quality guardrails key is not ready";
  } else if (surface.scaffold_key.empty()) {
    surface.failure_reason = "toolchain/runtime scaffold key is empty";
  } else {
    surface.failure_reason = "toolchain/runtime core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (!surface.diagnostics_hardening_consistent) {
    reason = "toolchain/runtime diagnostics hardening is inconsistent";
    return false;
  }
  if (!surface.diagnostics_hardening_ready) {
    reason = "toolchain/runtime diagnostics hardening is not ready";
    return false;
  }
  if (!surface.diagnostics_hardening_key_ready) {
    reason = "toolchain/runtime diagnostics hardening key is not ready";
    return false;
  }

  reason.clear();
  return true;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (!surface.recovery_determinism_consistent) {
    reason = "toolchain/runtime recovery and determinism hardening is inconsistent";
    return false;
  }
  if (!surface.recovery_determinism_ready) {
    reason = "toolchain/runtime recovery and determinism hardening is not ready";
    return false;
  }
  if (!surface.recovery_determinism_key_ready) {
    reason = "toolchain/runtime recovery and determinism hardening key is not ready";
    return false;
  }

  reason.clear();
  return true;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (!surface.conformance_matrix_consistent) {
    reason = "toolchain/runtime conformance matrix is inconsistent";
    return false;
  }
  if (!surface.conformance_matrix_ready) {
    reason = "toolchain/runtime conformance matrix is not ready";
    return false;
  }
  if (!surface.conformance_matrix_key_ready) {
    reason = "toolchain/runtime conformance matrix key is not ready";
    return false;
  }

  reason.clear();
  return true;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsConformanceCorpusReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (!surface.conformance_corpus_consistent) {
    reason = "toolchain/runtime conformance corpus is inconsistent";
    return false;
  }
  if (!surface.conformance_corpus_ready) {
    reason = "toolchain/runtime conformance corpus is not ready";
    return false;
  }
  if (!surface.conformance_corpus_key_ready) {
    reason = "toolchain/runtime conformance corpus key is not ready";
    return false;
  }

  reason.clear();
  return true;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsPerformanceQualityGuardrailsReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (!surface.performance_quality_guardrails_consistent) {
    reason = "toolchain/runtime performance quality guardrails are inconsistent";
    return false;
  }
  if (!surface.performance_quality_guardrails_ready) {
    reason = "toolchain/runtime performance quality guardrails are not ready";
    return false;
  }
  if (!surface.performance_quality_guardrails_key_ready) {
    reason = "toolchain/runtime performance quality guardrails key is not ready";
    return false;
  }

  reason.clear();
  return true;
}

inline bool IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty() ? "toolchain/runtime core feature implementation surface not ready"
                                          : surface.failure_reason;
  return false;
}

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
bool cross_lane_integration_sync_consistent = false;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
bool cross_lane_integration_sync_ready = false;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
bool cross_lane_integration_sync_key_ready = false;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
std::string cross_lane_integration_sync_key;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
toolchain-runtime-ga-operations-cross-lane-integration-sync:v1:
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.cross_lane_integration_sync_consistent =
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.cross_lane_integration_sync_ready =
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.cross_lane_integration_sync_key =
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.cross_lane_integration_sync_key_ready =
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.cross_lane_integration_sync_ready;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.cross_lane_integration_sync_key_ready;
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
;cross_lane_integration_sync_consistent=
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
;cross_lane_integration_sync_ready=
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
;cross_lane_integration_sync_key_ready=
*/

/* auto-contract-snippet check_m228_d012_object_emission_link_path_reliability_cross_lane_integration_sync_contract:core_surface_header
surface.cross_lane_integration_sync_key.find(";conformance_corpus_key_ready=true") !=
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
bool docs_operator_runbook_sync_consistent = false;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
bool docs_operator_runbook_sync_ready = false;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
bool docs_operator_runbook_sync_key_ready = false;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
std::string docs_operator_runbook_sync_key;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
toolchain-runtime-ga-operations-docs-operator-runbook-sync:v1:
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.docs_operator_runbook_sync_consistent =
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.docs_operator_runbook_sync_ready =
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.docs_operator_runbook_sync_key =
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.docs_operator_runbook_sync_key_ready =
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.docs_operator_runbook_sync_ready;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.docs_operator_runbook_sync_key_ready;
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
;docs_operator_runbook_sync_consistent=
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
;docs_operator_runbook_sync_ready=
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
;docs_operator_runbook_sync_key_ready=
*/

/* auto-contract-snippet check_m228_d013_object_emission_link_path_reliability_docs_operator_runbook_sync_contract:core_surface_header
surface.docs_operator_runbook_sync_key.find(";conformance_corpus_key_ready=true") !=
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
bool release_candidate_and_replay_dry_run_consistent = false;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
bool release_candidate_and_replay_dry_run_ready = false;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
bool release_candidate_and_replay_dry_run_key_ready = false;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
std::string release_candidate_and_replay_dry_run_key;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
toolchain-runtime-ga-operations-release-candidate-and-replay-dry-run:v1:
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.release_candidate_and_replay_dry_run_consistent =
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.release_candidate_and_replay_dry_run_ready =
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.release_candidate_and_replay_dry_run_key =
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.release_candidate_and_replay_dry_run_key_ready =
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.release_candidate_and_replay_dry_run_ready;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.release_candidate_and_replay_dry_run_key_ready;
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
;release_candidate_and_replay_dry_run_consistent=
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
;release_candidate_and_replay_dry_run_ready=
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
;release_candidate_and_replay_dry_run_key_ready=
*/

/* auto-contract-snippet check_m228_d014_object_emission_link_path_reliability_release_candidate_and_replay_dry_run_contract:core_surface_header
surface.release_candidate_and_replay_dry_run_key.find(";conformance_corpus_key_ready=true") !=
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
bool advanced_core_workpack_shard1_consistent = false;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
bool advanced_core_workpack_shard1_ready = false;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
bool advanced_core_workpack_shard1_key_ready = false;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
std::string advanced_core_workpack_shard1_key;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
toolchain-runtime-ga-operations-advanced-core-workpack-shard1:v1:
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.advanced_core_workpack_shard1_consistent =
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.advanced_core_workpack_shard1_ready =
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.advanced_core_workpack_shard1_key =
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.advanced_core_workpack_shard1_key_ready =
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.advanced_core_workpack_shard1_ready;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.advanced_core_workpack_shard1_key_ready;
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
;advanced_core_workpack_shard1_consistent=
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
;advanced_core_workpack_shard1_ready=
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
;advanced_core_workpack_shard1_key_ready=
*/

/* auto-contract-snippet check_m228_d015_object_emission_link_path_reliability_advanced_core_workpack_shard1_contract:core_surface_header
surface.advanced_core_workpack_shard1_key.find(";conformance_corpus_key_ready=true") !=
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
bool integration_closeout_and_gate_signoff_consistent = false;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
bool integration_closeout_and_gate_signoff_ready = false;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
bool integration_closeout_and_gate_signoff_key_ready = false;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
std::string integration_closeout_and_gate_signoff_key;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
toolchain-runtime-ga-operations-integration-closeout-and-gate-signoff:v1:
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.integration_closeout_and_gate_signoff_consistent =
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.integration_closeout_and_gate_signoff_ready =
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.integration_closeout_and_gate_signoff_key =
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.integration_closeout_and_gate_signoff_key_ready =
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.integration_closeout_and_gate_signoff_ready;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.integration_closeout_and_gate_signoff_key_ready;
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
;integration_closeout_and_gate_signoff_consistent=
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
;integration_closeout_and_gate_signoff_ready=
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
;integration_closeout_and_gate_signoff_key_ready=
*/

/* auto-contract-snippet check_m228_d016_object_emission_link_path_reliability_integration_closeout_and_gate_signoff_contract:core_surface_header
surface.integration_closeout_and_gate_signoff_key.find(";conformance_corpus_key_ready=true") !=
*/
