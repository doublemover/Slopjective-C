#pragma once

#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"

namespace objc3_ir_emission_core_feature_surface {

void PublishObjc3IREmissionCoreFeatureBaseReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3IREmissionCompletenessScaffold &scaffold);

void PublishObjc3IREmissionCoreFeatureQualityReadiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface);

void PublishObjc3IREmissionCoreFeatureAdvancedShard1Readiness(
    Objc3IREmissionCoreFeatureImplementationSurface &surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface);

}  // namespace objc3_ir_emission_core_feature_surface
