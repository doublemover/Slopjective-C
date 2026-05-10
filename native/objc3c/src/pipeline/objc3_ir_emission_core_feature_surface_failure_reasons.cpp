#include "pipeline/objc3_ir_emission_core_feature_surface_failure_reason_helpers.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureSurfaceFailureReasons(
    Objc3IREmissionCoreFeatureImplementationSurface &surface) {
  PublishObjc3IREmissionCoreFeatureBaseFailureReasons(surface);
  PublishObjc3IREmissionCoreFeatureAdvancedFailureReasons(surface);

  if (surface.core_feature_impl_ready) {
    return;
  }

  if (!surface.modular_split_ready) {
    surface.failure_reason =
        "IR emission completeness modular split scaffold is not ready";
  } else if (!surface.metadata_transport_ready) {
    surface.failure_reason =
        "IR emission completeness metadata transport is not ready";
  } else if (!surface.pass_graph_core_feature_ready) {
    surface.failure_reason = "pass-graph core feature is not ready";
  } else if (!surface.runtime_boundary_handoff_ready) {
    surface.failure_reason =
        "runtime boundary handoff replay surface is not ready";
  } else if (!surface.direct_ir_entrypoint_ready) {
    surface.failure_reason = "direct IR entrypoint is not ready";
  } else {
    surface.failure_reason =
        "IR emission core feature implementation surface is not ready";
  }
}

}  // namespace objc3_ir_emission_core_feature_surface
