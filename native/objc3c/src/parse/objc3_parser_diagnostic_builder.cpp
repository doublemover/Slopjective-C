#include "parse/objc3_parser_diagnostics.h"

#include "diag/objc3_diag_utils.h"

namespace objc3c::parse {

bool IsObjc3ParserOwnedDiagnosticCode(const char *code) {
  if (code == nullptr) {
    return false;
  }
  return Objc3RenderedDiagnosticCodeMatchesStage(
      Objc3FrontendDiagnosticStage::kParser,
      code);
}

std::string BuildObjc3ParserDiagnostic(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message) {
  return MakeDiag(token.line, token.column, code, message);
}

}  // namespace objc3c::parse
