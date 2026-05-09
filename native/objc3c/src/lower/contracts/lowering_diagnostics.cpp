#include "lower/contracts/lowering_diagnostics.h"

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
