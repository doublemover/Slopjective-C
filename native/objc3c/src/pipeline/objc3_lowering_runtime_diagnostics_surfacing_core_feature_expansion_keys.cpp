#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"

#include <sstream>

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-core-feature-expansion:v1:"
      << "core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";diagnostics_surfacing_scaffold_ready="
      << (surface.diagnostics_surfacing_scaffold_ready ? "true" : "false")
      << ";diagnostics_hardening_key_consistent="
      << (surface.diagnostics_hardening_key_consistent ? "true" : "false")
      << ";diagnostics_payload_accounting_consistent="
      << (surface.diagnostics_payload_accounting_consistent ? "true" : "false")
      << ";expansion_replay_keys_ready="
      << (surface.expansion_replay_keys_ready ? "true" : "false")
      << ";lowering_pipeline_expansion_ready="
      << (surface.lowering_pipeline_expansion_ready ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";lexer_diagnostic_count=" << surface.lexer_diagnostic_count
      << ";parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";semantic_diagnostic_count=" << surface.semantic_diagnostic_count
      << ";parser_diagnostic_code_count=" << surface.parser_diagnostic_code_count
      << ";parse_artifact_replay_key=" << surface.parse_artifact_replay_key
      << ";lowering_boundary_replay_key=" << surface.lowering_boundary_replay_key
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";core_feature_key=" << surface.core_feature_key;
  return key.str();
}
