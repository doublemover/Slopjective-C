#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_lowering_runtime_stability_keys.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_failure.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_facts.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_inputs.h"
#include "pipeline/objc3_lowering_runtime_stability_surface_builder_publication.h"

inline Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface
BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3LoweringRuntimeStabilityInvariantScaffold &scaffold) {
  Objc3LoweringRuntimeStabilityCoreFeatureImplementationSurface surface;
  PopulateObjc3LoweringRuntimeStabilitySurfaceInputs(surface,
                                                     typed_surface,
                                                     parse_surface,
                                                     scaffold);
  const Objc3LoweringRuntimeStabilityReadinessFacts facts =
      BuildObjc3LoweringRuntimeStabilityReadinessFacts(surface,
                                                       typed_surface,
                                                       parse_surface);
  PublishObjc3LoweringRuntimeStabilitySurfaceReadiness(surface, facts);
  PublishObjc3LoweringRuntimeStabilitySurfaceKeys(surface,
                                                  parse_surface,
                                                  facts);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  PublishObjc3LoweringRuntimeStabilitySurfaceFailureReason(surface, facts);
  return surface;
}
