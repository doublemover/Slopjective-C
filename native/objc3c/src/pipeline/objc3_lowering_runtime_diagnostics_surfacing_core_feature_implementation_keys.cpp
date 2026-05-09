#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"

#include <sstream>

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-core-feature-impl:v1:"
      << "stage_diagnostics_bus_consistent="
      << (surface.stage_diagnostics_bus_consistent ? "true" : "false")
      << ";parse_readiness_surface_ready="
      << (surface.parse_readiness_surface_ready ? "true" : "false")
      << ";diagnostics_surfacing_scaffold_ready="
      << (surface.diagnostics_surfacing_scaffold_ready ? "true" : "false")
      << ";parser_diagnostic_surface_consistent="
      << (surface.parser_diagnostic_surface_consistent ? "true" : "false")
      << ";parser_diagnostic_code_surface_deterministic="
      << (surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                               : "false")
      << ";semantic_diagnostics_deterministic="
      << (surface.semantic_diagnostics_deterministic ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";replay_keys_ready="
      << (surface.replay_keys_ready ? "true" : "false")
      << ";lowering_pipeline_ready="
      << (surface.lowering_pipeline_ready ? "true" : "false")
      << ";lexer_diagnostic_count=" << surface.lexer_diagnostic_count
      << ";parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";semantic_diagnostic_count=" << surface.semantic_diagnostic_count
      << ";parse_artifact_replay_key=" << surface.parse_artifact_replay_key
      << ";lowering_boundary_replay_key=" << surface.lowering_boundary_replay_key
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}
