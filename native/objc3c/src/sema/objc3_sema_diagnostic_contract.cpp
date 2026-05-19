#include "sema/objc3_sema_diagnostic_contract.h"

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "diag/objc3_diag_utils.h"

bool IsObjc3SemaOwnedDiagnosticCode(const char *code) {
  if (code == nullptr) {
    return false;
  }
  return Objc3RenderedDiagnosticCodeMatchesStage(
      Objc3FrontendDiagnosticStage::kSemantic,
      code);
}

std::string BuildObjc3SemaDiagnostic(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message) {
  return MakeDiag(line, column, code, message);
}
