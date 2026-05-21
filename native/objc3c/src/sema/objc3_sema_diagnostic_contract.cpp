#include "sema/objc3_sema_diagnostic_contract.h"

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "diag/objc3_diag_utils.h"

namespace {

std::string BuildObjc3SemaRecoveryMetadata(
    const std::string &message,
    const std::string &strategy,
    const std::string &boundary) {
  return message +
         " {recovery:strategy='" + strategy +
         "';boundary='" + boundary +
         "';deterministic=true;recovery-counts-as-success=false}";
}

}  // namespace

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

std::string BuildObjc3SemaDiagnosticWithRecovery(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message,
    const std::string &strategy,
    const std::string &boundary) {
  return BuildObjc3SemaDiagnostic(
      line, column, code,
      BuildObjc3SemaRecoveryMetadata(message, strategy, boundary));
}

std::string BuildObjc3SemaMissingReturnDiagnostic(
    unsigned line,
    unsigned column,
    const std::string &callable_context) {
  return BuildObjc3SemaDiagnosticWithRecovery(
      line,
      column,
      "O3S205",
      "missing return path in " + callable_context,
      "preserve-callable-body-and-continue-semantic-validation",
      "callable-body-end");
}
