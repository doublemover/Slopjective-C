#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface_owners.h"

namespace objc3_lowering_pipeline_pass_graph_core_feature {

void PublishFailureReason(
    Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  if (surface.core_feature_ready && surface.expansion_ready &&
      surface.edge_case_compatibility_ready &&
      surface.edge_case_robustness_ready &&
      surface.diagnostics_hardening_ready &&
      surface.recovery_determinism_ready &&
      surface.conformance_matrix_ready &&
      surface.conformance_corpus_ready &&
      surface.performance_quality_guardrails_ready) {
    return;
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
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason = "pass-graph diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason = "pass-graph diagnostics hardening is not ready";
  } else if (surface.diagnostics_hardening_key.empty()) {
    surface.failure_reason = "pass-graph diagnostics hardening key is not ready";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason = "pass-graph recovery determinism is inconsistent";
  } else if (!surface.recovery_determinism_ready) {
    surface.failure_reason = "pass-graph recovery determinism is not ready";
  } else if (surface.recovery_determinism_key.empty()) {
    surface.failure_reason = "pass-graph recovery determinism key is not ready";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason = "pass-graph conformance matrix is inconsistent";
  } else if (!surface.conformance_matrix_ready) {
    surface.failure_reason = "pass-graph conformance matrix is not ready";
  } else if (surface.conformance_matrix_key.empty()) {
    surface.failure_reason = "pass-graph conformance matrix key is not ready";
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason = "pass-graph conformance corpus is inconsistent";
  } else if (!surface.conformance_corpus_ready) {
    surface.failure_reason = "pass-graph conformance corpus is not ready";
  } else if (surface.conformance_corpus_key.empty()) {
    surface.failure_reason = "pass-graph conformance corpus key is not ready";
  } else if (!surface.performance_quality_guardrails_consistent) {
    surface.failure_reason =
        "pass-graph performance quality guardrails are inconsistent";
  } else if (!surface.performance_quality_guardrails_ready) {
    surface.failure_reason =
        "pass-graph performance quality guardrails are not ready";
  } else if (surface.performance_quality_guardrails_key.empty()) {
    surface.failure_reason =
        "pass-graph performance quality guardrails key is not ready";
  } else {
    surface.failure_reason =
        "lowering pipeline pass-graph core feature surface is not ready";
  }
}

}  // namespace objc3_lowering_pipeline_pass_graph_core_feature
