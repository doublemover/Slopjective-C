#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"

#include <sstream>
#include <string>

#include "ir/objc3_ir_emitter.h"

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

Objc3LoweringPipelinePassGraphScaffold BuildObjc3LoweringPipelinePassGraphScaffold(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3LoweringPipelinePassGraphScaffold scaffold;
  scaffold.lex_stage_ready = pipeline_result.stage_diagnostics.lexer.empty();
  scaffold.parse_stage_ready =
      scaffold.lex_stage_ready &&
      pipeline_result.stage_diagnostics.parser.empty() &&
      pipeline_result.parser_contract_snapshot.deterministic_handoff &&
      pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready;
  scaffold.sema_stage_ready =
      scaffold.parse_stage_ready &&
      pipeline_result.stage_diagnostics.semantic.empty() &&
      pipeline_result.typed_sema_to_lowering_contract_surface.semantic_handoff_consistent &&
      pipeline_result.typed_sema_to_lowering_contract_surface.semantic_handoff_deterministic;
  scaffold.typed_surface_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface.ready_for_lowering;
  scaffold.parse_lowering_readiness_ready =
      pipeline_result.parse_lowering_readiness_surface.ready_for_lowering;
  scaffold.semantic_stability_scaffold_ready =
      pipeline_result.semantic_stability_spec_delta_closure_scaffold.modular_split_ready;
  scaffold.lowering_runtime_invariant_scaffold_ready =
      pipeline_result.lowering_runtime_stability_invariant_scaffold.modular_split_ready;

  Objc3LoweringIRBoundary boundary;
  std::string boundary_error;
  if (TryBuildObjc3LoweringIRBoundary(options.lowering, boundary, boundary_error)) {
    scaffold.lowering_ir_boundary_ready = true;
    scaffold.lowering_boundary_replay_key = Objc3LoweringIRBoundaryReplayKey(boundary);
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
  scaffold.pass_graph_key = BuildObjc3LoweringPipelinePassGraphScaffoldKey(scaffold);

  if (scaffold.pass_graph_ready) {
    return scaffold;
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
      scaffold.failure_reason = "runtime dispatch declaration replay key is not ready";
    } else if (!scaffold.ir_emission_entrypoint_ready) {
      scaffold.failure_reason = "IR emission entrypoint is not ready";
    } else {
      scaffold.failure_reason = "lowering pipeline pass graph scaffold is not ready";
    }
  }

  return scaffold;
}

bool IsObjc3LoweringPipelinePassGraphScaffoldReady(
    const Objc3LoweringPipelinePassGraphScaffold &scaffold,
    std::string &reason) {
  if (scaffold.pass_graph_ready) {
    reason.clear();
    return true;
  }
  reason = scaffold.failure_reason.empty()
               ? "lowering pipeline pass graph scaffold is not ready"
               : scaffold.failure_reason;
  return false;
}
