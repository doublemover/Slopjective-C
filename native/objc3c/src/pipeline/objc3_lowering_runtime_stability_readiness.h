#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

inline bool IsObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurfaceReady(
    const Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering/runtime core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
