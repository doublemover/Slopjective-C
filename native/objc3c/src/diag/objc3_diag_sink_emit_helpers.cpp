#include "diag/objc3_diag_sink.h"

void EmitDiagnostic(std::vector<std::string> &diagnostics,
                    const Objc3DiagnosticPayload &payload) {
  Objc3DiagnosticSink sink(diagnostics);
  sink.Emit(payload);
}

void EmitErrorDiagnostic(std::vector<std::string> &diagnostics,
                         unsigned line,
                         unsigned column,
                         const std::string &code,
                         const std::string &message) {
  Objc3DiagnosticSink sink(diagnostics);
  sink.EmitError(line, column, code, message);
}
