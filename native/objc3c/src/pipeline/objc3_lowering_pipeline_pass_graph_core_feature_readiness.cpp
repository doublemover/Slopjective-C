#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"

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

bool IsObjc3LoweringPipelinePassGraphDiagnosticsHardeningReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.diagnostics_hardening_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph diagnostics hardening is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphRecoveryDeterminismReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.recovery_determinism_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph recovery determinism is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphConformanceMatrixReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.conformance_matrix_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph conformance matrix is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphConformanceCorpusReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.conformance_corpus_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph conformance corpus is not ready"
               : surface.failure_reason;
  return false;
}

bool IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.performance_quality_guardrails_ready) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty()
               ? "lowering pipeline pass-graph performance quality guardrails are not ready"
               : surface.failure_reason;
  return false;
}
