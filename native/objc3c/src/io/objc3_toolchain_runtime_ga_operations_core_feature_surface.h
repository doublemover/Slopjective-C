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
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool core_feature_impl_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string backend_output_path;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
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
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";edge_case_robustness_key_ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key_ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
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
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningKey(
    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "toolchain-runtime-ga-operations-diagnostics-hardening:v1:"
      << "backend=" << surface.backend_route_key
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false");
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
  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_consistent &&
      scaffold.compile_route_ready &&
      scaffold.object_artifact_ready &&
      surface.backend_dispatch_consistent &&
      surface.backend_output_payload_consistent;
  surface.edge_case_robustness_ready =
      surface.edge_case_expansion_consistent &&
      surface.edge_case_compatibility_ready &&
      surface.backend_output_path_deterministic &&
      !surface.backend_output_path.empty();
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
  surface.core_feature_impl_ready =
      surface.scaffold_ready &&
      surface.backend_route_deterministic &&
      surface.compile_status_success &&
      surface.backend_dispatch_consistent &&
      !surface.scaffold_key.empty();
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.core_feature_expansion_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.edge_case_compatibility_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.edge_case_robustness_ready;
  surface.core_feature_impl_ready = surface.core_feature_impl_ready && surface.diagnostics_hardening_ready;
  surface.core_feature_expansion_key = BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(surface);
  surface.edge_case_compatibility_key =
      BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey(surface);
  surface.edge_case_robustness_key =
      BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseRobustnessKey(surface);
  surface.diagnostics_hardening_key =
      BuildObjc3ToolchainRuntimeGaOperationsDiagnosticsHardeningKey(surface);
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
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason = "toolchain/runtime edge-case expansion is inconsistent";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason = "toolchain/runtime edge-case robustness is not ready";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason = "toolchain/runtime diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason = "toolchain/runtime diagnostics hardening is not ready";
  } else if (surface.scaffold_key.empty()) {
    surface.failure_reason = "toolchain/runtime scaffold key is empty";
  } else {
    surface.failure_reason = "toolchain/runtime core feature implementation is not ready";
  }

  return surface;
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
