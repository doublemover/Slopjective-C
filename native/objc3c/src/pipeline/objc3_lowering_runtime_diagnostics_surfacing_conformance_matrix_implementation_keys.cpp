#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"

#include <sstream>

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation:v1:"
      << "recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";parse-conformance-matrix-consistent="
      << (surface.parse_conformance_matrix_consistent ? "true" : "false")
      << ";parse-conformance-matrix-ready="
      << (surface.parse_conformance_matrix_ready ? "true" : "false")
      << ";semantic-conformance-matrix-consistent="
      << (surface.semantic_conformance_matrix_consistent ? "true" : "false")
      << ";semantic-conformance-matrix-ready="
      << (surface.semantic_conformance_matrix_ready ? "true" : "false")
      << ";lowering-pipeline-conformance-matrix-ready="
      << (surface.lowering_pipeline_conformance_matrix_ready ? "true" : "false")
      << ";conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance-matrix-key-ready="
      << (!surface.conformance_matrix_key.empty() ? "true" : "false")
      << ";parse-conformance-matrix-case-count="
      << surface.parse_lowering_conformance_matrix_case_count
      << ";recovery-determinism-key-ready="
      << (!surface.recovery_determinism_key.empty() ? "true" : "false")
      << ";parse-conformance-matrix-key-ready="
      << (!surface.parse_lowering_conformance_matrix_key.empty() ? "true"
                                                                  : "false")
      << ";long-tail-conformance-matrix-key-ready="
      << (!surface.long_tail_grammar_conformance_matrix_key.empty() ? "true"
                                                                     : "false")
      << ";parser-hooks-conformance-matrix-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_conformance_matrix_key.empty()
              ? "true"
              : "false")
      << ";semantic-conformance-matrix-key-ready="
      << (!surface.semantic_conformance_matrix_key.empty() ? "true" : "false")
      << ";lowering-pipeline-conformance-matrix-key-ready="
      << (!surface.lowering_pipeline_conformance_matrix_key.empty() ? "true"
                                                                     : "false");
  return key.str();
}
