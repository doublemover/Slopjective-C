#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"

#include <sstream>

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

std::string BuildObjc3LoweringPipelinePassGraphDiagnosticsHardeningKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-diagnostics-hardening:v1:"
      << "edge-case-robustness-ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";diagnostics-hardening-consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics-hardening-ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key
      << ";edge-case-robustness-key-ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false");
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphRecoveryDeterminismKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-recovery-determinism:v1:"
      << "diagnostics-hardening-ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key
      << ";diagnostics-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false");
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphConformanceMatrixKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-conformance-matrix:v1:"
      << "recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";lowering-boundary-replay-key=" << surface.lowering_boundary_replay_key
      << ";runtime-dispatch-declaration-replay-key="
      << surface.runtime_dispatch_declaration_replay_key
      << ";recovery-determinism-key-ready="
      << (!surface.recovery_determinism_key.empty() ? "true" : "false");
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphConformanceCorpusKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-conformance-corpus:v1:"
      << "conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance-corpus-consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance-matrix-key-ready="
      << (!surface.conformance_matrix_key.empty() ? "true" : "false");
  return key.str();
}

std::string BuildObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsKey(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "lowering-pipeline-pass-graph-performance-quality-guardrails:v1:"
      << "conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";performance-quality-guardrails-consistent="
      << (surface.performance_quality_guardrails_consistent ? "true" : "false")
      << ";performance-quality-guardrails-ready="
      << (surface.performance_quality_guardrails_ready ? "true" : "false")
      << ";performance-quality-guardrails-case-count="
      << surface.parse_lowering_performance_quality_guardrails_case_count
      << ";performance-quality-guardrails-passed-case-count="
      << surface.parse_lowering_performance_quality_guardrails_passed_case_count
      << ";performance-quality-guardrails-failed-case-count="
      << surface.parse_lowering_performance_quality_guardrails_failed_case_count
      << ";conformance-corpus-key-ready="
      << (!surface.conformance_corpus_key.empty() ? "true" : "false");
  return key.str();
}
