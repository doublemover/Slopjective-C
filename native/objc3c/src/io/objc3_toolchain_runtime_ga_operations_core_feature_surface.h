#pragma once

#include <sstream>
#include <string>

#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"

struct Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface {
  bool scaffold_ready = false;
  bool backend_route_deterministic = false;
  bool compile_status_success = false;
  bool backend_output_recorded = false;
  bool backend_dispatch_consistent = false;
  bool core_feature_impl_ready = false;
  std::string backend_route_key;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string failure_reason;
};

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
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
    const Objc3ToolchainRuntimeGaOperationsScaffold &scaffold,
    int compile_status,
    bool backend_output_recorded) {
  Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface surface;
  surface.scaffold_ready = scaffold.modular_split_ready;
  surface.backend_route_key = scaffold.backend_route_key;
  surface.scaffold_key = scaffold.scaffold_key;
  surface.backend_route_deterministic =
      scaffold.backend_route_key == "clang" || scaffold.backend_route_key == "llvm-direct";
  surface.compile_status_success = compile_status == 0;
  surface.backend_output_recorded = backend_output_recorded;
  surface.backend_dispatch_consistent = surface.compile_status_success && surface.backend_output_recorded;
  surface.core_feature_impl_ready =
      surface.scaffold_ready &&
      surface.backend_route_deterministic &&
      surface.compile_status_success &&
      surface.backend_dispatch_consistent &&
      !surface.scaffold_key.empty();
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
