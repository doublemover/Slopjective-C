#include "driver/objc3_driver_toolchain_runtime_gate.h"

#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"

bool ValidateObjc3DriverToolchainRuntimeCoreFeature(
    const Objc3DriverObjectBackendResult &object_backend,
    int compile_status,
    std::string &reason) {
  const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface surface =
      BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
          object_backend.scaffold,
          compile_status,
          object_backend.backend_output_recorded,
          object_backend.backend_out,
          object_backend.backend_output_payload);
  return IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(surface,
                                                                    reason);
}
