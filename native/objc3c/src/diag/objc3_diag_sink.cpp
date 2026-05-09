#include "diag/objc3_diag_sink.h"

#include "diag/objc3_diag_render.h"

Objc3DiagnosticSink::Objc3DiagnosticSink(
    std::vector<std::string> &diagnostics)
    : diagnostics_(&diagnostics) {}

void Objc3DiagnosticSink::Emit(const Objc3DiagnosticPayload &payload) {
  diagnostics_->push_back(RenderDiagnosticPayload(payload));
}

void Objc3DiagnosticSink::EmitError(unsigned line,
                                    unsigned column,
                                    const std::string &code,
                                    const std::string &message) {
  Emit(MakeErrorDiagnosticPayload(line, column, code, message));
}

std::size_t Objc3DiagnosticSink::size() const {
  return diagnostics_->size();
}

bool Objc3DiagnosticSink::empty() const {
  return diagnostics_->empty();
}

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
