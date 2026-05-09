#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold_owners.h"

namespace objc3_lowering_pipeline_pass_graph_scaffold {

void PublishFailureReason(Objc3LoweringPipelinePassGraphScaffold &scaffold) {
  if (scaffold.pass_graph_ready) {
    return;
  }

  if (scaffold.failure_reason.empty()) {
    if (!scaffold.lex_stage_ready) {
      scaffold.failure_reason = "lex stage is not ready";
    } else if (!scaffold.parse_stage_ready) {
      scaffold.failure_reason = "parse stage is not ready";
    } else if (!scaffold.sema_stage_ready) {
      scaffold.failure_reason = "sema stage is not ready";
    } else if (!scaffold.typed_surface_ready) {
      scaffold.failure_reason = "typed sema-to-lowering surface is not ready";
    } else if (!scaffold.parse_lowering_readiness_ready) {
      scaffold.failure_reason = "parse-to-lowering readiness surface is not ready";
    } else if (!scaffold.semantic_stability_scaffold_ready) {
      scaffold.failure_reason = "semantic stability scaffold is not ready";
    } else if (!scaffold.lowering_runtime_invariant_scaffold_ready) {
      scaffold.failure_reason = "lowering runtime invariant scaffold is not ready";
    } else if (!scaffold.lowering_ir_boundary_ready) {
      scaffold.failure_reason = "lowering IR boundary is not ready";
    } else if (!scaffold.runtime_dispatch_declaration_ready) {
      scaffold.failure_reason =
          "runtime dispatch declaration replay key is not ready";
    } else if (!scaffold.ir_emission_entrypoint_ready) {
      scaffold.failure_reason = "IR emission entrypoint is not ready";
    } else {
      scaffold.failure_reason =
          "lowering pipeline pass graph scaffold is not ready";
    }
  }
}

}  // namespace objc3_lowering_pipeline_pass_graph_scaffold
