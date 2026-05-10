#include "parse/objc3_diagnostic_grammar_hooks_edge_case_consistency_surface.h"

#include <sstream>

#include "parse/objc3_diagnostic_grammar_hooks_edge_case_consistency_building.inc"

bool IsObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurfaceReady(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface) {
  return surface.edge_case_consistency_ready && !surface.consistency_key.empty();
}
