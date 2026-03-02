#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"

#include <sstream>
#include <string>

std::string BuildObjc3LoweringPipelinePassGraphCoreFeatureKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-core-feature:v1:"
      << "scaffold-ready=" << (surface.scaffold_ready ? "true" : "false")
      << ";lowering-boundary-replay-key-consistent="
      << (surface.lowering_boundary_replay_key_consistent ? "true" : "false")
      << ";runtime-dispatch-declaration-consistent="
      << (surface.runtime_dispatch_declaration_consistent ? "true" : "false")
      << ";direct-ir-entrypoint-enabled="
      << (surface.direct_ir_entrypoint_enabled ? "true" : "false")
      << ";dispatch-shape-sharding-ready="
      << (surface.dispatch_shape_sharding_ready ? "true" : "false")
      << ";llc-object-emission-route-deterministic="
      << (surface.llc_object_emission_route_deterministic ? "true" : "false")
      << ";replay-proof-artifact-key-ready="
      << (surface.replay_proof_artifact_key_ready ? "true" : "false")
      << ";core-feature-ready=" << (surface.core_feature_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key;
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphCoreFeatureExpansionKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-core-feature-expansion:v1:"
      << "core-feature-ready=" << (surface.core_feature_ready ? "true" : "false")
      << ";edge-case-dispatch-shape-coverage-ready="
      << (surface.edge_case_dispatch_shape_coverage_ready ? "true" : "false")
      << ";replay-proof-expansion-ready="
      << (surface.replay_proof_expansion_ready ? "true" : "false")
      << ";expansion-ready=" << (surface.expansion_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key;
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-edge-case-compatibility:v1:"
      << "expansion-ready=" << (surface.expansion_ready ? "true" : "false")
      << ";compatibility-handoff-consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language-version-pragma-coordinate-order-consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                      : "false")
      << ";edge-case-compatibility-ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key;
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphEdgeCaseRobustnessKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-edge-case-robustness:v1:"
      << "edge-case-compatibility-ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";edge-case-expansion-consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge-case-robustness-ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key
      << ";edge-case-compatibility-key-ready="
      << (!surface.edge_case_compatibility_key.empty() ? "true" : "false");
  return key.str();
}

Objc3LoweringPipelinePassGraphCoreFeatureSurface
BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3LoweringPipelinePassGraphCoreFeatureSurface surface;
  const Objc3LoweringPipelinePassGraphScaffold &scaffold =
      pipeline_result.lowering_pipeline_pass_graph_scaffold;
  surface.scaffold_ready = scaffold.pass_graph_ready;
  surface.lowering_boundary_replay_key = scaffold.lowering_boundary_replay_key;
  surface.runtime_dispatch_declaration_replay_key =
      scaffold.runtime_dispatch_declaration_replay_key;
  surface.lowering_boundary_replay_key_consistent =
      !surface.lowering_boundary_replay_key.empty() &&
      !pipeline_result.typed_sema_to_lowering_contract_surface
           .lowering_boundary_replay_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface.lowering_boundary_replay_key
           .empty() &&
      surface.lowering_boundary_replay_key ==
          pipeline_result.typed_sema_to_lowering_contract_surface
              .lowering_boundary_replay_key &&
      surface.lowering_boundary_replay_key ==
          pipeline_result.parse_lowering_readiness_surface
              .lowering_boundary_replay_key;
  surface.runtime_dispatch_declaration_consistent =
      !surface.runtime_dispatch_declaration_replay_key.empty() &&
      surface.runtime_dispatch_declaration_replay_key.find("declare i32 @") == 0 &&
      surface.runtime_dispatch_declaration_replay_key.find(
          options.lowering.runtime_dispatch_symbol) != std::string::npos;
  surface.direct_ir_entrypoint_enabled = scaffold.ir_emission_entrypoint_ready;
  surface.dispatch_shape_sharding_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface
          .runtime_dispatch_contract_consistent &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_consistent;
  surface.llc_object_emission_route_deterministic =
      options.lowering.max_message_send_args <= kObjc3RuntimeDispatchMaxArgs &&
      IsValidRuntimeDispatchSymbol(options.lowering.runtime_dispatch_symbol);
  surface.replay_proof_artifact_key_ready =
      pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.runtime_dispatch_declaration_replay_key.empty();
  surface.edge_case_dispatch_shape_coverage_ready =
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_expansion_ready &&
      options.lowering.max_message_send_args >= kObjc3RuntimeDispatchDefaultArgs &&
      options.lowering.max_message_send_args <= kObjc3RuntimeDispatchMaxArgs;
  surface.replay_proof_expansion_ready =
      surface.replay_proof_artifact_key_ready &&
      !pipeline_result.typed_sema_to_lowering_contract_surface
           .typed_core_feature_expansion_key.empty() &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_expansion_key.empty();
  surface.compatibility_handoff_consistent =
      pipeline_result.parse_lowering_readiness_surface
          .compatibility_handoff_consistent &&
      !pipeline_result.parse_lowering_readiness_surface.compatibility_handoff_key
           .empty();
  surface.language_version_pragma_coordinate_order_consistent =
      pipeline_result.parse_lowering_readiness_surface
          .language_version_pragma_coordinate_order_consistent;
  surface.core_feature_ready =
      surface.scaffold_ready &&
      surface.lowering_boundary_replay_key_consistent &&
      surface.runtime_dispatch_declaration_consistent &&
      surface.direct_ir_entrypoint_enabled &&
      surface.dispatch_shape_sharding_ready &&
      surface.llc_object_emission_route_deterministic &&
      surface.replay_proof_artifact_key_ready;
  surface.expansion_ready = surface.core_feature_ready &&
                            surface.edge_case_dispatch_shape_coverage_ready &&
                            surface.replay_proof_expansion_ready;
  surface.edge_case_compatibility_ready =
      surface.expansion_ready &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_compatibility_ready &&
      pipeline_result.typed_sema_to_lowering_contract_surface
          .typed_core_feature_expansion_consistent;
  surface.edge_case_compatibility_key =
      BuildObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityKey(surface);
  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_ready &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .parse_artifact_edge_case_robustness_consistent;
  surface.edge_case_robustness_ready =
      surface.edge_case_expansion_consistent &&
      pipeline_result.parse_lowering_readiness_surface
          .long_tail_grammar_edge_case_robustness_ready &&
      !pipeline_result.parse_lowering_readiness_surface
           .long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.edge_case_robustness_key =
      BuildObjc3LoweringPipelinePassGraphEdgeCaseRobustnessKey(surface);
  surface.expansion_key =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureExpansionKey(surface);
  surface.core_feature_key =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureKey(surface);

  if (surface.core_feature_ready && surface.expansion_ready &&
      surface.edge_case_compatibility_ready &&
      surface.edge_case_robustness_ready) {
    return surface;
  }

  if (!surface.scaffold_ready) {
    surface.failure_reason = "pass-graph scaffold is not ready";
  } else if (!surface.lowering_boundary_replay_key_consistent) {
    surface.failure_reason = "lowering boundary replay key is inconsistent";
  } else if (!surface.runtime_dispatch_declaration_consistent) {
    surface.failure_reason = "runtime dispatch declaration is inconsistent";
  } else if (!surface.direct_ir_entrypoint_enabled) {
    surface.failure_reason = "direct IR emission entrypoint is disabled";
  } else if (!surface.dispatch_shape_sharding_ready) {
    surface.failure_reason = "dispatch-shape sharding contract is not ready";
  } else if (!surface.llc_object_emission_route_deterministic) {
    surface.failure_reason = "llc object-emission route is not deterministic";
  } else if (!surface.replay_proof_artifact_key_ready) {
    surface.failure_reason = "replay-proof artifact keys are not ready";
  } else if (!surface.edge_case_dispatch_shape_coverage_ready) {
    surface.failure_reason = "edge-case dispatch-shape coverage is not ready";
  } else if (!surface.replay_proof_expansion_ready) {
    surface.failure_reason = "replay-proof expansion anchors are not ready";
  } else if (!surface.expansion_ready) {
    surface.failure_reason = "pass-graph core-feature expansion is not ready";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason =
        "language version pragma coordinate order is inconsistent";
  } else if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason = "pass-graph edge-case compatibility is not ready";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason = "pass-graph edge-case expansion is inconsistent";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason = "pass-graph edge-case robustness is not ready";
  } else if (surface.edge_case_robustness_key.empty()) {
    surface.failure_reason = "pass-graph edge-case robustness key is not ready";
  } else {
    surface.failure_reason =
        "lowering pipeline pass-graph core feature surface is not ready";
  }
  return surface;
}

bool IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.core_feature_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph core feature is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphCoreFeatureExpansionReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.expansion_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph core feature expansion is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.edge_case_compatibility_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph edge-case compatibility is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphEdgeCaseRobustnessReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.edge_case_robustness_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph edge-case robustness is not ready"
               : surface.failure_reason;
  return false;
}
