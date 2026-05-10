#include "artifacts/objc3_frontend_artifact_diagnostics.h"

#include <string>
#include <utility>

#include "artifacts/objc3_frontend_artifacts.h"
#include "diag/objc3_diag_utils.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"

namespace objc3::artifacts::frontend {
namespace {

class PostPipelineFailureRecorder {
 public:
  void Record(const char *code, std::string message) {
    RecordObjc3FrontendArtifactPostPipelineFailure(failure_, code,
                                                   std::move(message));
  }

  [[nodiscard]] const Objc3FrontendArtifactPostPipelineFailure &failure() const {
    return failure_;
  }

 private:
  Objc3FrontendArtifactPostPipelineFailure failure_;
};

}  // namespace

Objc3FrontendArtifactPostPipelineFailure
BuildObjc3FrontendArtifactInitialPostPipelineFailure(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface,
    bool metadata_only_ir_emission_mode) {
  PostPipelineFailureRecorder recorder;
  if (metadata_only_ir_emission_mode) {
    return recorder.failure();
  }

  std::string parse_lowering_readiness_error;
  if (!IsObjc3ParseLoweringReadinessSurfaceReady(
          parse_lowering_readiness_surface, parse_lowering_readiness_error)) {
    recorder.Record(
        "O3L300",
        "LLVM IR emission failed: parse-to-lowering readiness check failed: " +
            parse_lowering_readiness_error);
  }

  std::string diagnostics_surfacing_scaffold_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(
          pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold,
          diagnostics_surfacing_scaffold_error)) {
    recorder.Record(
        "O3L301",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing scaffold check failed: " +
            diagnostics_surfacing_scaffold_error);
  }

