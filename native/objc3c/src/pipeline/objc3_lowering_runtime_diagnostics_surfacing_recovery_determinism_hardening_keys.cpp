#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"

#include <sstream>

std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening:v1:"
      << "diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";parse_recovery_determinism_consistent="
      << (surface.parse_recovery_determinism_consistent ? "true" : "false")
      << ";parse_recovery_determinism_ready="
      << (surface.parse_recovery_determinism_ready ? "true" : "false")
      << ";semantic_recovery_determinism_consistent="
      << (surface.semantic_recovery_determinism_consistent ? "true" : "false")
      << ";semantic_recovery_determinism_ready="
      << (surface.semantic_recovery_determinism_ready ? "true" : "false")
      << ";lowering_pipeline_recovery_determinism_ready="
      << (surface.lowering_pipeline_recovery_determinism_ready ? "true"
                                                                : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";diag-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";parse-recovery-key-ready="
      << (!surface.parse_recovery_determinism_hardening_key.empty() ? "true"
                                                                     : "false")
      << ";long-tail-recovery-key-ready="
      << (!surface.long_tail_grammar_recovery_determinism_key.empty() ? "true"
                                                                       : "false")
      << ";parser-hooks-recovery-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_recovery_determinism_key.empty()
              ? "true"
              : "false")
      << ";semantic-recovery-key-ready="
      << (!surface.semantic_recovery_determinism_key.empty() ? "true"
                                                              : "false")
      << ";lowering-pipeline-recovery-key-ready="
      << (!surface.lowering_pipeline_recovery_determinism_key.empty() ? "true"
                                                                       : "false")
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";long_tail_grammar_recovery_determinism_key="
      << surface.long_tail_grammar_recovery_determinism_key
      << ";parser_diagnostic_grammar_hooks_recovery_determinism_key="
      << surface.parser_diagnostic_grammar_hooks_recovery_determinism_key
      << ";semantic_recovery_determinism_key="
      << surface.semantic_recovery_determinism_key
      << ";lowering_pipeline_recovery_determinism_key="
      << surface.lowering_pipeline_recovery_determinism_key;
  return key.str();
}
