#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"

#include <sstream>

std::string BuildObjc3LoweringPipelinePassGraphScaffoldKey(
    const Objc3LoweringPipelinePassGraphScaffold &scaffold) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-scaffold:v1:"
      << "lex-stage-ready=" << (scaffold.lex_stage_ready ? "true" : "false")
      << ";parse-stage-ready=" << (scaffold.parse_stage_ready ? "true" : "false")
      << ";sema-stage-ready=" << (scaffold.sema_stage_ready ? "true" : "false")
      << ";typed-surface-ready=" << (scaffold.typed_surface_ready ? "true" : "false")
      << ";parse-lowering-readiness-ready="
      << (scaffold.parse_lowering_readiness_ready ? "true" : "false")
      << ";semantic-stability-scaffold-ready="
      << (scaffold.semantic_stability_scaffold_ready ? "true" : "false")
      << ";lowering-runtime-invariant-scaffold-ready="
      << (scaffold.lowering_runtime_invariant_scaffold_ready ? "true" : "false")
      << ";lowering-ir-boundary-ready="
      << (scaffold.lowering_ir_boundary_ready ? "true" : "false")
      << ";runtime-dispatch-declaration-ready="
      << (scaffold.runtime_dispatch_declaration_ready ? "true" : "false")
      << ";ir-emission-entrypoint-ready="
      << (scaffold.ir_emission_entrypoint_ready ? "true" : "false")
      << ";pass-graph-ready=" << (scaffold.pass_graph_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << scaffold.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << scaffold.runtime_dispatch_declaration_replay_key;
  return key.str();
}
