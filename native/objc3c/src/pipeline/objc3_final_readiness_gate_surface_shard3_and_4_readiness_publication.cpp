#include "pipeline/objc3_final_readiness_gate_surface_readiness_helpers.h"
#include "pipeline/readiness/objc3_final_readiness_gate_advanced_keys.h"

namespace objc3_final_readiness_gate_surface {

void PublishObjc3FinalReadinessGateAdvancedShard3And4Readiness(
    Objc3FinalReadinessGateCoreFeatureImplementationSurface &surface,
    const Objc3FinalReadinessGateLaneSurface &lane_a_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_b_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_c_surface,
    const Objc3FinalReadinessGateLaneSurface &lane_d_surface) {
#include "pipeline/objc3_final_readiness_gate_surface_shard3_and_4_readiness_publication_core_shards.inc"

#include "pipeline/objc3_final_readiness_gate_surface_shard3_and_4_readiness_publication_diagnostics_conformance.inc"

#include "pipeline/objc3_final_readiness_gate_surface_shard3_and_4_readiness_publication_integration_performance.inc"

#include "pipeline/objc3_final_readiness_gate_surface_shard3_and_4_readiness_publication_shard4_closeout.inc"
}

}  // namespace objc3_final_readiness_gate_surface
