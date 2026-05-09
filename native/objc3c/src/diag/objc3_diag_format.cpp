#include "diag/objc3_diag_format.h"

#include "diag/objc3_diag_record.h"
#include "diag/objc3_diag_render.h"

std::string MakeDiag(unsigned line,
                     unsigned column,
                     const std::string &code,
                     const std::string &message) {
  return RenderDiagnosticPayload(
      MakeErrorDiagnosticPayload(line, column, code, message));
}
