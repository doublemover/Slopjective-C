#include "pipeline/objc3_final_readiness_gate_surface_owners.h"

namespace objc3_final_readiness_gate_surface {

void PopulateObjc3FinalReadinessGateSurfaceEvidence(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateCoreFeatureScaffold &scaffold,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface) {
  surface.governance_contract_ready = scaffold.governance_contract_ready;
  surface.modular_split_ready = scaffold.modular_split_ready;
  surface.lane_a_core_feature_ready = lane_a_surface.core_feature_ready;
  surface.lane_b_core_feature_ready = lane_b_surface.core_feature_impl_ready;
  surface.lane_c_core_feature_ready = lane_c_surface.core_feature_impl_ready;
  surface.lane_d_core_feature_ready = lane_d_surface.core_feature_impl_ready;
  surface.governance_key = scaffold.governance_key;
  surface.modular_split_key = scaffold.modular_split_key;
  surface.lane_a_key = lane_a_surface.core_feature_key;
  surface.lane_b_key = lane_b_surface.core_feature_key;
  surface.lane_c_key = lane_c_surface.core_feature_key;
  surface.lane_d_key = lane_d_surface.core_feature_key;
}

}  // namespace objc3_final_readiness_gate_surface
