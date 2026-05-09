#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendPipelineReadinessMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3OwnershipAwareLoweringBehaviorScaffold
        &ownership_aware_lowering_behavior_scaffold,
    const Objc3IREmissionCompletenessScaffold
        &ir_emission_completeness_scaffold,
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface
        &lowering_pipeline_pass_graph_core_feature_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface);

}  // namespace objc3::artifacts::frontend
