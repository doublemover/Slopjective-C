#include "parse/objc3_parser_diagnostics.h"

#include "diag/objc3_diag_utils.h"

namespace objc3c::parse {
namespace {

std::string EscapeObjc3DiagnosticMetadataValue(std::string_view value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (const char ch : value) {
    if (ch == '\\' || ch == '\'') {
      escaped.push_back('\\');
    }
    escaped.push_back(ch);
  }
  return escaped;
}

std::string AppendObjc3ParserFixItMetadata(
    const std::string &message,
    const Objc3ParserDiagnosticFixIt &fixit) {
  std::string enriched = message;
  enriched += " {fix-it:";
  enriched += "label='";
  enriched += EscapeObjc3DiagnosticMetadataValue(fixit.label);
  enriched += "';range=";
  enriched += std::to_string(fixit.start_line);
  enriched += ":";
  enriched += std::to_string(fixit.start_column);
  enriched += "-";
  enriched += std::to_string(fixit.end_line);
  enriched += ":";
  enriched += std::to_string(fixit.end_column);
  enriched += ";replacement='";
  enriched += EscapeObjc3DiagnosticMetadataValue(fixit.replacement);
  enriched += "';machine-applicable=";
  enriched += fixit.machine_applicable ? "true" : "false";
  enriched += "}";
  return enriched;
}

std::string AppendObjc3ParserRecoveryMetadata(
    const std::string &message,
    std::string_view strategy,
    std::string_view boundary) {
  std::string enriched = message;
  enriched += " {recovery:";
  enriched += "strategy='";
  enriched += EscapeObjc3DiagnosticMetadataValue(strategy);
  enriched += "';boundary='";
  enriched += EscapeObjc3DiagnosticMetadataValue(boundary);
  enriched += "';deterministic=true;recovery-counts-as-success=false}";
  return enriched;
}

}  // namespace

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

std::string BuildObjc3ParserDiagnosticWithFixIt(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message,
    const Objc3ParserDiagnosticFixIt &fixit) {
  return BuildObjc3ParserDiagnostic(
      token, code, AppendObjc3ParserFixItMetadata(message, fixit));
}

std::string BuildObjc3ParserDiagnosticWithRecovery(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message,
    std::string_view strategy,
    std::string_view boundary) {
  return BuildObjc3ParserDiagnostic(
      token, code, AppendObjc3ParserRecoveryMetadata(message, strategy, boundary));
}

}  // namespace objc3c::parse
