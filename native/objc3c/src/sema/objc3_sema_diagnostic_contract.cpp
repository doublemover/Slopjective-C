#include "sema/objc3_sema_diagnostic_contract.h"

#include "diag/objc3_diag_utils.h"

std::string BuildObjc3SemaDiagnostic(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message) {
  return MakeDiag(line, column, code, message);
}
