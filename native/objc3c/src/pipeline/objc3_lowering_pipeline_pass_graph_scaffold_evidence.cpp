#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold_owners.h"

namespace objc3_lowering_pipeline_pass_graph_scaffold {

void PopulateEvidence(Objc3LoweringPipelinePassGraphScaffold &scaffold,
                      const Objc3FrontendPipelineResult &pipeline_result) {
  scaffold.lex_stage_ready = pipeline_result.stage_diagnostics.lexer.empty();
  scaffold.parse_stage_ready =
      scaffold.lex_stage_ready &&
      pipeline_result.stage_diagnostics.parser.empty() &&
      pipeline_result.parser_contract_snapshot.deterministic_handoff &&
      pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready;
  scaffold.sema_stage_ready =
      scaffold.parse_stage_ready &&
      pipeline_result.stage_diagnostics.semantic.empty() &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .semantic_handoff_consistent &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .semantic_handoff_deterministic;
  scaffold.typed_surface_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface.ready_for_lowering;
  scaffold.parse_lowering_readiness_ready =
      pipeline_result.parse_lowering_readiness_surface.ready_for_lowering;
  scaffold.semantic_stability_scaffold_ready =
      pipeline_result.semantic_stability_spec_delta_closure_scaffold
          .modular_split_ready;
  scaffold.lowering_runtime_invariant_scaffold_ready =
      pipeline_result.lowering_runtime_stability_invariant_scaffold
          .modular_split_ready;
}

}  // namespace objc3_lowering_pipeline_pass_graph_scaffold