  std::string diagnostics_surfacing_core_feature_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface,
          diagnostics_surfacing_core_feature_error)) {
    recorder.Record(
        "O3L321",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature check failed: " +
            diagnostics_surfacing_core_feature_error);
  }

  std::string diagnostics_surfacing_core_feature_expansion_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface,
          diagnostics_surfacing_core_feature_expansion_error)) {
    recorder.Record(
        "O3L322",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature expansion check failed: " +
            diagnostics_surfacing_core_feature_expansion_error);
  }

  std::string diagnostics_surfacing_edge_case_compatibility_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface,
          diagnostics_surfacing_edge_case_compatibility_error)) {
    recorder.Record(
        "O3L323",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case compatibility check failed: " +
            diagnostics_surfacing_edge_case_compatibility_error);
  }

  std::string diagnostics_surfacing_edge_case_robustness_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface,
          diagnostics_surfacing_edge_case_robustness_error)) {
    recorder.Record(
        "O3L324",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case expansion and robustness check failed: " +
            diagnostics_surfacing_edge_case_robustness_error);
  }

  std::string diagnostics_surfacing_diagnostics_hardening_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface,
          diagnostics_surfacing_diagnostics_hardening_error)) {
    recorder.Record(
        "O3L325",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing diagnostics hardening check failed: " +
            diagnostics_surfacing_diagnostics_hardening_error);
  }

  std::string diagnostics_surfacing_recovery_determinism_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface,
          diagnostics_surfacing_recovery_determinism_error)) {
    recorder.Record(
        "O3L326",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing recovery/determinism hardening check failed: " +
            diagnostics_surfacing_recovery_determinism_error);
  }

  std::string diagnostics_surfacing_conformance_matrix_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface,
          diagnostics_surfacing_conformance_matrix_error)) {
    recorder.Record(
        "O3L327",
        "LLVM IR emission failed: lowering/runtime diagnostics surfacing conformance matrix check failed: " +
            diagnostics_surfacing_conformance_matrix_error);
  }

  std::string lowering_pass_graph_error;
  if (!IsObjc3LoweringPipelinePassGraphScaffoldReady(
          pipeline_result.lowering_pipeline_pass_graph_scaffold,
          lowering_pass_graph_error)) {
    recorder.Record(
        "O3L301",
        "LLVM IR emission failed: lowering pipeline pass-graph scaffold check failed: " +
            lowering_pass_graph_error);
  }

  std::string lowering_pass_graph_core_feature_error;
  if (!IsObjc3IREmissionCompletenessCoreFeatureReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_core_feature_error)) {
    recorder.Record(
        "O3L302",
        "LLVM IR emission failed: lowering pipeline pass-graph core feature check failed: " +
            lowering_pass_graph_core_feature_error);
  }

  std::string lowering_pass_graph_expansion_error;
  if (!IsObjc3IREmissionCompletenessExpansionReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_expansion_error)) {
    recorder.Record(
        "O3L303",
        "LLVM IR emission failed: lowering pipeline pass-graph core feature expansion check failed: " +
            lowering_pass_graph_expansion_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_compatibility_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_compatibility_error)) {
    recorder.Record(
        "O3L305",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case compatibility check failed: " +
            lowering_pass_graph_lane_a_edge_case_compatibility_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_robustness_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseRobustnessReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_robustness_error)) {
    recorder.Record(
        "O3L307",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case robustness check failed: " +
            lowering_pass_graph_lane_a_edge_case_robustness_error);
  }

  std::string lowering_pass_graph_lane_a_diagnostics_hardening_error;
  if (!IsObjc3LoweringPipelinePassGraphDiagnosticsHardeningReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_diagnostics_hardening_error)) {
    recorder.Record(
        "O3L308",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A diagnostics hardening check failed: " +
            lowering_pass_graph_lane_a_diagnostics_hardening_error);
  }

  std::string lowering_pass_graph_lane_a_recovery_determinism_error;
  if (!IsObjc3LoweringPipelinePassGraphRecoveryDeterminismReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_recovery_determinism_error)) {
    recorder.Record(
        "O3L309",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A recovery determinism check failed: " +
            lowering_pass_graph_lane_a_recovery_determinism_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_matrix_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceMatrixReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_matrix_error)) {
    recorder.Record(
        "O3L311",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance matrix check failed: " +
            lowering_pass_graph_lane_a_conformance_matrix_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_corpus_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceCorpusReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_corpus_error)) {
    recorder.Record(
        "O3L313",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance corpus check failed: " +
            lowering_pass_graph_lane_a_conformance_corpus_error);
  }

  std::string lowering_pass_graph_lane_a_performance_quality_guardrails_error;
  if (!IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_performance_quality_guardrails_error)) {
    recorder.Record(
        "O3L315",
        "LLVM IR emission failed: lowering pipeline pass-graph lane-A performance quality guardrails check failed: " +
            lowering_pass_graph_lane_a_performance_quality_guardrails_error);
  }

  std::string lowering_pass_graph_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_edge_case_compatibility_error)) {
    recorder.Record(
        "O3L304",
        "LLVM IR emission failed: lowering pipeline pass-graph edge-case compatibility check failed: " +
            lowering_pass_graph_edge_case_compatibility_error);
  }

  std::string ir_emission_core_feature_impl_error;
  if (!IsObjc3IREmissionCoreFeatureImplementationReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_impl_error)) {
    recorder.Record(
        "O3L306",
        "LLVM IR emission failed: IR emission core feature implementation check failed: " +
            ir_emission_core_feature_impl_error);
  }

  std::string ir_emission_core_feature_expansion_error;
  if (!IsObjc3IREmissionCoreFeatureExpansionReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_expansion_error)) {
    recorder.Record(
        "O3L314",
        "LLVM IR emission failed: IR emission core feature expansion check failed: " +
            ir_emission_core_feature_expansion_error);
  }

  std::string ir_emission_core_feature_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_compatibility_error)) {
    recorder.Record(
        "O3L316",
        "LLVM IR emission failed: IR emission core feature edge-case compatibility check failed: " +
            ir_emission_core_feature_edge_case_compatibility_error);
  }

  std::string ir_emission_core_feature_edge_case_robustness_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_robustness_error)) {
    recorder.Record(
        "O3L317",
        "LLVM IR emission failed: IR emission core feature edge-case robustness check failed: " +
            ir_emission_core_feature_edge_case_robustness_error);
  }

  std::string ir_emission_core_feature_diagnostics_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_diagnostics_hardening_error)) {
    recorder.Record(
        "O3L318",
        "LLVM IR emission failed: IR emission core feature diagnostics hardening check failed: " +
            ir_emission_core_feature_diagnostics_hardening_error);
  }

  std::string ir_emission_core_feature_recovery_determinism_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_recovery_determinism_hardening_error)) {
    recorder.Record(
        "O3L319",
        "LLVM IR emission failed: IR emission core feature recovery determinism hardening check failed: " +
            ir_emission_core_feature_recovery_determinism_hardening_error);
  }

  std::string ir_emission_core_feature_conformance_matrix_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_matrix_error)) {
    recorder.Record(
        "O3L320",
        "LLVM IR emission failed: IR emission core feature conformance matrix check failed: " +
            ir_emission_core_feature_conformance_matrix_error);
  }

  std::string ir_emission_core_feature_conformance_corpus_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_corpus_error)) {
    recorder.Record(
        "O3L330",
        "LLVM IR emission failed: IR emission core feature conformance corpus check failed: " +
            ir_emission_core_feature_conformance_corpus_error);
  }

  std::string ir_emission_core_feature_performance_quality_guardrails_error;
  if (!IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_performance_quality_guardrails_error)) {
    recorder.Record(
        "O3L331",
        "LLVM IR emission failed: IR emission core feature performance quality guardrails check failed: " +
            ir_emission_core_feature_performance_quality_guardrails_error);
  }

  std::string ir_emission_core_feature_cross_lane_integration_sync_error;
  if (!IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_cross_lane_integration_sync_error)) {
    recorder.Record(
        "O3L332",
        "LLVM IR emission failed: IR emission core feature cross-lane integration sync check failed: " +
            ir_emission_core_feature_cross_lane_integration_sync_error);
  }

  std::string ir_emission_core_feature_advanced_core_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_core_shard1_error)) {
    recorder.Record(
        "O3L333",
        "LLVM IR emission failed: IR emission core feature advanced core shard 1 check failed: " +
            ir_emission_core_feature_advanced_core_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_edge_compatibility_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_edge_compatibility_shard1_error)) {
    recorder.Record(
        "O3L334",
        "LLVM IR emission failed: IR emission core feature advanced edge compatibility shard 1 check failed: " +
            ir_emission_core_feature_advanced_edge_compatibility_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_diagnostics_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_diagnostics_shard1_error)) {
    recorder.Record(
        "O3L335",
        "LLVM IR emission failed: IR emission core feature advanced diagnostics shard 1 check failed: " +
            ir_emission_core_feature_advanced_diagnostics_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_conformance_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_conformance_shard1_error)) {
    recorder.Record(
        "O3L336",
        "LLVM IR emission failed: IR emission core feature advanced conformance shard 1 check failed: " +
            ir_emission_core_feature_advanced_conformance_shard1_error);
  }

  std::string ir_emission_core_feature_advanced_integration_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_integration_shard1_error)) {
    recorder.Record(
        "O3L337",
        "LLVM IR emission failed: IR emission core feature advanced integration shard 1 check failed: " +
            ir_emission_core_feature_advanced_integration_shard1_error);
  }

  return recorder.failure();
}

void RecordObjc3FrontendArtifactPostPipelineFailure(
    Objc3FrontendArtifactPostPipelineFailure &failure,
    const char *code,
    std::string message) {
  if (failure.present) {
    return;
  }
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
}

bool FinalizeObjc3FrontendPostPipelineFailure(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactPostPipelineFailure &failure) {
  if (failure.empty()) {
    return false;
  }

  if (!options.emit_ir && !options.emit_object) {
    return true;
  }

  bundle.post_pipeline_diagnostics = {
      MakeDiag(1, 1, failure.code, failure.message)};
  bundle.diagnostics = bundle.post_pipeline_diagnostics;
  return true;
}

}  // namespace objc3::artifacts::frontend
