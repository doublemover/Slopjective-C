#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface_owners.h"

namespace objc3_lowering_pipeline_pass_graph_core_feature {

void PopulateEvidence(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const Objc3LoweringPipelinePassGraphScaffold &scaffold =
      pipeline_result.lowering_pipeline_pass_graph_scaffold;

  surface.scaffold_ready = scaffold.pass_graph_ready;
  surface.lowering_boundary_replay_key = scaffold.lowering_boundary_replay_key;
  surface.runtime_dispatch_declaration_replay_key =
      scaffold.runtime_dispatch_declaration_replay_key;
  surface.direct_ir_entrypoint_enabled = scaffold.ir_emission_entrypoint_ready;
}

}  // namespace objc3_lowering_pipeline_pass_graph_core_feature
