#pragma once

#include <string>

#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface_keys.h"
#include "pipeline/objc3_semantic_stability_surface_builder_failure.h"
#include "pipeline/objc3_semantic_stability_surface_builder_facts.h"
#include "pipeline/objc3_semantic_stability_surface_builder_inputs.h"
#include "pipeline/objc3_semantic_stability_surface_builder_publication.h"

inline Objc3SemanticStabilityCoreFeatureImplementationSurface
BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3SemanticStabilitySpecDeltaClosureScaffold &scaffold) {
  Objc3SemanticStabilityCoreFeatureImplementationSurface surface;
  PopulateObjc3SemanticStabilitySurfaceInputs(surface,
                                              typed_surface,
                                              parse_surface,
                                              scaffold);
  Objc3SemanticStabilityReadinessFacts facts =
      BuildObjc3SemanticStabilityReadinessFacts(surface, parse_surface);
  PublishObjc3SemanticStabilitySurfaceReadiness(surface, facts);
  PublishObjc3SemanticStabilitySurfaceKeys(surface, parse_surface, facts);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  PublishObjc3SemanticStabilitySurfaceFailureReason(surface, facts);
  return surface;
}

inline bool IsObjc3SemanticStabilityCoreFeatureImplementationSurfaceReady(
    const Objc3SemanticStabilityCoreFeatureImplementationSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "semantic stability core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
