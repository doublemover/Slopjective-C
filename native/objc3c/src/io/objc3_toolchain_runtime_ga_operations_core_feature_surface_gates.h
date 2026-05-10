#pragma once

#include <string>

#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface_types.h"

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
