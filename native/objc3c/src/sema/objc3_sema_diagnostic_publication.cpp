#include "sema/objc3_sema_diagnostic_publication.h"

bool Objc3SemaDiagnosticsBusHasSink(const Objc3SemaDiagnosticsBus &bus) {
  return bus.diagnostics != nullptr;
}

bool Objc3SemaDiagnosticsBusHasHardCutoverOwner(
    const Objc3SemaDiagnosticsBus &bus) {
  return Objc3SemaOwnerIsExplicit(bus.diagnostic_handoff_owner) &&
         Objc3SemaOwnerIsExplicit(bus.diagnostic_catalog_owner) &&
         Objc3SemaOwnerIsExplicit(bus.diagnostic_fixit_owner) &&
         Objc3SemaOwnerIsExplicit(bus.diagnostic_recovery_owner) &&
         bus.owner_model == kObjc3SemaNoFallbackOwnerModel &&
         bus.strict_no_fallback && bus.strict_no_compatibility &&
         !bus.recovery_counts_as_success &&
         Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kSemantic);
}

std::size_t Objc3SemaDiagnosticsBusCount(const Objc3SemaDiagnosticsBus &bus) {
  return bus.Count();
}

void PublishObjc3SemaDiagnostic(
    const Objc3SemaDiagnosticsBus &bus,
    const std::string &diagnostic) {
  if (!Objc3SemaDiagnosticsBusHasHardCutoverOwner(bus)) {
    return;
  }
  bus.Publish(diagnostic);
}

void PublishObjc3SemaDiagnostics(
    const Objc3SemaDiagnosticsBus &bus,
    const std::vector<std::string> &diagnostics) {
  if (!Objc3SemaDiagnosticsBusHasHardCutoverOwner(bus)) {
    return;
  }
  bus.PublishBatch(diagnostics);
}
