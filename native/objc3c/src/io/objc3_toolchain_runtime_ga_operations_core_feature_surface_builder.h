#pragma once

#include <filesystem>
#include <string>

#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface_keys.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface_types.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "support/objc3_string_predicates.h"

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
      objc3c::support::EndsWith(
          backend_output_path.filename().string(), ".object-backend.txt");
  surface.backend_output_payload_consistent =
      surface.backend_output_recorded &&
      !expected_backend_output_payload.empty() &&
      backend_output_payload == expected_backend_output_payload;
  surface.core_feature_expansion_ready =
      surface.backend_output_path_deterministic &&
      surface.backend_output_payload_consistent &&
      !surface.backend_output_path.empty();
  const bool edge_case_route_consistency_contract_consistent =
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
  const bool edge_case_output_consistency_contract_consistent =
      surface.core_feature_expansion_ready &&
      surface.backend_output_recorded &&
      surface.backend_dispatch_consistent &&
      !surface.backend_output_path.empty();
  surface.edge_case_consistency_contract_consistent =
      edge_case_route_consistency_contract_consistent &&
      edge_case_output_consistency_contract_consistent;
  surface.edge_case_consistency_contract_ready =
      surface.edge_case_consistency_contract_consistent &&
      !surface.backend_route_key.empty() &&
      scaffold.object_artifact_ready;
  surface.edge_case_consistency_contract_key =
      BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseConsistencyKey(surface);
  surface.edge_case_expansion_consistent =
      surface.edge_case_consistency_contract_consistent &&
      scaffold.compile_route_ready &&
      scaffold.object_artifact_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent;
  surface.edge_case_robustness_consistent =
      surface.edge_case_expansion_consistent &&
      surface.edge_case_consistency_contract_ready &&
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
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.edge_case_consistency_contract_ready;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      !surface.edge_case_consistency_contract_key.empty();
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
  } else if (!surface.edge_case_consistency_contract_consistent) {
    surface.failure_reason =
        "toolchain/runtime edge-case consistency contract is inconsistent";
  } else if (!surface.edge_case_consistency_contract_ready) {
    surface.failure_reason =
        "toolchain/runtime edge-case consistency contract is not ready";
  } else if (surface.edge_case_consistency_contract_key.empty()) {
    surface.failure_reason =
        "toolchain/runtime edge-case consistency contract key is not ready";
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
