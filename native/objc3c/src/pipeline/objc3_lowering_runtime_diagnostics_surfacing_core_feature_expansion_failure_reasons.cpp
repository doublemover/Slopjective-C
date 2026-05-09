#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface_owners.h"

namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion {

void PublishFailureReason(
    Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface &surface) {
  if (surface.core_feature_expansion_ready) {
    return;
  }

  if (!surface.core_feature_impl_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature implementation is not ready";
  } else if (!surface.diagnostics_surfacing_scaffold_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing scaffold is not ready";
  } else if (!surface.diagnostics_hardening_key_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening key continuity is inconsistent";
  } else if (!surface.diagnostics_payload_accounting_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics payload accounting is inconsistent";
  } else if (!surface.expansion_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics expansion replay keys are not ready";
  } else if (!surface.lowering_pipeline_expansion_ready) {
    surface.failure_reason =
        "lowering pipeline expansion prerequisites are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature expansion is not ready";
  }
}

}  // namespace objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion
