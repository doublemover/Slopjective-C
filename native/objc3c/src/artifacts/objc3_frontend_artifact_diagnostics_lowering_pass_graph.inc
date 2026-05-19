namespace {

void RecordObjc3LoweringPassGraphReadinessFailures(
    const Objc3FrontendPipelineResult &pipeline_result,
    PostPipelineFailureRecorder &recorder) {
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
}

}  // namespace
