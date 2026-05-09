#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"

#include <sstream>

std::string BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-diagnostics-hardening:v1:"
      << "edge_case_robustness_consistent="
      << (surface.edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";parse_diagnostics_hardening_consistent="
      << (surface.parse_diagnostics_hardening_consistent ? "true" : "false")
      << ";parse_diagnostics_hardening_ready="
      << (surface.parse_diagnostics_hardening_ready ? "true" : "false")
      << ";semantic_diagnostics_hardening_consistent="
      << (surface.semantic_diagnostics_hardening_consistent ? "true" : "false")
      << ";semantic_diagnostics_hardening_ready="
      << (surface.semantic_diagnostics_hardening_ready ? "true" : "false")
      << ";lowering_pipeline_diagnostics_hardening_ready="
      << (surface.lowering_pipeline_diagnostics_hardening_ready ? "true"
                                                                 : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diag-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";edge-case-robustness-key-ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";parse-artifact-diag-hardening-key-ready="
      << (!surface.parse_artifact_diagnostics_hardening_key.empty() ? "true"
                                                                     : "false")
      << ";long-tail-diag-hardening-key-ready="
      << (!surface.long_tail_grammar_diagnostics_hardening_key.empty() ? "true"
                                                                        : "false")
      << ";parser-hooks-diag-hardening-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty()
              ? "true"
              : "false")
      << ";semantic-diag-hardening-key-ready="
      << (!surface.semantic_diagnostics_hardening_key.empty() ? "true"
                                                               : "false")
      << ";lowering-pipeline-diag-hardening-key-ready="
      << (!surface.lowering_pipeline_diagnostics_hardening_key.empty() ? "true"
                                                                        : "false")
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";parse_artifact_diagnostics_hardening_key="
      << surface.parse_artifact_diagnostics_hardening_key
      << ";long_tail_grammar_diagnostics_hardening_key="
      << surface.long_tail_grammar_diagnostics_hardening_key
      << ";parser_diagnostic_grammar_hooks_diagnostics_hardening_key="
      << surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key
      << ";semantic_diagnostics_hardening_key="
      << surface.semantic_diagnostics_hardening_key
      << ";lowering_pipeline_diagnostics_hardening_key="
      << surface.lowering_pipeline_diagnostics_hardening_key;
  return key.str();
}
