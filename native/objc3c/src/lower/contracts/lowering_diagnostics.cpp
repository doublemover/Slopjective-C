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

Objc3LoweringDiagnostic Objc3MakeLoweringDiagnostic(
    const std::string &code, Objc3LoweringDiagnosticSeverity severity,
    const std::string &message, unsigned line, unsigned column) {
  Objc3LoweringDiagnostic diagnostic;
  diagnostic.code = code;
  diagnostic.severity = severity;
  diagnostic.message = message;
  diagnostic.line = line;
  diagnostic.column = column;
  diagnostic.replay_key = Objc3LoweringDiagnosticReplayKey(diagnostic);
  return diagnostic;
}

Objc3LoweringDiagnostic Objc3MakeUnsupportedLoweringDiagnostic(
    const std::string &surface, unsigned line, unsigned column) {
  return Objc3MakeLoweringDiagnostic(
      "O3L300", Objc3LoweringDiagnosticSeverity::Error,
      "unsupported lowering surface: " + surface, line, column);
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
      << ";message=" << diagnostic.message;
  return out.str();
}
