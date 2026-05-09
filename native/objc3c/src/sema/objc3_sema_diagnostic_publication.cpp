#include "sema/objc3_sema_diagnostic_publication.h"

bool Objc3SemaDiagnosticsBusHasSink(const Objc3SemaDiagnosticsBus &bus) {
  return bus.diagnostics != nullptr;
}

std::size_t Objc3SemaDiagnosticsBusCount(const Objc3SemaDiagnosticsBus &bus) {
  return bus.Count();
}

void PublishObjc3SemaDiagnostic(
    const Objc3SemaDiagnosticsBus &bus,
    const std::string &diagnostic) {
  bus.Publish(diagnostic);
}

void PublishObjc3SemaDiagnostics(
    const Objc3SemaDiagnosticsBus &bus,
    const std::vector<std::string> &diagnostics) {
  bus.PublishBatch(diagnostics);
}
