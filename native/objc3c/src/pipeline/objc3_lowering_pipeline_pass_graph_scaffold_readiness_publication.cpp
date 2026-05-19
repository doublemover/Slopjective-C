#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold_owners.h"

#include <string>

#include "ir/objc3_ir_emitter.h"

namespace objc3_lowering_pipeline_pass_graph_scaffold {

void PublishReadiness(Objc3LoweringPipelinePassGraphScaffold &scaffold,
                      const Objc3FrontendOptions &options) {
  Objc3LoweringIRBoundary boundary;
  std::string boundary_error;
  if (TryBuildObjc3LoweringIRBoundary(options.lowering, boundary, boundary_error)) {
    scaffold.lowering_ir_boundary_ready = true;
    scaffold.lowering_boundary_replay_key =
        Objc3LoweringIRBoundaryReplayKey(boundary);
    scaffold.runtime_dispatch_declaration_replay_key =
        Objc3RuntimeDispatchDeclarationReplayKey(boundary);
    scaffold.runtime_dispatch_declaration_ready =
        !scaffold.runtime_dispatch_declaration_replay_key.empty();
  } else {
    scaffold.failure_reason =
        "lowering boundary normalization failed: " + boundary_error;
  }

  scaffold.ir_emission_entrypoint_ready = (&EmitObjc3IRText != nullptr);
  scaffold.pass_graph_ready =
      scaffold.lex_stage_ready &&
      scaffold.parse_stage_ready &&
      scaffold.sema_stage_ready &&
      scaffold.typed_surface_ready &&
      scaffold.parse_lowering_readiness_ready &&
      scaffold.semantic_stability_scaffold_ready &&
      scaffold.lowering_runtime_invariant_scaffold_ready &&
      scaffold.lowering_ir_boundary_ready &&
      scaffold.runtime_dispatch_declaration_ready &&
      scaffold.ir_emission_entrypoint_ready &&
      !scaffold.lowering_boundary_replay_key.empty() &&
      !scaffold.runtime_dispatch_declaration_replay_key.empty();
  scaffold.pass_graph_key =
      BuildObjc3LoweringPipelinePassGraphScaffoldKey(scaffold);
}

}  // namespace objc3_lowering_pipeline_pass_graph_scaffold
