namespace {

void RecordObjc3ParseLoweringReadinessFailure(
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface,
    PostPipelineFailureRecorder &recorder) {
  std::string parse_lowering_readiness_error;
  if (!IsObjc3ParseLoweringReadinessSurfaceReady(
          parse_lowering_readiness_surface, parse_lowering_readiness_error)) {
    recorder.Record(
        "O3L300",
        "LLVM IR emission failed: parse-to-lowering readiness check failed: " +
            parse_lowering_readiness_error);
  }
}

void RecordObjc3RuntimeDiagnosticsSurfacingReadinessFailures(
    const Objc3FrontendPipelineResult &pipeline_result,
    PostPipelineFailureRecorder &recorder) {
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
}

}  // namespace
