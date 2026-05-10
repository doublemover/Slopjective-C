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

#include "artifacts/objc3_frontend_artifact_diagnostics_recording.inc"
#include "artifacts/objc3_frontend_artifact_diagnostics_parse_runtime.inc"
#include "artifacts/objc3_frontend_artifact_diagnostics_lowering_pass_graph.inc"
#include "artifacts/objc3_frontend_artifact_diagnostics_ir_emission.inc"
#include "artifacts/objc3_frontend_artifact_diagnostics_finalization.inc"

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

  RecordObjc3ParseLoweringReadinessFailure(parse_lowering_readiness_surface,
                                           recorder);
  RecordObjc3RuntimeDiagnosticsSurfacingReadinessFailures(pipeline_result,
                                                          recorder);
  RecordObjc3LoweringPassGraphReadinessFailures(pipeline_result, recorder);
  RecordObjc3IREmissionReadinessFailures(ir_emission_core_feature_impl_surface,
                                         recorder);

  return recorder.failure();
}

}  // namespace objc3::artifacts::frontend
