#include "lower/contracts/lowering_diagnostics.h"

#include <sstream>

const char *Objc3LoweringDiagnosticSeverityName(
    Objc3LoweringDiagnosticSeverity severity) {
  switch (severity) {
    case Objc3LoweringDiagnosticSeverity::Note:
      return "note";
    case Objc3LoweringDiagnosticSeverity::Warning:
      return "warning";
    case Objc3LoweringDiagnosticSeverity::Error:
    default:
      return "error";
  }
}

bool Objc3LoweringDiagnosticIsBlocking(
    const Objc3LoweringDiagnostic &diagnostic) {
  return diagnostic.severity == Objc3LoweringDiagnosticSeverity::Error;
}

std::string Objc3LoweringDiagnosticReplayKey(
    const Objc3LoweringDiagnostic &diagnostic) {
  std::ostringstream out;
  out << "code=" << diagnostic.code
      << ";severity=" << Objc3LoweringDiagnosticSeverityName(diagnostic.severity)
      << ";line=" << diagnostic.line << ";column=" << diagnostic.column
      << ";message=" << diagnostic.message << ";"
      << Objc3LoweringOwnerReplayKey(
             diagnostic.diagnostic_handoff_owner,
             diagnostic.diagnostic_owner_model,
             diagnostic.strict_no_fallback,
             diagnostic.strict_no_compatibility);
  return out.str();
}
