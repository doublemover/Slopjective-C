#include "sema/objc3_sema_diagnostics_bus.h"

#include <algorithm>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "diag/objc3_diag_utils.h"

namespace {

bool IsObjc3SemaDiagnosticLess(const std::string &lhs, const std::string &rhs) {
  const DiagSortKey lhs_key = ParseDiagSortKey(lhs);
  const DiagSortKey rhs_key = ParseDiagSortKey(rhs);
  if (lhs_key.line != rhs_key.line) {
    return lhs_key.line < rhs_key.line;
  }
  if (lhs_key.column != rhs_key.column) {
    return lhs_key.column < rhs_key.column;
  }
  if (lhs_key.severity_rank != rhs_key.severity_rank) {
    return lhs_key.severity_rank < rhs_key.severity_rank;
  }
  if (lhs_key.code != rhs_key.code) {
    return lhs_key.code < rhs_key.code;
  }
  if (lhs_key.message != rhs_key.message) {
    return lhs_key.message < rhs_key.message;
  }
  return lhs_key.raw < rhs_key.raw;
}

}  // namespace

void CanonicalizeObjc3SemaPassDiagnostics(std::vector<std::string> &diagnostics) {
  std::stable_sort(diagnostics.begin(), diagnostics.end(), IsObjc3SemaDiagnosticLess);
}

bool AreObjc3SemaPassDiagnosticsCanonical(const std::vector<std::string> &diagnostics) {
  return std::is_sorted(diagnostics.begin(), diagnostics.end(), IsObjc3SemaDiagnosticLess);
}

bool AreObjc3SemaPassDiagnosticsHardCutoverOwned(
    const std::vector<std::string> &diagnostics) {
  if (!Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kSemantic)) {
    return false;
  }
  for (const std::string &diagnostic : diagnostics) {
    unsigned line = 0u;
    unsigned column = 0u;
    std::string code;
    if (!TryParseDiagnosticCoordinateAndCode(diagnostic, line, column, code) ||
        !Objc3RenderedDiagnosticCodeMatchesStage(
            Objc3FrontendDiagnosticStage::kSemantic,
            code)) {
      return false;
    }
  }
  return true;
}
